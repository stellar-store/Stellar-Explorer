#include "filetab.h"
#include "fileopworker.h"
#include "utils.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTreeWidgetItem>
#include <QStandardItemModel>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QMenu>
#include <QAction>
#include <QProgressDialog>
#include <QSettings>
#include <QShortcut>
#include <QKeySequence>
#include <QProcess>
#include <QLabel>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QDialog>
#include <QPushButton>
#include <QTextStream>
#include <QAbstractItemView>
#include <QStandardPaths>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// -----------------------------------------------------------------------
FileTab::FileTab(const QString &startPath, QWidget *parent)
    : QWidget(parent)
    , m_historyIndex(0)
{
    QDir dir(startPath);
    m_currentPath = dir.exists() ? dir.canonicalPath() : QDir::homePath();

    QVBoxLayout *v = new QVBoxLayout(this);
    v->setContentsMargins(2, 2, 2, 2);

    // Barra superior: breadcrumbs + búsqueda
    QHBoxLayout *top = new QHBoxLayout();
    m_breadcrumbs = new Breadcrumbs(m_currentPath, this);
    connect(m_breadcrumbs, &Breadcrumbs::pathChanged, this, &FileTab::setDirectory);
    top->addWidget(m_breadcrumbs, 1);

    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Buscar en esta carpeta… (Enter)");
    connect(m_searchEdit, &QLineEdit::returnPressed, this, &FileTab::startSearch);
    top->addWidget(m_searchEdit);
    v->addLayout(top);

    // Splitter: árbol izquierda + lista derecha
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    v->addWidget(splitter, 1);

    // Árbol lateral
    m_tree = new QTreeWidget(this);
    m_tree->setHeaderHidden(true);
    m_tree->setMaximumWidth(220);
    splitter->addWidget(m_tree);
    buildSideTree();

    // Modelo de archivos y vista
    m_fileModel = new QFileSystemModel(this);
    m_fileModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs);
    m_fileModel->setRootPath(m_currentPath);

    m_view = new QListView(this);
    m_view->setModel(m_fileModel);
    m_view->setRootIndex(m_fileModel->index(m_currentPath));
    m_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_view->setDragEnabled(true);
    m_view->setAcceptDrops(true);
    m_view->setDragDropMode(QAbstractItemView::DragDrop);
    m_view->setDefaultDropAction(Qt::MoveAction);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_view, &QListView::doubleClicked,  this, &FileTab::onDoubleClick);
    connect(m_view, &QListView::customContextMenuRequested, this, &FileTab::contextMenu);
    splitter->addWidget(m_view);

    splitter->setSizes({220, 600});

    // Portapapeles
    m_clipboardPaths.clear();
    m_clipboardMode.clear();

    // Atajo atrás
    QShortcut *backShortcut = new QShortcut(QKeySequence(Qt::Key_Back), this);
    connect(backShortcut, &QShortcut::activated, this, [this](){ goBack(); });

    // Historial
    m_history.append(m_currentPath);
    m_historyIndex = 0;

    applySettings();

    // Barra inferior con slider de tamaño de iconos
    QHBoxLayout *bottomBar = new QHBoxLayout();
    bottomBar->addStretch(1);
    bottomBar->addWidget(new QLabel("Tamaño de iconos:", this));
    m_iconSlider = new QSlider(Qt::Horizontal, this);
    m_iconSlider->setMinimum(16);
    m_iconSlider->setMaximum(128);
    QSettings settings;
    int iconSize = settings.value("icon_size", 48).toInt();
    m_iconSlider->setValue(iconSize);
    m_view->setIconSize(QSize(iconSize, iconSize));
    m_iconSlider->setTickInterval(8);
    m_iconSlider->setTickPosition(QSlider::TicksBelow);
    m_iconSlider->setToolTip("Tamaño de iconos");
    connect(m_iconSlider, &QSlider::valueChanged, this, &FileTab::changeIconSize);
    bottomBar->addWidget(m_iconSlider);
    v->addLayout(bottomBar);
}

