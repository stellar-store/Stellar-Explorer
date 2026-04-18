#include "configdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QGroupBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QSettings>
#include <QColorDialog>
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QCoreApplication>

static const QString APP_NAME = "Stellar Explorer";

ConfigDialog::ConfigDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Configuración");
    setMinimumWidth(420);

    QVBoxLayout *layout = new QVBoxLayout(this);
    QTabWidget  *tabs   = new QTabWidget(this);
    layout->addWidget(tabs);

    QSettings settings;

    // ----------------------------------------------------------------
    // Pestaña General
    // ----------------------------------------------------------------
    QWidget     *generalTab    = new QWidget(this);
    QVBoxLayout *generalLayout = new QVBoxLayout(generalTab);

    // Grupo Visualización
    QGroupBox   *groupView   = new QGroupBox("Visualización", generalTab);
    QVBoxLayout *groupLayout = new QVBoxLayout(groupView);

    m_showHidden = new QCheckBox("Mostrar archivos ocultos", groupView);
    m_showHidden->setChecked(settings.value("show_hidden", false).toBool());
    groupLayout->addWidget(m_showHidden);

    m_showThumbnails = new QCheckBox("Mostrar miniaturas de imágenes", groupView);
    m_showThumbnails->setChecked(settings.value("show_thumbnails", true).toBool());
    groupLayout->addWidget(m_showThumbnails);

    QHBoxLayout *viewLayout = new QHBoxLayout();
    viewLayout->addWidget(new QLabel("Tipo de vista:", groupView));
    m_viewMode = new QComboBox(groupView);
    m_viewMode->addItems({"Lista", "Iconos", "Iconos grandes"});
    m_viewMode->setCurrentText(settings.value("view_mode", "Lista").toString());
    viewLayout->addWidget(m_viewMode);
    groupLayout->addLayout(viewLayout);

    groupView->setLayout(groupLayout);
    generalLayout->addWidget(groupView);

    // Grupo Permisos
    QGroupBox   *groupPerms  = new QGroupBox("Permisos", generalTab);
    QVBoxLayout *permsLayout = new QVBoxLayout(groupPerms);

    m_autoExec = new QCheckBox(
        "Dar permisos de ejecución automáticamente a scripts", groupPerms);
    m_autoExec->setToolTip("Aplicar chmod +x automáticamente a archivos .sh, .py, etc.");
    m_autoExec->setChecked(settings.value("auto_exec", false).toBool());
    permsLayout->addWidget(m_autoExec);

    QHBoxLayout *execLayout = new QHBoxLayout();
    execLayout->addWidget(new QLabel("Extensiones ejecutables:", groupPerms));
    m_execExtensions = new QLineEdit(groupPerms);
    m_execExtensions->setPlaceholderText("sh py bash pl rb");
    m_execExtensions->setText(settings.value("exec_extensions", "sh py bash pl rb").toString());
    execLayout->addWidget(m_execExtensions);
    permsLayout->addLayout(execLayout);

    groupPerms->setLayout(permsLayout);
    generalLayout->addWidget(groupPerms);
    generalLayout->addStretch();

    tabs->addTab(generalTab, "General");

    // ----------------------------------------------------------------
    // Pestaña Temas
    // ----------------------------------------------------------------
    QWidget     *themesTab    = new QWidget(this);
    QVBoxLayout *themesLayout = new QVBoxLayout(themesTab);

    QGroupBox   *groupThemes       = new QGroupBox("Temas de la aplicación", themesTab);
    QVBoxLayout *themesGroupLayout = new QVBoxLayout(groupThemes);

    QHBoxLayout *themeLayout = new QHBoxLayout();
    themeLayout->addWidget(new QLabel("Tema:", groupThemes));
    m_themeSelector = new QComboBox(groupThemes);
    m_themeSelector->addItems({"Claro", "Oscuro", "Sistema"});
    m_themeSelector->setCurrentText(settings.value("theme", "Sistema").toString());
    themeLayout->addWidget(m_themeSelector);
    themesGroupLayout->addLayout(themeLayout);

    QHBoxLayout *colorLayout = new QHBoxLayout();
    colorLayout->addWidget(new QLabel("Color de acento:", groupThemes));
    m_accentColor = new QPushButton(groupThemes);
    m_accentColor->setFixedWidth(100);
    QString currentColor = settings.value("accent_color", "#0078D7").toString();
    m_accentColor->setStyleSheet(QString("background-color: %1;").arg(currentColor));
    connect(m_accentColor, &QPushButton::clicked, this, &ConfigDialog::chooseColor);
    colorLayout->addWidget(m_accentColor);
    themesGroupLayout->addLayout(colorLayout);

    groupThemes->setLayout(themesGroupLayout);
    themesLayout->addWidget(groupThemes);
    themesLayout->addStretch();

    tabs->addTab(themesTab, "Temas");

    // ----------------------------------------------------------------
    // Pestaña Acerca de
    // ----------------------------------------------------------------
    QWidget     *aboutTab    = new QWidget(this);
    QVBoxLayout *aboutLayout = new QVBoxLayout(aboutTab);

    QLabel *logoLabel = new QLabel(aboutTab);
    logoLabel->setPixmap(QIcon("icons/about.png").pixmap(64, 64));
    logoLabel->setAlignment(Qt::AlignCenter);
    aboutLayout->addWidget(logoLabel);

    QLabel *infoText = new QLabel(
        QString("<h2>%1</h2>"
                "<p>Gestor de archivos para Linux en C++ (Qt5)</p>"
                "<p>Versión 1.0</p>"
                "<p>© 2024</p>"
                "<p><a href='https://github.com/'>GitHub</a></p>")
        .arg(APP_NAME), aboutTab);
    infoText->setOpenExternalLinks(true);
    infoText->setAlignment(Qt::AlignCenter);
    aboutLayout->addWidget(infoText);
    aboutLayout->addStretch();

    tabs->addTab(aboutTab, "Acerca de...");

    // ----------------------------------------------------------------
    // Botones OK / Cancelar
    // ----------------------------------------------------------------
    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &ConfigDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

