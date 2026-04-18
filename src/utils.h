#pragma once
#include <QString>
#include <QDir>

// Convierte bytes a representación legible (B, KB, MB, GB...)
QString humanSize(qint64 bytes);

// Abre un archivo/directorio con la aplicación predeterminada del sistema (xdg-open)
void openWithDefault(const QString &path);

// Ejecuta un archivo ejecutable
void runExecutable(const QString &path);

// Abre una terminal en el directorio dado
void openTerminalHere(const QString &directory);