// -----------------------------------------------------------------------
// Árbol lateral
// -----------------------------------------------------------------------
void FileTab::buildSideTree()
{
    QString home = QDir::homePath();

    // --- Grupo Ubicaciones ---
    QTreeWidgetItem *ubicaciones = new QTreeWidgetItem(QStringList{"Ubicaciones"});
    ubicaciones->setIcon(0, QIcon::fromTheme("folder"));
    m_tree->addTopLevelItem(ubicaciones);

    struct Acceso { QString nombre; QString ruta; QString iconName; };
    const QList<Acceso> accesos = {
        {"Carpeta Personal",  home,                          "user-home"},
        {"Escritorio",        home + "/Escritorio",          "user-desktop"},
        {"Documentos",        home + "/Documentos",          "folder-documents"},
        {"Música",            home + "/Música",              "folder-music"},
        {"Imágenes",          home + "/Imágenes",            "folder-pictures"},
        {"Videos",            home + "/Videos",              "folder-videos"},
        {"Descargas",         home + "/Descargas",           "folder-download"},
        {"Papelera",          "trash://",                    "user-trash"},
    };

    for (const Acceso &a : accesos) {
        QTreeWidgetItem *item = new QTreeWidgetItem(QStringList{a.nombre});
        item->setIcon(0, QIcon::fromTheme(a.iconName));
        item->setData(0, Qt::UserRole, a.ruta);
        ubicaciones->addChild(item);
    }

    // --- Grupo Dispositivos ---
    QTreeWidgetItem *dispositivos = new QTreeWidgetItem(QStringList{"Dispositivos"});
    dispositivos->setIcon(0, QIcon::fromTheme("drive-harddisk"));
    dispositivos->setData(0, Qt::UserRole, "/media");
    m_tree->addTopLevelItem(dispositivos);
    dispositivos->addChild(new QTreeWidgetItem(QStringList{"Cargando..."}));

    // --- Grupo Red ---
    QTreeWidgetItem *red = new QTreeWidgetItem(QStringList{"Red"});
    red->setIcon(0, QIcon::fromTheme("network-workgroup"));
    red->setData(0, Qt::UserRole, "/run/user/1000/gvfs");
    m_tree->addTopLevelItem(red);
    red->addChild(new QTreeWidgetItem(QStringList{"Cargando..."}));

    m_tree->expandItem(ubicaciones);

    connect(m_tree, &QTreeWidget::itemClicked,
            this, &FileTab::onTreeItemClicked);
    m_tree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_tree, &QTreeWidget::customContextMenuRequested,
            this, &FileTab::onTreeContextMenu);
    connect(m_tree, &QTreeWidget::itemExpanded,
            this, &FileTab::onTreeItemExpanded);
}

// -----------------------------------------------------------------------
// Navegación
// -----------------------------------------------------------------------
void FileTab::setDirectory(const QString &path)
{
    if (path == "trash://") {
        showTrashView();
        return;
    }

    QFileInfo fi(path);

    // Verificar permiso de lectura
    if (!fi.isReadable()) {
        // Puede ser un dispositivo que necesita montar
        if (path.startsWith("/dev/") || path.startsWith("/media/")) {
            if (!mountWithAuth(path))
                return;
        } else {
            QMessageBox::warning(this, "Error de permisos",
                QString("No tienes permisos para acceder a:\n%1\n\n"
                        "Intenta ejecutar el programa con privilegios elevados.").arg(path));
            emit statusMessage("Error de permisos: " + path);
            return;
        }
    }

    if (!fi.exists()) {
        emit statusMessage("Ruta no existe");
        return;
    }

    m_currentPath = QDir(path).canonicalPath();

    // Restaurar modelo si se cambió para la papelera
    if (m_view->model() != m_fileModel) {
        m_view->setModel(m_fileModel);
        connect(m_view, &QListView::doubleClicked,
                this, &FileTab::onDoubleClick);
    }

    m_fileModel->setRootPath(m_currentPath);
    m_view->setRootIndex(m_fileModel->index(m_currentPath));
    m_breadcrumbs->setPath(m_currentPath);
    emit statusMessage(m_currentPath);

    // Actualizar historial
    if (m_historyIndex < m_history.size() - 1)
        m_history = m_history.mid(0, m_historyIndex + 1);
    m_history.append(m_currentPath);
    m_historyIndex = m_history.size() - 1;
}

void FileTab::goUp()
{
    QDir dir(m_currentPath);
    dir.cdUp();
    setDirectory(dir.absolutePath());
}

bool FileTab::goBack()
{
    if (m_historyIndex > 0) {
        m_historyIndex--;
        m_currentPath = m_history[m_historyIndex];
        m_fileModel->setRootPath(m_currentPath);
        m_view->setRootIndex(m_fileModel->index(m_currentPath));
        m_breadcrumbs->setPath(m_currentPath);
        emit statusMessage(m_currentPath);
        return true;
    }
    return false;
}

bool FileTab::goForward()
{
    if (m_historyIndex < m_history.size() - 1) {
        m_historyIndex++;
        m_currentPath = m_history[m_historyIndex];
        m_fileModel->setRootPath(m_currentPath);
        m_view->setRootIndex(m_fileModel->index(m_currentPath));
        m_breadcrumbs->setPath(m_currentPath);
        emit statusMessage(m_currentPath);
        return true;
    }
    return false;
}

void FileTab::refresh()
{
    QString path = m_currentPath;
    m_fileModel->setRootPath("");      // forzar recarga
    m_fileModel->setRootPath(path);
    m_view->setRootIndex(m_fileModel->index(path));
}

// -----------------------------------------------------------------------
// Árbol — slots
// -----------------------------------------------------------------------
void FileTab::onTreeItemClicked(QTreeWidgetItem *item, int /*column*/)
{
    QString path = item->data(0, Qt::UserRole).toString();
    if (path.isEmpty()) return;
    setDirectory(path);
}

