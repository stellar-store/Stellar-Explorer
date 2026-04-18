#pragma once
#include <QDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

// Equivalente a ConfigDialog en Python.
// Tres pestañas: General, Temas, Acerca de
class ConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ConfigDialog(QWidget *parent = nullptr);

    QPalette getDarkPalette()  const;
    QPalette getLightPalette() const;

private slots:
    void chooseColor();
    void accept() override;

private:
    // --- General ---
    QCheckBox   *m_showHidden;
    QCheckBox   *m_showThumbnails;
    QComboBox   *m_viewMode;
    QCheckBox   *m_autoExec;
    QLineEdit   *m_execExtensions;

    // --- Temas ---
    QComboBox   *m_themeSelector;
    QPushButton *m_accentColor;

    void applyTheme(const QString &theme);
};
