#include "utils.h"
#include <QProcess>
#include <QStandardPaths>

QString humanSize(qint64 bytes)
{
    const QStringList units = {"B", "KB", "MB", "GB", "TB"};
    double num = static_cast<double>(bytes);
    for (const QString &unit : units) {
        if (num < 1024.0)
            return QString::number(num, 'f', 1) + " " + unit;
        num /= 1024.0;
    }
    return QString::number(num, 'f', 1) + " PB";
}

void openWithDefault(const QString &path)
{
    QProcess::startDetached("xdg-open", {path});
}

void runExecutable(const QString &path)
{
    QProcess::startDetached(path, {});
}

void openTerminalHere(const QString &directory)
{
    // Buscar el emulador de terminal disponible en el sistema
    QStringList candidates = {"x-terminal-emulator", "gnome-terminal", "konsole", "xterm"};
    for (const QString &term : candidates) {
        QString found = QStandardPaths::findExecutable(term);
        if (!found.isEmpty()) {
            QProcess *p = new QProcess();
            p->setWorkingDirectory(directory);
            p->startDetached(found, {});
            return;
        }
    }
}