void FileTab::onTreeItemExpanded(QTreeWidgetItem *item)
{
    QString path = item->data(0, Qt::UserRole).toString();
    if (path == "/media") {
        refreshDevices(item);
    } else if (path == "/run/user/1000/gvfs") {
        qDeleteAll(item->takeChildren());
        QDir gvfs("/run/user/1000/gvfs");
        if (gvfs.exists()) {
            for (const QString &name : gvfs.entryList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
                QTreeWidgetItem *net = new QTreeWidgetItem(QStringList{name});
                net->setIcon(0, QIcon::fromTheme("network-server"));
                net->setData(0, Qt::UserRole, gvfs.absoluteFilePath(name));
                item->addChild(net);
            }
        }
        if (item->childCount() == 0)
            item->addChild(new QTreeWidgetItem(QStringList{"(Sin recursos de red)"}));
    }
}

void FileTab::refreshDevices(QTreeWidgetItem *item)
{
    qDeleteAll(item->takeChildren());

    QProcess proc;
    proc.start("lsblk", {"-o", "NAME,SIZE,LABEL,MOUNTPOINT", "--json"});
    proc.waitForFinished(3000);

    if (proc.exitCode() != 0) {
        item->addChild(new QTreeWidgetItem(QStringList{"(Sin dispositivos)"}));
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(proc.readAllStandardOutput());
    QJsonArray blockdevices = doc.object()["blockdevices"].toArray();

    for (const QJsonValue &val : blockdevices) {
        QJsonObject dev = val.toObject();
        QString type = dev["type"].toString();
        if (type == "loop" || type == "rom") continue;  // Excluir loops y CDs

        QString devName   = "/dev/" + dev["name"].toString();
        QString label     = dev["label"].toString();
        QString size      = dev["size"].toString();
        QString mountpoint = dev["mountpoint"].toString();

        if (!mountpoint.isEmpty()) {
            QString display = devName;
            if (!label.isEmpty())      display += " (" + label + ")";
            if (!size.isEmpty())       display += " - " + size;
            display += " [Montado en " + mountpoint + "]";

            QTreeWidgetItem *devItem = new QTreeWidgetItem(QStringList{display});
            devItem->setIcon(0, QIcon::fromTheme("drive-harddisk"));
            devItem->setData(0, Qt::UserRole, mountpoint);
            item->addChild(devItem);
        }

        // Particiones hijas
        for (const QJsonValue &cval : dev["children"].toArray()) {
            QJsonObject ch = cval.toObject();
            QString cName  = "/dev/" + ch["name"].toString();
            QString cLabel = ch["label"].toString();
            QString cSize  = ch["size"].toString();
            QString cMount = ch["mountpoint"].toString();

            if (!cMount.isEmpty()) {
                QString cDisplay = cName;
                if (!cLabel.isEmpty()) cDisplay += " (" + cLabel + ")";
                if (!cSize.isEmpty())  cDisplay += " - " + cSize;
                cDisplay += " [Montado en " + cMount + "]";

                QTreeWidgetItem *chItem = new QTreeWidgetItem(QStringList{cDisplay});
                chItem->setIcon(0, QIcon::fromTheme("drive-harddisk"));
                chItem->setData(0, Qt::UserRole, cMount);
                item->addChild(chItem);
            }
        }
    }

    if (item->childCount() == 0)
        item->addChild(new QTreeWidgetItem(QStringList{"(Sin dispositivos)"}));
}

// -----------------------------------------------------------------------
// Papelera
// -----------------------------------------------------------------------
void FileTab::showTrashView()
{
    QString trashFiles = QDir::homePath() + "/.local/share/Trash/files";
    QDir dir(trashFiles);

    if (!dir.exists()) {
        emit statusMessage("La Papelera está vacía.");
        return;
    }

    QFileIconProvider iconProvider;
    QStandardItemModel *model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"Nombre", "Ruta"});

    for (const QFileInfo &fi : dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
        QStandardItem *nameItem = new QStandardItem(fi.fileName());
        QStandardItem *pathItem = new QStandardItem(fi.absoluteFilePath());
        QIcon icon = fi.isDir()
            ? iconProvider.icon(QFileIconProvider::Folder)
            : iconProvider.icon(fi);
        nameItem->setIcon(icon);
        model->appendRow({nameItem, pathItem});
    }

    // Reemplazar modelo de la vista
    m_view->setModel(model);
    emit statusMessage(QString("Papelera: %1 (%2 elemento(s))")
                       .arg(trashFiles).arg(model->rowCount()));

    // Reconectar señales de la vista para el contexto de papelera
    disconnect(m_view, &QListView::customContextMenuRequested, nullptr, nullptr);
    connect(m_view, &QListView::customContextMenuRequested,
            this, [this, model](const QPoint &pos){
                trashContextMenu(pos, model);
            });

    disconnect(m_view, &QListView::doubleClicked, nullptr, nullptr);
    connect(m_view, &QListView::doubleClicked,
            this, [model](const QModelIndex &idx){
                if (!idx.isValid()) return;
                QString path = model->item(idx.row(), 1)->text();
                openWithDefault(path);
            });
}

