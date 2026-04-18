#include "fileopworker.h"
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>

FileOpWorker::FileOpWorker(Operation op,
                           const QStringList &items,
                           const QString &dest,
                           QObject *parent)
    : QThread(parent), m_op(op), m_items(items), m_dest(dest)
{}

QStringList FileOpWorker::collectFiles(const QStringList &paths) const
{
    QStringList result;
    for (const QString &path : paths) {
        QFileInfo fi(path);
        if (fi.isDir()) {
            QDirIterator it(path, QDir::Files | QDir::NoDotAndDotDot,
                            QDirIterator::Subdirectories);
            while (it.hasNext())
                result << it.next();
        } else {
            result << path;
        }
    }
    return result;
}

bool FileOpWorker::copyPath(const QString &src, const QString &dst)
{
    QFileInfo fi(src);
    if (fi.isDir()) {
        QDir destDir(dst);
        if (!destDir.exists())
            return QDir().mkpath(dst);
        // Copiar contenido dentro del directorio existente
        QDirIterator it(src, QDir::AllEntries | QDir::NoDotAndDotDot,
                        QDirIterator::Subdirectories);
        while (it.hasNext()) {
            it.next();
            QString rel = QDir(src).relativeFilePath(it.filePath());
            QString target = dst + "/" + rel;
            if (it.fileInfo().isDir()) {
                QDir().mkpath(target);
            } else {
                QDir().mkpath(QFileInfo(target).absolutePath());
                QFile::copy(it.filePath(), target);
            }
        }
        return true;
    } else {
        QDir().mkpath(QFileInfo(dst).absolutePath());
        return QFile::copy(src, dst);
    }
}

bool FileOpWorker::movePath(const QString &src, const QString &dst)
{
    return QFile::rename(src, dst);
}

void FileOpWorker::run()
{
    try {
        QStringList allFiles = collectFiles(m_items);
        int total = qMax(allFiles.size(), 1);
        int done = 0;

        for (const QString &item : m_items) {
            QFileInfo fi(item);
            QString target = m_dest + "/" + fi.fileName();

            bool ok = false;
            if (m_op == Copy) {
                ok = copyPath(item, target);
            } else {
                ok = movePath(item, target);
            }

            if (!ok) {
                emit finishedError(QString("No se pudo procesar: %1").arg(item));
                return;
            }

            // Calcular progreso real contando ficheros movidos/copiados
            if (fi.isDir()) {
                QDirIterator it(item, QDir::Files | QDir::NoDotAndDotDot,
                                QDirIterator::Subdirectories);
                while (it.hasNext()) {
                    it.next();
                    done++;
                    emit progress(done * 100 / total, it.filePath());
                }
            } else {
                done++;
                emit progress(done * 100 / total, item);
            }
        }
        emit finishedOk();
    } catch (const std::exception &e) {
        emit finishedError(QString::fromStdString(e.what()));
    }
}
