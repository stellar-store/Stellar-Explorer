#pragma once
#include <QWidget>
#include <QFileSystemModel>
#include <QListView>
#include <QTreeWidget>
#include <QLineEdit>
#include <QSlider>
#include <QStringList>
#include <QDir>
#include "breadcrumbs.h"

// Equivalente a la clase FileTab de Python.
// Cada pestaña contiene: árbol lateral, vista de archivos, breadcrumbs y búsqueda.
class FileTab : public QWidget
{
    Q_OBJECT
public:
    explicit FileTab(const QString &startPath, QWidget *parent = nullptr);

    QString currentPath() const { return m_currentPath; }

    // Navegación
    void setDirectory(const QString &path);
    void goUp();
    bool goBack();
    bool goForward();
    void refresh();

    // Acciones sobre archivos
    void newFolder();
    void renameSelected();
    void deleteSelected();
    void copySelected();
    void cutSelected();
    void pasteIntoCurrent();

    // Aplicar configuración guardada
    void applySettings();

signals:
    void requestOpenPath(const QString &path);
    void statusMessage(const QString &msg);

private slots:
    void onTreeItemClicked(QTreeWidgetItem *item, int column);
    void onTreeItemExpanded(QTreeWidgetItem *item);
    void onTreeContextMenu(const QPoint &pos);
    void onDoubleClick(const QModelIndex &index);
    void contextMenu(const QPoint &pos);
    void startSearch();
    void changeIconSize(int value);

private:
    // --- Widgets ---
    Breadcrumbs     *m_breadcrumbs;
    QLineEdit       *m_searchEdit;
    QTreeWidget     *m_tree;
    QFileSystemModel *m_fileModel;
    QListView       *m_view;
    QSlider         *m_iconSlider;

    // --- Estado ---
    QString          m_currentPath;
    QStringList      m_clipboardPaths;
    QString          m_clipboardMode;   // "copy" o "cut"
    QList<QString>   m_history;
    int              m_historyIndex;

    // --- Helpers de árbol ---
    void buildSideTree();
    void refreshDevices(QTreeWidgetItem *item);

    // --- Papelera ---
    void showTrashView();
    void trashContextMenu(const QPoint &pos, class QStandardItemModel *model);
    void restoreFromTrash(const QString &filePath);
    void deletePermanently(const QString &filePath);

    // --- Dispositivos ---
    bool isDeviceMounted(const QString &devicePath) const;
    bool mountDevice(const QString &devicePath);
    void unmountDevice(const QString &devicePath);
    bool mountWithAuth(const QString &devicePath);

    // --- Menú contextual de árbol ---
    void handleTreeDeviceMenu(QTreeWidgetItem *item, const QString &path);

    // --- Acciones de archivo extra ---
    void runInTerminal(const QString &path);
    void openWithDialog(const QString &path);
    void moveToDialog();
    void showPropertiesDialog(const QString &path);
    void applyExecPermissions();

    // --- Selección ---
    QStringList selectedPaths() const;
};