void FileTab::trashContextMenu(const QPoint &pos, QStandardItemModel *model)
{
    QModelIndex idx = m_view->indexAt(pos);
    if (!idx.isValid()) return;

    QString filePath = model->item(idx.row(), 1)->text();

    QMenu menu(m_view);
    QAction *actOpen    = menu.addAction("Abrir");
    QAction *actRestore = menu.addAction("Restaurar");
    QAction *actDelete  = menu.addAction("Eliminar definitivamente");

    QAction *action = menu.exec(m_view->viewport()->mapToGlobal(pos));
    if (!action) return;

    if (action == actOpen)    openWithDefault(filePath);
    else if (action == actRestore) restoreFromTrash(filePath);
    else if (action == actDelete)  deletePermanently(filePath);
}

void FileTab::restoreFromTrash(const QString &filePath)
{
    QString infoPath = QDir::homePath() + "/.local/share/Trash/info/"
                       + QFileInfo(filePath).fileName() + ".trashinfo";
    QFile infoFile(infoPath);
    if (!infoFile.exists()) {
        QMessageBox::warning(this, "Restaurar", "No se encontró información de restauración.");
        return;
    }

    QString origPath;
    infoFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream ts(&infoFile);
    while (!ts.atEnd()) {
        QString line = ts.readLine();
        if (line.startsWith("Path=")) {
            origPath = line.mid(5).trimmed();
            break;
        }
    }
    infoFile.close();

    if (origPath.isEmpty()) {
        QMessageBox::warning(this, "Restaurar", "No se encontró la ruta original.");
        return;
    }

    QFileInfo origInfo(origPath);
    if (origInfo.exists()) {
        auto ans = QMessageBox::question(this, "Restaurar",
            QString("El archivo original existe:\n%1\n¿Sobrescribir?").arg(origPath));
        if (ans != QMessageBox::Yes) return;
        if (origInfo.isDir()) QDir(origPath).removeRecursively();
        else QFile::remove(origPath);
    }

    if (QFile::rename(filePath, origPath)) {
        QFile::remove(infoPath);
        showTrashView();
        QMessageBox::information(this, "Restaurar",
            QString("Archivo restaurado a:\n%1").arg(origPath));
    } else {
        QMessageBox::warning(this, "Restaurar", "No se pudo restaurar el archivo.");
    }
}

void FileTab::deletePermanently(const QString &filePath)
{
    QString infoPath = QDir::homePath() + "/.local/share/Trash/info/"
                       + QFileInfo(filePath).fileName() + ".trashinfo";

    QFileInfo fi(filePath);
    bool ok = fi.isDir() ? QDir(filePath).removeRecursively() : QFile::remove(filePath);

    if (ok) {
        QFile::remove(infoPath);
        showTrashView();
        QMessageBox::information(this, "Eliminar", "Archivo eliminado definitivamente.");
    } else {
        QMessageBox::warning(this, "Eliminar", "No se pudo eliminar el archivo.");
    }
}

// -----------------------------------------------------------------------
// Vista doble clic
// -----------------------------------------------------------------------
void FileTab::onDoubleClick(const QModelIndex &index)
{
    QString path = m_fileModel->filePath(index);
    if (QFileInfo(path).isDir())
        setDirectory(path);
    else if (QFileInfo(path).isExecutable() ||
             path.endsWith(".appimage", Qt::CaseInsensitive) ||
             path.endsWith(".run", Qt::CaseInsensitive) ||
             path.endsWith(".sh", Qt::CaseInsensitive))
        runExecutable(path);
    else
        openWithDefault(path);
}

// -----------------------------------------------------------------------
// Menú contextual de la vista de archivos
// -----------------------------------------------------------------------
void FileTab::contextMenu(const QPoint &pos)
{
    QMenu menu(this);
    QModelIndex idx = m_view->indexAt(pos);
    QString path = idx.isValid() ? m_fileModel->filePath(idx) : m_currentPath;

    QAction *actRun = nullptr;

    if (idx.isValid()) {
        QAction *actOpen     = menu.addAction("Abrir");
        QAction *actOpenWith = menu.addAction("Abrir con...");

        // Opción ejecutar para scripts y ejecutables
        QFileInfo fi(path);
        if (fi.suffix() == "sh" || fi.suffix() == "appimage" || fi.suffix() == "run" || fi.isExecutable())
            actRun = menu.addAction("Ejecutar en terminal");

        menu.addSeparator();
        QAction *actCopy   = menu.addAction("Copiar");
        QAction *actCut    = menu.addAction("Cortar");
        QAction *actPaste  = menu.addAction("Pegar");
        menu.addSeparator();
        QAction *actRename = menu.addAction("Renombrar");
        QAction *actDelete = menu.addAction("Eliminar (Papelera)");
        menu.addSeparator();
        QAction *actMoveTo = menu.addAction("Mover a...");
        QAction *actProps  = menu.addAction("Propiedades");
        menu.addSeparator();
        QAction *actTerm   = menu.addAction("Abrir terminal aquí");

        QAction *action = menu.exec(m_view->viewport()->mapToGlobal(pos));
        if (!action) return;

        if      (actRun   && action == actRun)    runInTerminal(path);
        else if (action == actOpen) {
            if (QFileInfo(path).isDir()) setDirectory(path);
            else openWithDefault(path);
        }
        else if (action == actOpenWith) openWithDialog(path);
        else if (action == actCopy)     copySelected();
        else if (action == actCut)      cutSelected();
        else if (action == actPaste)    pasteIntoCurrent();
        else if (action == actRename)   renameSelected();
        else if (action == actDelete)   deleteSelected();
        else if (action == actMoveTo)   moveToDialog();
        else if (action == actProps)    showPropertiesDialog(path);
        else if (action == actTerm)     openTerminalHere(m_currentPath);
    } else {
        // Menú de fondo (sin selección)
        QAction *actPaste = menu.addAction("Pegar");
        QAction *actNew   = menu.addAction("Carpeta nueva");
        QAction *actTerm  = menu.addAction("Abrir terminal aquí");
        QAction *action   = menu.exec(m_view->viewport()->mapToGlobal(pos));
        if (!action) return;
        if      (action == actPaste) pasteIntoCurrent();
        else if (action == actNew)   newFolder();
        else if (action == actTerm)  openTerminalHere(m_currentPath);
    }
}

