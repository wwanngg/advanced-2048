#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

private slots:
    void saveSettings();
    void loadSettings();
    void onButtonBoxClicked(QAbstractButton *button);
    
signals:
    void settingsChanged();

private:
    void createUI();

    QComboBox *m_colorThemeCombo;
    QComboBox *m_gameThemeCombo;
    
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
    QPushButton *m_applyButton;
    
    QSettings *m_settings;
};

#endif