void ConfigDialog::chooseColor()
{
    QSettings settings;
    QColor initial(settings.value("accent_color", "#0078D7").toString());
    QColor color = QColorDialog::getColor(initial, this, "Elegir color de acento");
    if (color.isValid())
        m_accentColor->setStyleSheet(
            QString("background-color: %1;").arg(color.name()));
}

void ConfigDialog::accept()
{
    QSettings settings;
    settings.setValue("show_hidden",     m_showHidden->isChecked());
    settings.setValue("show_thumbnails", m_showThumbnails->isChecked());
    settings.setValue("view_mode",       m_viewMode->currentText());
    settings.setValue("auto_exec",       m_autoExec->isChecked());
    settings.setValue("exec_extensions", m_execExtensions->text());
    settings.setValue("theme",           m_themeSelector->currentText());

    // Extraer color del botón
    QString colorStr = m_accentColor->palette().color(QPalette::Button).name();
    settings.setValue("accent_color", colorStr);

    applyTheme(m_themeSelector->currentText());
    QDialog::accept();
}

void ConfigDialog::applyTheme(const QString &theme)
{
    QApplication *app = qobject_cast<QApplication*>(QCoreApplication::instance());
    if (!app) return;

    // Cargar themes.css
    QString cssFile = QDir(QCoreApplication::applicationDirPath()).filePath("themes.css");
    QString css;
    QFile f(cssFile);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        css = QString::fromUtf8(f.readAll());
        f.close();
    }

    auto isDarkSystem = [&]() -> bool {
        return QApplication::palette().window().color().lightness() < 128;
    };

    if (theme == "Oscuro" || (theme == "Sistema" && isDarkSystem())) {
        app->setProperty("theme", "dark");
        app->setStyleSheet(css);
        app->setPalette(getDarkPalette());
    } else {
        app->setProperty("theme", "light");
        app->setStyleSheet(css);
        app->setPalette(getLightPalette());
    }
    app->processEvents();
}

QPalette ConfigDialog::getDarkPalette() const
{
    QPalette p;
    p.setColor(QPalette::Window,          QColor(53, 53, 53));
    p.setColor(QPalette::WindowText,      QColor(255, 255, 255));
    p.setColor(QPalette::Base,            QColor(42, 42, 42));
    p.setColor(QPalette::AlternateBase,   QColor(66, 66, 66));
    p.setColor(QPalette::ToolTipBase,     QColor(255, 255, 255));
    p.setColor(QPalette::ToolTipText,     QColor(255, 255, 255));
    p.setColor(QPalette::Text,            QColor(255, 255, 255));
    p.setColor(QPalette::Button,          QColor(53, 53, 53));
    p.setColor(QPalette::ButtonText,      QColor(255, 255, 255));
    p.setColor(QPalette::Link,            QColor(42, 130, 218));
    p.setColor(QPalette::Highlight,       QColor(42, 130, 218));
    p.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
    return p;
}

QPalette ConfigDialog::getLightPalette() const
{
    QPalette p;
    p.setColor(QPalette::Window,          QColor(240, 240, 240));
    p.setColor(QPalette::WindowText,      QColor(0, 0, 0));
    p.setColor(QPalette::Base,            QColor(255, 255, 255));
    p.setColor(QPalette::AlternateBase,   QColor(245, 245, 245));
    p.setColor(QPalette::ToolTipBase,     QColor(0, 0, 0));
    p.setColor(QPalette::ToolTipText,     QColor(0, 0, 0));
    p.setColor(QPalette::Text,            QColor(0, 0, 0));
    p.setColor(QPalette::Button,          QColor(240, 240, 240));
    p.setColor(QPalette::ButtonText,      QColor(0, 0, 0));
    p.setColor(QPalette::Link,            QColor(0, 120, 215));
    p.setColor(QPalette::Highlight,       QColor(0, 120, 215));
    p.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
    return p;
}