// -----------------------------------------------------------------------
// Menú contextual del árbol lateral
// -----------------------------------------------------------------------
void FileTab::onTreeContextMenu(const QPoint &pos)
{
    QTreeWidgetItem *item = m_tree->itemAt(pos);
    if (!item) return;

    QString path = item->data(0, Qt::UserRole).toString();
    QMenu menu(m_tree);

    if (path == "/media") {
        QAction *actRefresh = menu.addAction("Actualizar dispositivos");
        QAction *action = menu.exec(m_tree->viewport()->mapToGlobal(pos));
        if (action == actRefresh) refreshDevices(item);
    } else if (path.startsWith("/dev/")) {
        handleTreeDeviceMenu(item, path);
    } else {
        QAction *actOpen = menu.addAction("Abrir");
        QAction *actTab  = menu.addAction("Abrir en una pestaña nueva");
        QAction *actWin  = menu.addAction("Abrir en una ventana nueva");
        QAction *action  = menu.exec(m_tree->viewport()->mapToGlobal(pos));
        if (!action) return;
        if      (action == actOpen) setDirectory(path);
        else if (action == actTab)  emit requestOpenPath(path);
        else if (action == actWin)  QProcess::startDetached(QApplication::applicationFilePath(), {path});
    }
}

void FileTab::handleTreeDeviceMenu(QTreeWidgetItem *item, const QString &path)
{
    bool mounted = isDeviceMounted(path);

    QMenu menu(m_tree);
    QAction *actOpen = menu.addAction("Abrir");
    QAction *actTab  = menu.addAction("Abrir en una pestaña nueva");
    menu.addSeparator();

    QAction *actMount   = nullptr;
    QAction *actUnmount = nullptr;
    if (mounted) {
        actUnmount = menu.addAction("Desmontar");
        actUnmount->setIcon(QIcon::fromTheme("media-eject"));
    } else {
        actMount = menu.addAction("Montar");
        actMount->setIcon(QIcon::fromTheme("media-mount"));
    }

    QAction *action = menu.exec(m_tree->viewport()->mapToGlobal(
        m_tree->visualItemRect(item).center()));
    if (!action) return;

    if (action == actOpen) {
        if (mounted || mountDevice(path)) setDirectory(path);
    } else if (action == actTab) {
        if (mounted || mountDevice(path)) emit requestOpenPath(path);
    } else if (actUnmount && action == actUnmount) {
        unmountDevice(path);
        refreshDevices(item->parent());
    } else if (actMount && action == actMount) {
        if (mountDevice(path)) {
            refreshDevices(item->parent());
            emit statusMessage("Dispositivo montado: " + path);
        }
    }
}

