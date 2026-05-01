#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QVBoxLayout>

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    ~SettingsDialog();

private slots:
    void saveSettings();
    void loadSettings();
    void onButtonBoxClicked(QAbstractButton* button);
    void onComboBoxChanged(int index);
    void onMusicCheckBoxToggled(bool checked);

signals:
    void settingsChanged();
    void musicStateChanged(bool play);

#ifdef UNIT_TEST
    friend class TestSettingsDialog;
#endif

private:
    void createUI();

    QComboBox* m_colorThemeCombo;
    QComboBox* m_gameThemeCombo;
    QComboBox* m_mapX;
    QComboBox* m_mapY;
    QComboBox* m_animationTimeLasts;
    QComboBox* m_animationEasingCurve;

    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
    QPushButton* m_applyButton;

    QCheckBox* m_isPlayEffects;
    QCheckBox* m_isPlayBackgroundMusic;

    QSettings* m_settings;
};

#endif