#include <QApplication>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QIcon>
#include "mainwindow.h"
#include "configdialog.h"

static const QString APP_NAME    = "Stellar Explorer";
static const QString ORG_NAME    = "StellarExplorer";
static const QString ORG_DOMAIN  = "stellarexplorer.local";

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(APP_NAME);
    app.setOrganizationName(ORG_NAME);
    app.setOrganizationDomain(ORG_DOMAIN);
    app.setWindowIcon(QIcon(QCoreApplication::applicationDirPath() + "/icons/icon.png"));

    // Cargar themes.css
    QString cssPath = QDir(QCoreApplication::applicationDirPath()).filePath("themes.css");
    QFile cssFile(cssPath);
    if (cssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        app.setStyleSheet(QString::fromUtf8(cssFile.readAll()));
        cssFile.close();
    }

    // Aplicar tema guardado en la configuración
    QSettings settings;
    QString theme = settings.value("theme", "Sistema").toString();

    ConfigDialog configHelper;   // solo para usar getDarkPalette / getLightPalette

    auto isDarkSystem = [&]() -> bool {
        return QApplication::palette().window().color().lightness() < 128;
    };

    if (theme == "Oscuro" || (theme == "Sistema" && isDarkSystem())) {
        app.setProperty("theme", "dark");
        app.setPalette(configHelper.getDarkPalette());
    } else {
        app.setProperty("theme", "light");
        app.setPalette(configHelper.getLightPalette());
    }

    // Ruta inicial opcional como argumento de línea de comandos
    QString startPath = (argc > 1)
        ? QString::fromLocal8Bit(argv[1])
        : QDir::homePath();

    MainWindow w;
    w.show();

    return app.exec();
}
