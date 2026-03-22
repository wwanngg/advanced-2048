#include "../include/settingsdialog.h"
#include <QMessageBox>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , m_settings(new QSettings("wwanngg", "2048advanced", this)) {
    setWindowTitle(tr("Settings"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMinimumSize(400, 300);
    
    createUI();
    loadSettings();
}

SettingsDialog::~SettingsDialog() {}

void SettingsDialog::createUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QGroupBox *generalGroup = new QGroupBox(tr("Game Settings"), this);
    QFormLayout *formLayout = new QFormLayout(generalGroup);
    
    m_gameThemeCombo = new QComboBox(this);
    m_gameThemeCombo->addItem(tr("Number Theme"), "number");
    m_gameThemeCombo->addItem(tr("Cai Xukun Theme"), "caixukun");
    formLayout->addRow(tr("Game Theme:"), m_gameThemeCombo);

    m_colorThemeCombo = new QComboBox(this);
    m_colorThemeCombo->addItem(tr("White Theme"), "light");
    m_colorThemeCombo->addItem(tr("Dark Theme"), "dark");
    m_colorThemeCombo->addItem(tr("System Default"), "system");
    formLayout->addRow(tr("Color Theme:"), m_colorThemeCombo);
    
    mainLayout->addWidget(generalGroup);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    m_okButton = buttonBox->button(QDialogButtonBox::Ok);
    m_cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    m_applyButton = buttonBox->addButton(tr("Apply"), QDialogButtonBox::ApplyRole);
    
    if (m_okButton) m_okButton->setText(tr("OK"));
    if (m_cancelButton) m_cancelButton->setText(tr("Cancel"));
    
    connect(buttonBox, &QDialogButtonBox::clicked, this, &SettingsDialog::onButtonBoxClicked);
    
    mainLayout->addWidget(buttonBox);
}

void SettingsDialog::loadSettings() {
    QString colorTheme = m_settings->value("Game/ColorTheme", "system").toString();
    int colorThemeIndex = m_colorThemeCombo->findData(colorTheme);
    if (colorThemeIndex >= 0) {
        m_colorThemeCombo->setCurrentIndex(colorThemeIndex);
    }

    QString gameTheme = m_settings->value("Game/GameTheme", "number").toString();
    int gameThemeIndex = m_colorThemeCombo->findData(gameTheme);
    if (gameThemeIndex >= 0) {
        m_colorThemeCombo->setCurrentIndex(gameThemeIndex);
    }
}

void SettingsDialog::saveSettings() {
    m_settings->setValue("Game/ColorTheme", m_colorThemeCombo->currentData());
    m_settings->setValue("Game/GameTheme", m_gameThemeCombo->currentData());
    
    m_settings->sync();
    
    emit settingsChanged();
}

void SettingsDialog::onButtonBoxClicked(QAbstractButton *button) {
    if (button == m_okButton) {
        saveSettings();
        accept();
    } else if (button == m_cancelButton) {
        reject();
    } else if (button == m_applyButton) {
        saveSettings();
        QMessageBox::information(this, tr("Info"), tr("Settings applied"));
    }
}