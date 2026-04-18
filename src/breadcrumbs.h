#pragma once
#include <QWidget>
#include <QHBoxLayout>
#include <QString>

// Equivalente a la clase Breadcrumbs de Python.
// Muestra botones de ruta navegables en la barra superior.
class Breadcrumbs : public QWidget
{
    Q_OBJECT
public:
    explicit Breadcrumbs(const QString &path, QWidget *parent = nullptr);
    void setPath(const QString &path);

signals:
    void pathChanged(const QString &path);

private:
    QHBoxLayout *m_layout;
    void clearLayout();
};
