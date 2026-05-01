#include "../include/settingsdialog.h"
#include <QMessageBox>

SettingsDialog::SettingsDialog(QWidget* parent)
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
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QGroupBox* generalGroup = new QGroupBox(tr("Game Settings"), this);
    QFormLayout* formLayout = new QFormLayout(generalGroup);

    m_gameThemeCombo = new QComboBox(this);
    m_gameThemeCombo->addItem(tr("Number Theme"), "number");
    m_gameThemeCombo->addItem(tr("Cai Xukun Theme"), "caixukun");
    m_gameThemeCombo->addItem(tr("Chemistry Theme"), "chemistry");
    formLayout->addRow(tr("Game Theme:"), m_gameThemeCombo);

    m_colorThemeCombo = new QComboBox(this);
    m_colorThemeCombo->addItem(tr("White Theme"), "light");
    m_colorThemeCombo->addItem(tr("Dark Theme"), "dark");
    m_colorThemeCombo->addItem(tr("System Default"), "system");
    formLayout->addRow(tr("Color Theme:"), m_colorThemeCombo);

    mainLayout->addWidget(generalGroup);

    QGroupBox* personalizeGroup = new QGroupBox(tr("Personalize"), this);
    QFormLayout* formLayoutForPersonalize = new QFormLayout(personalizeGroup);

    m_mapX = new QComboBox(this);
    m_mapX->addItem(tr("2"), "2");
    m_mapX->addItem(tr("3"), "3");
    m_mapX->addItem(tr("4"), "4");
    m_mapX->addItem(tr("5"), "5");
    m_mapX->addItem(tr("6"), "6");
    formLayoutForPersonalize->addRow(tr("X"), m_mapX);

    m_mapY = new QComboBox(this);
    m_mapY->addItem(tr("2"), "2");
    m_mapY->addItem(tr("3"), "3");
    m_mapY->addItem(tr("4"), "4");
    m_mapY->addItem(tr("5"), "5");
    m_mapY->addItem(tr("6"), "6");
    formLayoutForPersonalize->addRow(tr("Y"), m_mapY);

    m_animationTimeLasts = new QComboBox(this);
    m_animationTimeLasts->addItem(tr("10ms"), "10ms");
    m_animationTimeLasts->addItem(tr("100ms"), "100ms");
    m_animationTimeLasts->addItem(tr("800ms"), "800ms");
    m_animationTimeLasts->addItem(tr("1000ms"), "1000ms");
    m_animationTimeLasts->addItem(tr("2000ms"), "2000ms");
    m_animationTimeLasts->addItem(tr("5000ms"), "5000ms");
    m_animationTimeLasts->addItem(tr("10000ms"), "10000ms");
    formLayoutForPersonalize->addRow(tr("Animation Duration"), m_animationTimeLasts);

    m_animationEasingCurve = new QComboBox(this);
    m_animationEasingCurve->addItem(tr("Linear"), "Linear");
    m_animationEasingCurve->addItem(tr("InQuad"), "InQuad");
    m_animationEasingCurve->addItem(tr("OutQuad"), "OutQuad");
    m_animationEasingCurve->addItem(tr("InOutQuad"), "InOutQuad");
    m_animationEasingCurve->addItem(tr("OutInQuad"), "OutInQuad");
    m_animationEasingCurve->addItem(tr("InCubic"), "InCubic");
    m_animationEasingCurve->addItem(tr("OutCubic"), "OutCubic");
    m_animationEasingCurve->addItem(tr("InOutCubic"), "InOutCubic");
    m_animationEasingCurve->addItem(tr("OutInCubic"), "OutInCubic");
    m_animationEasingCurve->addItem(tr("InQuart"), "InQuart");
    m_animationEasingCurve->addItem(tr("OutQuart"), "OutQuart");
    m_animationEasingCurve->addItem(tr("InOutQuart"), "InOutQuart");
    m_animationEasingCurve->addItem(tr("OutInQuart"), "OutInQuart");
    m_animationEasingCurve->addItem(tr("InQuint"), "InQuint");
    m_animationEasingCurve->addItem(tr("OutQuint"), "OutQuint");
    m_animationEasingCurve->addItem(tr("InOutQuint"), "InOutQuint");
    m_animationEasingCurve->addItem(tr("OutInQuint"), "OutInQuint");
    m_animationEasingCurve->addItem(tr("InSine"), "InSine");
    m_animationEasingCurve->addItem(tr("OutSine"), "OutSine");
    m_animationEasingCurve->addItem(tr("InOutSine"), "InOutSine");
    m_animationEasingCurve->addItem(tr("OutInSine"), "OutInSine");
    m_animationEasingCurve->addItem(tr("InExpo"), "InExpo");
    m_animationEasingCurve->addItem(tr("OutExpo"), "OutExpo");
    m_animationEasingCurve->addItem(tr("InOutExpo"), "InOutExpo");
    m_animationEasingCurve->addItem(tr("OutInExpo"), "OutInExpo");
    m_animationEasingCurve->addItem(tr("InCirc"), "InCirc");
    m_animationEasingCurve->addItem(tr("OutCirc"), "OutCirc");
    m_animationEasingCurve->addItem(tr("InOutCirc"), "InOutCirc");
    m_animationEasingCurve->addItem(tr("OutInCirc"), "OutInCirc");
    m_animationEasingCurve->addItem(tr("InElastic"), "InElastic");
    m_animationEasingCurve->addItem(tr("OutElastic"), "OutElastic");
    m_animationEasingCurve->addItem(tr("InBack"), "InBack");
    m_animationEasingCurve->addItem(tr("OutBack"), "OutBack");
    m_animationEasingCurve->addItem(tr("InBounce"), "InBounce");
    m_animationEasingCurve->addItem(tr("OutBounce"), "OutBounce");
    formLayoutForPersonalize->addRow(tr("Animation Easing Curve"), m_animationEasingCurve);

    mainLayout->addWidget(personalizeGroup);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    m_okButton = buttonBox->button(QDialogButtonBox::Ok);
    m_cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    m_applyButton = buttonBox->addButton(tr("Apply"), QDialogButtonBox::ApplyRole);

    if (m_okButton)
        m_okButton->setText(tr("OK"));
    if (m_cancelButton)
        m_cancelButton->setText(tr("Cancel"));

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
    int gameThemeIndex = m_gameThemeCombo->findData(gameTheme);
    if (gameThemeIndex >= 0) {
        m_gameThemeCombo->setCurrentIndex(gameThemeIndex);
    }

    QString mapX = m_settings->value("Personalize/mapX", "4").toString();
    int mapXIndex = m_mapX->findData(mapX);
    if (mapXIndex >= 0) {
        m_mapX->setCurrentIndex(mapXIndex);
    }

    QString mapY = m_settings->value("Personalize/mapY", "4").toString();
    int mapYIndex = m_mapY->findData(mapY);
    if (mapYIndex >= 0) {
        m_mapY->setCurrentIndex(mapYIndex);
    }

    QString animationDuration = m_settings->value("Personalize/animationDuration", "800ms").toString();
    int animationDurationIndex = m_animationTimeLasts->findData(animationDuration);
    if (animationDurationIndex >= 0) {
        m_animationTimeLasts->setCurrentIndex(animationDurationIndex);
    }

    QString animationEasingCurve = m_settings->value("Personalize/animationEasingCurve", "InOutQuad").toString();
    int animationEasingCurveIndex = m_animationEasingCurve->findData(animationEasingCurve);
    if (animationEasingCurveIndex >= 0) {
        m_animationEasingCurve->setCurrentIndex(animationEasingCurveIndex);
    }
}

void SettingsDialog::saveSettings() {
    m_settings->setValue("Game/ColorTheme", m_colorThemeCombo->currentData());
    m_settings->setValue("Game/GameTheme", m_gameThemeCombo->currentData());
    m_settings->setValue("Personalize/mapX", m_mapX->currentData());
    m_settings->setValue("Personalize/mapY", m_mapY->currentData());
    m_settings->setValue("Personalize/animationDuration", m_animationTimeLasts->currentData());
    m_settings->setValue("Personalize/animationEasingCurve", m_animationEasingCurve->currentData());

    m_settings->sync();

    emit settingsChanged();
}

void SettingsDialog::onButtonBoxClicked(QAbstractButton* button) {
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