// -----------------------------------------------------------------------
// Dispositivos
// -----------------------------------------------------------------------
bool FileTab::isDeviceMounted(const QString &devicePath) const
{
    QFile mounts("/proc/mounts");
    if (!mounts.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QTextStream ts(&mounts);
    while (!ts.atEnd()) {
        if (ts.readLine().contains(devicePath))
            return true;
    }
    return false;
}

bool FileTab::mountDevice(const QString &devicePath)
{
    QString script = "/tmp/mount_script.sh";
    QFile f(script);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream ts(&f);
    ts << "#!/bin/bash\n"
       << "udisksctl mount -b " << devicePath << "\n";
    f.close();
    f.setPermissions(QFileDevice::ExeOwner | QFileDevice::ReadOwner | QFileDevice::WriteOwner);

    QProcess proc;
    proc.start("pkexec", {script});
    proc.waitForFinished(15000);
    f.remove();

    if (proc.exitCode() == 0) {
        emit statusMessage("Dispositivo montado: " + devicePath);
        return true;
    }
    QMessageBox::warning(this, "Error al montar",
        QString("No se pudo montar %1:\n%2").arg(devicePath,
            QString::fromLocal8Bit(proc.readAllStandardError())));
    return false;
}

void FileTab::unmountDevice(const QString &devicePath)
{
    QString script = "/tmp/unmount_script.sh";
    QFile f(script);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream ts(&f);
    ts << "#!/bin/bash\n"
       << "udisksctl unmount -b " << devicePath << "\n";
    f.close();
    f.setPermissions(QFileDevice::ExeOwner | QFileDevice::ReadOwner | QFileDevice::WriteOwner);

    QProcess proc;
    proc.start("pkexec", {script});
    proc.waitForFinished(15000);
    f.remove();

    if (proc.exitCode() == 0)
        emit statusMessage("Dispositivo desmontado: " + devicePath);
    else
        QMessageBox::warning(this, "Error al desmontar",
            QString::fromLocal8Bit(proc.readAllStandardError()));
}

bool FileTab::mountWithAuth(const QString &devicePath)
{
    QDialog authDialog(this);
    authDialog.setWindowTitle("Autenticación requerida");
    QVBoxLayout *layout = new QVBoxLayout(&authDialog);

    QHBoxLayout *header = new QHBoxLayout();
    QLabel *iconLabel = new QLabel(&authDialog);
    iconLabel->setPixmap(QIcon::fromTheme("dialog-password").pixmap(48, 48));
    header->addWidget(iconLabel);

    QLabel *msg = new QLabel(
        QString("Se requiere autenticación para acceder al dispositivo:\n%1\n\n"
                "Este dispositivo requiere permisos de administrador para acceder.")
        .arg(devicePath), &authDialog);
    msg->setWordWrap(true);
    header->addWidget(msg);
    layout->addLayout(header);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Yes | QDialogButtonBox::No, &authDialog);
    buttons->button(QDialogButtonBox::Yes)->setText("Dar permisos");
    buttons->button(QDialogButtonBox::No)->setText("Cancelar");
    connect(buttons, &QDialogButtonBox::accepted, &authDialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &authDialog, &QDialog::reject);
    layout->addWidget(buttons);

    if (authDialog.exec() != QDialog::Accepted)
        return false;

    return mountDevice(devicePath);
}

