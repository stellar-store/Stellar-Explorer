#pragma once
#include <QThread>
#include <QStringList>

// Equivalente a FileOpWorker en Python:
// Realiza operaciones de copia o movimiento en un hilo separado
// emitiendo señales de progreso y resultado.
class FileOpWorker : public QThread
{
    Q_OBJECT
public:
    enum Operation { Copy, Move };

    explicit FileOpWorker(Operation op,
                          const QStringList &items,
                          const QString &dest,
                          QObject *parent = nullptr);

signals:
    void progress(int percent, const QString &currentFile);
    void finishedOk();
    void finishedError(const QString &message);

protected:
    void run() override;

private:
    Operation m_op;
    QStringList m_items;
    QString m_dest;

    // Cuenta recursivamente todos los ficheros dentro de una lista de rutas
    QStringList collectFiles(const QStringList &paths) const;

    bool copyPath(const QString &src, const QString &dst);
    bool movePath(const QString &src, const QString &dst);
};
