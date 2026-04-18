#include "breadcrumbs.h"
#include <QToolButton>
#include <QDir>

Breadcrumbs::Breadcrumbs(const QString &path, QWidget *parent)
    : QWidget(parent)
{
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    setPath(path);
}

void Breadcrumbs::clearLayout()
{
    while (m_layout->count() > 0) {
        QLayoutItem *item = m_layout->takeAt(0);
        if (item->widget())
            item->widget()->deleteLater();
        delete item;
    }
}

void Breadcrumbs::setPath(const QString &path)
{
    clearLayout();

    // Descomponer la ruta en partes
    QString canonical = QDir(path).canonicalPath();
    if (canonical.isEmpty())
        canonical = path;

    QStringList parts = canonical.split('/', Qt::SkipEmptyParts);

    // Siempre mostrar la raíz "/"
    QString current = "/";
    {
        QToolButton *btn = new QToolButton(this);
        btn->setText("/");
        btn->setAutoRaise(true);
        const QString p = current;
        connect(btn, &QToolButton::clicked, this, [this, p]() {
            emit pathChanged(p);
        });
        m_layout->addWidget(btn);
    }

    for (const QString &part : parts) {
        current += (current.endsWith('/') ? "" : "/") + part;
        QToolButton *btn = new QToolButton(this);
        btn->setText(part);
        btn->setAutoRaise(true);
        const QString p = current;
        connect(btn, &QToolButton::clicked, this, [this, p]() {
            emit pathChanged(p);
        });
        m_layout->addWidget(btn);
    }

    m_layout->addStretch(1);
}