// -----------------------------------------------------------------------
// Operaciones con archivos
// -----------------------------------------------------------------------
void FileTab::newFolder()
{
    bool ok;
    QString name = QInputDialog::getText(this, "Carpeta nueva", "Nombre:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;
    if (!QDir(m_currentPath).mkdir(name)) {
        QMessageBox::warning(this, "Error", "No se pudo crear la carpeta.");
    } else {
        refresh();
    }
}

void FileTab::renameSelected()
{
    QStringList paths = selectedPaths();
    if (paths.size() != 1) {
        QMessageBox::information(this, "Renombrar", "Selecciona un único elemento");
        return;
    }
    QFileInfo fi(paths[0]);
    bool ok;
    QString name = QInputDialog::getText(this, "Renombrar", "Nuevo nombre:",
        QLineEdit::Normal, fi.fileName(), &ok);
    if (!ok || name.isEmpty() || name == fi.fileName()) return;
    if (!QFile::rename(paths[0], fi.dir().absoluteFilePath(name))) {
        QMessageBox::warning(this, "Error", "No se pudo renombrar.");
    } else {
        refresh();
    }
}

void FileTab::deleteSelected()
{
    QStringList paths = selectedPaths();
    if (paths.isEmpty()) return;

    auto ans = QMessageBox::question(this, "Eliminar",
        QString("¿Enviar %1 elemento(s) a la Papelera?").arg(paths.size()));
    if (ans != QMessageBox::Yes) return;

    for (const QString &p : paths) {
        // Mover a la carpeta Trash (equivalente a send2trash)
        QString trashDir = QDir::homePath() + "/.local/share/Trash/files/";
        QString infoDir  = QDir::homePath() + "/.local/share/Trash/info/";
        QDir().mkpath(trashDir);
        QDir().mkpath(infoDir);

        QFileInfo fi(p);
        QString dest = trashDir + fi.fileName();

        // Escribir .trashinfo
        QFile info(infoDir + fi.fileName() + ".trashinfo");
        if (info.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream ts(&info);
            ts << "[Trash Info]\nPath=" << p << "\nDeletionDate="
               << QDateTime::currentDateTime().toString(Qt::ISODate) << "\n";
            info.close();
        }

        if (!QFile::rename(p, dest)) {
            QMessageBox::warning(this, "Error",
                QString("No se pudo enviar a Papelera:\n%1").arg(p));
        }
    }
    refresh();
}

QStringList FileTab::selectedPaths() const
{
    QStringList result;
    for (const QModelIndex &idx : m_view->selectionModel()->selectedIndexes())
        result << m_fileModel->filePath(idx);
    return result;
}

void FileTab::copySelected()
{
    m_clipboardPaths = selectedPaths();
    m_clipboardMode  = "copy";
    emit statusMessage(QString("Copiados %1 elemento(s)").arg(m_clipboardPaths.size()));
}

void FileTab::cutSelected()
{
    m_clipboardPaths = selectedPaths();
    m_clipboardMode  = "cut";
    emit statusMessage(QString("Cortados %1 elemento(s)").arg(m_clipboardPaths.size()));
}

void FileTab::pasteIntoCurrent()
{
    if (m_clipboardPaths.isEmpty()) return;

    FileOpWorker::Operation op = (m_clipboardMode == "cut")
        ? FileOpWorker::Move : FileOpWorker::Copy;

    FileOpWorker *worker = new FileOpWorker(op, m_clipboardPaths, m_currentPath, this);

    QProgressDialog *dlg = new QProgressDialog(
        (op == FileOpWorker::Move ? "Moviendo…" : "Copiando…"),
        "Cancelar", 0, 100, this);
    dlg->setWindowTitle("Progreso");
    dlg->setAutoClose(true);

    connect(worker, &FileOpWorker::progress,
            this, [dlg](int pct, const QString &){ dlg->setValue(pct); });
    connect(worker, &FileOpWorker::finishedOk,
            this, [this, dlg](){
                dlg->setValue(100);
                refresh();
            });
    connect(worker, &FileOpWorker::finishedError,
            this, [this](const QString &err){
                QMessageBox::critical(this, "Error", err);
            });
    connect(worker, &FileOpWorker::finished, worker, &QObject::deleteLater);

    worker->start();
    dlg->exec();

    m_clipboardPaths.clear();
    m_clipboardMode.clear();
}

// -----------------------------------------------------------------------
// Búsqueda
// -----------------------------------------------------------------------
void FileTab::startSearch()
{
    QString term = m_searchEdit->text().trimmed();
    if (term.isEmpty()) return;

    emit statusMessage(QString("Buscando '%1' en %2…").arg(term, m_currentPath));

    QStringList results;
    QDirIterator it(m_currentPath,
                    QDir::AllEntries | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        if (it.fileName().contains(term, Qt::CaseInsensitive))
            results << it.filePath();
    }

    emit statusMessage(results.isEmpty()
        ? "0 resultados"
        : QString("Encontrados %1 resultado(s)").arg(results.size()));
}

// -----------------------------------------------------------------------
// Configuración
// -----------------------------------------------------------------------
void FileTab::applySettings()
{
    QSettings settings;
    bool showHidden  = settings.value("show_hidden", false).toBool();
    QString viewMode = settings.value("view_mode", "Lista").toString();
    bool autoExec    = settings.value("auto_exec", false).toBool();

    QDir::Filters filters = QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs;
    if (showHidden) filters |= QDir::Hidden;
    m_fileModel->setFilter(filters);

    if (viewMode == "Iconos") {
        m_view->setViewMode(QListView::IconMode);
        m_view->setIconSize(QSize(48, 48));
        m_view->setGridSize(QSize(80, 80));
        m_view->setSpacing(10);
    } else if (viewMode == "Iconos grandes") {
        m_view->setViewMode(QListView::IconMode);
        m_view->setIconSize(QSize(96, 96));
        m_view->setGridSize(QSize(120, 120));
        m_view->setSpacing(20);
    } else {
        m_view->setViewMode(QListView::ListMode);
        m_view->setIconSize(QSize(16, 16));
        m_view->setGridSize(QSize());
        m_view->setSpacing(0);
    }

    if (autoExec) applyExecPermissions();
    refresh();
}

void FileTab::applyExecPermissions()
{
    QSettings settings;
    QStringList exts = settings.value("exec_extensions", "sh py bash pl rb")
                               .toString().split(' ', Qt::SkipEmptyParts);
    QDirIterator it(m_currentPath, QDir::Files | QDir::NoDotAndDotDot);
    while (it.hasNext()) {
        it.next();
        QString suffix = it.fileInfo().suffix();
        if (exts.contains(suffix)) {
            QFile::setPermissions(it.filePath(),
                it.fileInfo().permissions()
                | QFileDevice::ExeOwner
                | QFileDevice::ExeGroup
                | QFileDevice::ExeOther);
        }
    }
}

void FileTab::changeIconSize(int value)
{
    m_view->setIconSize(QSize(value, value));
    if (m_view->viewMode() == QListView::IconMode)
        m_view->setGridSize(QSize(value + 32, value + 32));

    QSettings settings;
    settings.setValue("icon_size", value);
}

// -----------------------------------------------------------------------
// Acciones de menú contextual extra
// -----------------------------------------------------------------------
void FileTab::runInTerminal(const QString &path)
{
    QFileInfo fi(path);
    if (!fi.isExecutable()) {
        QFile::setPermissions(path,
            fi.permissions()
            | QFileDevice::ExeOwner
            | QFileDevice::ExeGroup
            | QFileDevice::ExeOther);
    }

    QStringList candidates = {"x-terminal-emulator", "gnome-terminal", "konsole", "xterm"};
    for (const QString &term : candidates) {
        QString found = QStandardPaths::findExecutable(term);
        if (!found.isEmpty()) {
            QProcess::startDetached(found, {"-e",
                QString("bash -c '%1; echo; read -p \"Presione Enter para cerrar...\"'").arg(path)});
            return;
        }
    }
    QMessageBox::critical(this, "Error", "No se encontró un emulador de terminal.");
}

void FileTab::openWithDialog(const QString &path)
{
    // Obtener tipo MIME con xdg-mime
    QProcess proc;
    proc.start("xdg-mime", {"query", "filetype", path});
    proc.waitForFinished(3000);
    QString mimeType = QString::fromLocal8Bit(proc.readAllStandardOutput()).trimmed();

    // Buscar archivos .desktop
    QStringList desktopDirs = {
        QDir::homePath() + "/.local/share/applications",
        "/usr/share/applications",
        "/usr/local/share/applications"
    };

    struct AppEntry { QString name; QString exec; QString icon; };
    QList<AppEntry> apps;

    for (const QString &ddir : desktopDirs) {
        QDir dir(ddir);
        if (!dir.exists()) continue;
        for (const QFileInfo &f : dir.entryInfoList({"*.desktop"}, QDir::Files)) {
            QFile file(f.absoluteFilePath());
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) continue;
            QString content = QString::fromUtf8(file.readAll());
            file.close();
            if (!content.contains("MimeType=")) continue;
            AppEntry e;
            for (const QString &line : content.split('\n')) {
                if (line.startsWith("Name="))    e.name = line.mid(5).trimmed();
                else if (line.startsWith("Exec=")) e.exec = line.mid(5).trimmed();
                else if (line.startsWith("Icon=")) e.icon = line.mid(5).trimmed();
            }
            if (!e.name.isEmpty() && !e.exec.isEmpty())
                apps.append(e);
        }
    }

    // Diálogo
    QDialog dialog(this);
    dialog.setWindowTitle("Abrir con...");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->addWidget(new QLabel("Selecciona una aplicación:"));

    QListWidget *listw = new QListWidget(&dialog);
    for (const AppEntry &a : apps) {
        QListWidgetItem *item = new QListWidgetItem(a.name);
        if (!a.icon.isEmpty())
            item->setIcon(QIcon::fromTheme(a.icon));
        item->setData(Qt::UserRole, a.exec);
        listw->addItem(item);
    }
    layout->addWidget(listw);

    QPushButton *browseBtn = new QPushButton("Buscar otra aplicación...", &dialog);
    layout->addWidget(browseBtn);
    connect(browseBtn, &QPushButton::clicked, &dialog, [&]{
        QString file = QFileDialog::getOpenFileName(&dialog,
            "Seleccionar aplicación", "/usr/bin", "Todos los archivos (*)");
        if (!file.isEmpty()) {
            QListWidgetItem *item = new QListWidgetItem(file);
            item->setData(Qt::UserRole, file);
            listw->addItem(item);
            listw->setCurrentItem(item);
        }
    });

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);

    if (dialog.exec() == QDialog::Accepted && listw->currentItem()) {
        QString cmd = listw->currentItem()->data(Qt::UserRole).toString();
        cmd.replace("%f", path).replace("%F", path);
        // Eliminar parámetros %x restantes
        QStringList args = cmd.split(' ', Qt::SkipEmptyParts);
        args.erase(std::remove_if(args.begin(), args.end(),
            [](const QString &a){ return a.startsWith('%'); }), args.end());
        if (!args.isEmpty())
            QProcess::startDetached(args.takeFirst(), args);
    }
}

