#pragma once
#include <QMainWindow>
#include <QTabWidget>
#include <QLineEdit>
#include <QAction>
#include "filetab.h"

// Equivalente a MainWindow de Python.
// Gestiona las pestañas, barra de herramientas y atajos de teclado.
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newTab(const QString &path = QString());
    void newTabHere();
    void closeTab(int index);
    void goAddress();
    void onNewFolder();
    void onRename();
    void onDelete();
    void onCopy();
    void onCut();
    void onPaste();
    void onRefresh();
    void onUp();
    void onBack();
    void onForward();
    void showConfig();
    void updateNavigationButtons();

private:
    QTabWidget *m_tabs;
    QLineEdit  *m_address;

    QAction *m_actBack;
    QAction *m_actUp;
    QAction *m_actForward;
    QAction *m_actNewTab;
    QAction *m_actNewFolder;
    QAction *m_actRename;
    QAction *m_actDelete;
    QAction *m_actCopy;
    QAction *m_actCut;
    QAction *m_actPaste;
    QAction *m_actRefresh;
    QAction *m_actConfig;

    FileTab *currentTab() const;
};