void FileTab::moveToDialog()
{
    QStringList paths = selectedPaths();
    if (paths.isEmpty()) {
        QMessageBox::information(this, "Mover a...",
            "Selecciona al menos un archivo o carpeta.");
        return;
    }

    QString dest = QFileDialog::getExistingDirectory(this,
        "Seleccionar carpeta destino", m_currentPath);
    if (dest.isEmpty()) return;

    int moved = 0;
    for (const QString &p : paths) {
        QString target = dest + "/" + QFileInfo(p).fileName();
        if (QFile::rename(p, target)) moved++;
        else QMessageBox::warning(this, "Error al mover",
            QString("No se pudo mover:\n%1").arg(p));
    }
    refresh();
    emit statusMessage(QString("Movidos %1 elemento(s) a %2").arg(moved).arg(dest));
}

void FileTab::showPropertiesDialog(const QString &path)
{
    QFileInfo fi(path);
    QString size    = fi.isFile() ? humanSize(fi.size()) : "-";
    QString tipo    = fi.isDir() ? "Carpeta" : "Archivo";
    QString modif   = fi.lastModified().toString("yyyy-MM-dd hh:mm:ss");
    QString created = fi.birthTime().toString("yyyy-MM-dd hh:mm:ss");

    QFile::Permissions perms = fi.permissions();
    QString permsStr;
    if (perms & QFileDevice::ReadOwner)  permsStr += "r";
    if (perms & QFileDevice::WriteOwner) permsStr += "w";
    if (perms & QFileDevice::ExeOwner)   permsStr += "x";

    QString msg = QString(
        "<b>Nombre:</b> %1<br>"
        "<b>Ruta:</b> %2<br>"
        "<b>Tipo:</b> %3<br>"
        "<b>Tamaño:</b> %4<br>"
        "<b>Modificado:</b> %5<br>"
        "<b>Creado:</b> %6<br>"
        "<b>Permisos:</b> %7")
        .arg(fi.fileName(), fi.absoluteFilePath(), tipo, size, modif, created, permsStr);

    QMessageBox::information(this, "Propiedades", msg);
}
