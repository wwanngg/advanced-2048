#include <QApplication>
#include <QPushButton>
#include <QSettings>
#include <QSignalSpy>
#include <QTest>
#include <QtCore>
#include <QtTest>

#include "../include/constants.h"
#include "../include/settingsdialog.h"

class TestSettingsDialog : public QObject {
    Q_OBJECT

private:
    SettingsDialog* m_dialog;

    void clearTestSettings() {
        QSettings settings("wwanngg", "2048advanced");
        settings.remove("Game/ColorTheme");
        settings.remove("Game/GameTheme");
        settings.remove("Game/isPlayEffect");
        settings.remove("Game/isPlayBGM");
        settings.remove("Personalize/mapX");
        settings.remove("Personalize/mapY");
        settings.remove("Personalize/animationDuration");
        settings.remove("Personalize/animationEasingCurve");
    }

private slots:
    void initTestCase() {
        qputenv("QT_QPA_PLATFORM", "offscreen");
    }

    void init() {
        clearTestSettings();
        m_dialog = new SettingsDialog();
    }

    void cleanup() {
        delete m_dialog;
        clearTestSettings();
    }

    void testConstructor_WindowTitle() {
        QVERIFY(!m_dialog->windowTitle().isEmpty());
    }

    void testConstructor_MinimumSize() {
        QVERIFY(m_dialog->minimumWidth() >= 400);
        QVERIFY(m_dialog->minimumHeight() >= 300);
    }

    void testGameThemeCombo_HasThreeItems() {
        QCOMPARE(m_dialog->m_gameThemeCombo->count(), 3);
        QCOMPARE(m_dialog->m_gameThemeCombo->itemData(0).toString(), QString("number"));
        QCOMPARE(m_dialog->m_gameThemeCombo->itemData(1).toString(), QString("caixukun"));
        QCOMPARE(m_dialog->m_gameThemeCombo->itemData(2).toString(), QString("chemistry"));
    }

    void testColorThemeCombo_HasThreeItems() {
        QCOMPARE(m_dialog->m_colorThemeCombo->count(), 3);
        QCOMPARE(m_dialog->m_colorThemeCombo->itemData(0).toString(), QString("light"));
        QCOMPARE(m_dialog->m_colorThemeCombo->itemData(1).toString(), QString("dark"));
        QCOMPARE(m_dialog->m_colorThemeCombo->itemData(2).toString(), QString("system"));
    }

    void testMapX_HasFiveItems() {
        QCOMPARE(m_dialog->m_mapX->count(), 5);
        for (int i = 0; i < 5; ++i) {
            QCOMPARE(m_dialog->m_mapX->itemData(i).toString(),
                     QString::number(i + 2));
        }
    }

    void testMapY_HasFiveItems() {
        QCOMPARE(m_dialog->m_mapY->count(), 5);
        for (int i = 0; i < 5; ++i) {
            QCOMPARE(m_dialog->m_mapY->itemData(i).toString(),
                     QString::number(i + 2));
        }
    }

    void testAnimationDuration_HasSevenItems() {
        QCOMPARE(m_dialog->m_animationTimeLasts->count(), 7);
        QCOMPARE(m_dialog->m_animationTimeLasts->itemData(0).toString(), QString("10ms"));
        QCOMPARE(m_dialog->m_animationTimeLasts->itemData(6).toString(), QString("10000ms"));
    }

    void testEasingCurve_HasMoreThanTwentyItems() {
        QVERIFY(m_dialog->m_animationEasingCurve->count() > 20);
    }

    void testApplyButton_Exists() {
        QVERIFY(m_dialog->m_applyButton);
        QCOMPARE(m_dialog->m_applyButton->text(), QString("Apply"));
    }

    void testDefaultValues_AfterConstruction() {
        QCOMPARE(m_dialog->m_colorThemeCombo->currentData().toString(),
                 QString("system"));
        QCOMPARE(m_dialog->m_gameThemeCombo->currentData().toString(),
                 QString("number"));
        QCOMPARE(m_dialog->m_mapX->currentData().toString(), QString("4"));
        QCOMPARE(m_dialog->m_mapY->currentData().toString(), QString("4"));
        QCOMPARE(m_dialog->m_animationTimeLasts->currentData().toString(),
                 QString("800ms"));
        QCOMPARE(m_dialog->m_animationEasingCurve->currentData().toString(),
                 QString("InOutQuad"));
    }

    void testSaveSettings_WritesAllValues() {
        m_dialog->m_colorThemeCombo->setCurrentIndex(0);
        m_dialog->m_gameThemeCombo->setCurrentIndex(1);
        m_dialog->m_mapX->setCurrentIndex(3);
        m_dialog->m_mapY->setCurrentIndex(1);
        m_dialog->m_animationTimeLasts->setCurrentIndex(3);
        m_dialog->m_animationEasingCurve->setCurrentIndex(4);

        m_dialog->saveSettings();

        QSettings settings("wwanngg", "2048advanced");
        QCOMPARE(settings.value("Game/ColorTheme").toString(), QString("light"));
        QCOMPARE(settings.value("Game/GameTheme").toString(), QString("caixukun"));
        QCOMPARE(settings.value("Personalize/mapX").toString(), QString("5"));
        QCOMPARE(settings.value("Personalize/mapY").toString(), QString("3"));
        QCOMPARE(settings.value("Personalize/animationDuration").toString(),
                 QString("1000ms"));
        QCOMPARE(settings.value("Personalize/animationEasingCurve").toString(),
                 QString("OutInQuad"));
    }

    void testSaveSettings_EmitsSettingsChanged() {
        QSignalSpy spy(m_dialog, &SettingsDialog::settingsChanged);
        m_dialog->saveSettings();
        QCOMPARE(spy.count(), 1);
    }

    void testSaveSettings_SyncsSettings() {
        m_dialog->saveSettings();
        QSettings settings("wwanngg", "2048advanced");
        QVERIFY(settings.contains("Game/ColorTheme"));
    }

    void testLoadSettings_ReadsSavedValues() {
        QSettings settings("wwanngg", "2048advanced");
        settings.setValue("Game/ColorTheme", "dark");
        settings.setValue("Game/GameTheme", "chemistry");
        settings.setValue("Personalize/mapX", "2");
        settings.setValue("Personalize/mapY", "6");
        settings.setValue("Personalize/animationDuration", "5000ms");
        settings.setValue("Personalize/animationEasingCurve", "InBounce");

        m_dialog->loadSettings();

        QCOMPARE(m_dialog->m_colorThemeCombo->currentData().toString(),
                 QString("dark"));
        QCOMPARE(m_dialog->m_gameThemeCombo->currentData().toString(),
                 QString("chemistry"));
        QCOMPARE(m_dialog->m_mapX->currentData().toString(), QString("2"));
        QCOMPARE(m_dialog->m_mapY->currentData().toString(), QString("6"));
        QCOMPARE(m_dialog->m_animationTimeLasts->currentData().toString(),
                 QString("5000ms"));
        QCOMPARE(m_dialog->m_animationEasingCurve->currentData().toString(),
                 QString("InBounce"));
    }

    void testLoadSettings_InvalidValue_KeepsPrevious() {
        m_dialog->m_gameThemeCombo->setCurrentIndex(0);

        QSettings settings("wwanngg", "2048advanced");
        settings.setValue("Game/GameTheme", "nonexistent_theme");

        m_dialog->loadSettings();
        QCOMPARE(m_dialog->m_gameThemeCombo->currentData().toString(),
                 QString("number"));
    }

    void testLoadSettings_PartialSettings_KeepsDefaults() {
        QSettings settings("wwanngg", "2048advanced");
        settings.setValue("Game/ColorTheme", "dark");

        m_dialog->loadSettings();

        QCOMPARE(m_dialog->m_colorThemeCombo->currentData().toString(),
                 QString("dark"));
        QCOMPARE(m_dialog->m_gameThemeCombo->currentData().toString(),
                 QString("number"));
    }

    void testOkButtonClick_SavesAndAccepts() {
        m_dialog->m_colorThemeCombo->setCurrentIndex(1);

        QSignalSpy spy(m_dialog, &SettingsDialog::settingsChanged);

        m_dialog->onButtonBoxClicked(m_dialog->m_okButton);

        QCOMPARE(spy.count(), 1);
        QCOMPARE(m_dialog->result(), QDialog::Accepted);

        QSettings settings("wwanngg", "2048advanced");
        QCOMPARE(settings.value("Game/ColorTheme").toString(), QString("dark"));
    }

    void testOkButtonClick_WithDefaultValues_SavesDefaults() {
        m_dialog->onButtonBoxClicked(m_dialog->m_okButton);

        QCOMPARE(m_dialog->result(), QDialog::Accepted);

        QSettings settings("wwanngg", "2048advanced");
        QCOMPARE(settings.value("Game/ColorTheme").toString(), QString("system"));
        QCOMPARE(settings.value("Game/GameTheme").toString(), QString("number"));
    }

    void testCancelButtonClick_RejectsWithoutSaving() {
        m_dialog->m_colorThemeCombo->setCurrentIndex(0);

        QSignalSpy spy(m_dialog, &SettingsDialog::settingsChanged);

        m_dialog->onButtonBoxClicked(m_dialog->m_cancelButton);

        QCOMPARE(spy.count(), 0);
        QCOMPARE(m_dialog->result(), QDialog::Rejected);

        QSettings settings("wwanngg", "2048advanced");
        QVERIFY(!settings.contains("Game/ColorTheme"));
    }

    void testApplyButtonClick_SavesSettings() {
        m_dialog->m_colorThemeCombo->setCurrentIndex(2);

        QSignalSpy spy(m_dialog, &SettingsDialog::settingsChanged);

        QTimer::singleShot(0, []() {
            QWidget* msgBox = QApplication::activeModalWidget();
            if (msgBox) {
                msgBox->close();
            }
        });

        m_dialog->onButtonBoxClicked(m_dialog->m_applyButton);

        QCOMPARE(spy.count(), 1);

        QSettings settings("wwanngg", "2048advanced");
        QCOMPARE(settings.value("Game/ColorTheme").toString(), QString("system"));
    }

    void testSaveThenLoad_RoundTrip() {
        m_dialog->m_gameThemeCombo->setCurrentIndex(2);
        m_dialog->m_colorThemeCombo->setCurrentIndex(1);
        m_dialog->m_mapX->setCurrentIndex(4);
        m_dialog->m_mapY->setCurrentIndex(0);
        m_dialog->m_animationTimeLasts->setCurrentIndex(6);
        m_dialog->m_animationEasingCurve->setCurrentIndex(2);

        m_dialog->saveSettings();

        delete m_dialog;
        m_dialog = new SettingsDialog();

        QCOMPARE(m_dialog->m_gameThemeCombo->currentData().toString(),
                 QString("chemistry"));
        QCOMPARE(m_dialog->m_colorThemeCombo->currentData().toString(),
                 QString("dark"));
        QCOMPARE(m_dialog->m_mapX->currentData().toString(), QString("6"));
        QCOMPARE(m_dialog->m_mapY->currentData().toString(), QString("2"));
        QCOMPARE(m_dialog->m_animationTimeLasts->currentData().toString(),
                 QString("10000ms"));
        QCOMPARE(m_dialog->m_animationEasingCurve->currentData().toString(),
                 QString("OutQuad"));
    }

    void testMultipleSaveSavesLatest() {
        m_dialog->m_colorThemeCombo->setCurrentIndex(0);
        m_dialog->saveSettings();

        m_dialog->m_colorThemeCombo->setCurrentIndex(1);
        m_dialog->saveSettings();

        QSettings settings("wwanngg", "2048advanced");
        QCOMPARE(settings.value("Game/ColorTheme").toString(), QString("dark"));
    }

    void testIsPlayEffects_DefaultChecked() {
        QVERIFY(m_dialog->m_isPlayEffects);
        QVERIFY(m_dialog->m_isPlayEffects->isEnabled());
        QVERIFY(m_dialog->m_isPlayEffects->isChecked());
    }

    void testIsPlayBackgroundMusic_InitiallyDisabled() {
        QVERIFY(m_dialog->m_isPlayBackgroundMusic);
        QVERIFY(!m_dialog->m_isPlayBackgroundMusic->isEnabled());
    }

    void testComboBoxChanged_ToCaixukun_EnablesBGM() {
        m_dialog->m_gameThemeCombo->setCurrentIndex(1);
        QVERIFY(m_dialog->m_isPlayBackgroundMusic->isEnabled());
    }

    void testComboBoxChanged_AwayFromCaixukun_DisablesBGM() {
        m_dialog->m_gameThemeCombo->setCurrentIndex(1);
        QVERIFY(m_dialog->m_isPlayBackgroundMusic->isEnabled());
        m_dialog->m_gameThemeCombo->setCurrentIndex(0);
        QVERIFY(!m_dialog->m_isPlayBackgroundMusic->isEnabled());
    }

    void testComboBoxChanged_ToCaixukun_EmitsMusicStateChanged() {
        QSignalSpy spy(m_dialog, &SettingsDialog::musicStateChanged);
        m_dialog->m_gameThemeCombo->setCurrentIndex(1);
        QCOMPARE(spy.count(), 1);
        QVERIFY(spy.at(0).at(0).toBool());
    }

    void testComboBoxChanged_ToCaixukun_BGMGloballyDisabled_EmitsFalse() {
        bool original = Constants::isPlayBGM;
        Constants::isPlayBGM = false;

        QSignalSpy spy(m_dialog, &SettingsDialog::musicStateChanged);
        m_dialog->m_gameThemeCombo->setCurrentIndex(1);
        QCOMPARE(spy.count(), 1);
        QVERIFY(!spy.at(0).at(0).toBool());

        Constants::isPlayBGM = original;
    }

    void testComboBoxChanged_AwayFromCaixukun_EmitsMusicStateChanged() {
        m_dialog->m_gameThemeCombo->setCurrentIndex(1);
        QSignalSpy spy(m_dialog, &SettingsDialog::musicStateChanged);
        m_dialog->m_gameThemeCombo->setCurrentIndex(0);
        QCOMPARE(spy.count(), 1);
        QVERIFY(!spy.at(0).at(0).toBool());
    }

    void testMusicCheckBoxToggled_Checked_EmitsTrue() {
        m_dialog->m_gameThemeCombo->setCurrentIndex(1);
        m_dialog->m_isPlayBackgroundMusic->setChecked(false);
        QSignalSpy spy(m_dialog, &SettingsDialog::musicStateChanged);
        m_dialog->m_isPlayBackgroundMusic->setChecked(true);
        QCOMPARE(spy.count(), 1);
        QVERIFY(spy.at(0).at(0).toBool());
    }

    void testMusicCheckBoxToggled_Unchecked_EmitsFalse() {
        m_dialog->m_gameThemeCombo->setCurrentIndex(1);
        QSignalSpy spy(m_dialog, &SettingsDialog::musicStateChanged);
        m_dialog->m_isPlayBackgroundMusic->setChecked(false);
        QCOMPARE(spy.count(), 1);
        QVERIFY(!spy.at(0).at(0).toBool());
    }

    void testSaveSettings_SavesCheckBoxStates() {
        m_dialog->m_isPlayEffects->setChecked(false);
        m_dialog->m_isPlayBackgroundMusic->setChecked(true);
        m_dialog->saveSettings();

        QSettings settings("wwanngg", "2048advanced");
        QCOMPARE(settings.value("Game/isPlayEffect").toBool(), false);
        QCOMPARE(settings.value("Game/isPlayBGM").toBool(), true);
    }

    void testLoadSettings_LoadsCheckBoxStates() {
        QSettings settings("wwanngg", "2048advanced");
        settings.setValue("Game/isPlayEffect", false);
        settings.setValue("Game/isPlayBGM", true);

        m_dialog->loadSettings();

        QCOMPARE(m_dialog->m_isPlayEffects->isChecked(), false);
        QCOMPARE(m_dialog->m_isPlayBackgroundMusic->isChecked(), true);
    }

    void testLoadSettings_BGMAndCaixukun_EmitsMusicStateChanged() {
        QSettings settings("wwanngg", "2048advanced");
        settings.setValue("Game/GameTheme", "caixukun");
        settings.setValue("Game/isPlayBGM", true);

        QSignalSpy spy(m_dialog, &SettingsDialog::musicStateChanged);
        m_dialog->loadSettings();
        QVERIFY(spy.count() >= 1);
        QVERIFY(spy.at(spy.count() - 1).at(0).toBool());
    }

    void testLoadSettings_BGMDisabled_EmitsMusicStateFalse() {
        QSettings settings("wwanngg", "2048advanced");
        settings.setValue("Game/GameTheme", "caixukun");
        settings.setValue("Game/isPlayBGM", false);

        QSignalSpy spy(m_dialog, &SettingsDialog::musicStateChanged);
        m_dialog->loadSettings();
        QVERIFY(spy.count() >= 1);
        QVERIFY(!spy.at(spy.count() - 1).at(0).toBool());
    }

    void testSaveThenLoad_RoundTrip_WithCheckBoxes() {
        m_dialog->m_gameThemeCombo->setCurrentIndex(1);
        m_dialog->m_isPlayEffects->setChecked(false);
        m_dialog->m_isPlayBackgroundMusic->setChecked(true);
        m_dialog->saveSettings();

        delete m_dialog;
        m_dialog = new SettingsDialog();

        QCOMPARE(m_dialog->m_gameThemeCombo->currentData().toString(),
                 QString("caixukun"));
        QCOMPARE(m_dialog->m_isPlayEffects->isChecked(), false);
        QCOMPARE(m_dialog->m_isPlayBackgroundMusic->isChecked(), true);
        QVERIFY(m_dialog->m_isPlayBackgroundMusic->isEnabled());
    }

    void testOkButton_HasCorrectText() {
        QVERIFY(m_dialog->m_okButton);
        QCOMPARE(m_dialog->m_okButton->text(), QString("OK"));
    }

    void testCancelButton_ExistsAndHasCorrectText() {
        QVERIFY(m_dialog->m_cancelButton);
        QCOMPARE(m_dialog->m_cancelButton->text(), QString("Cancel"));
    }
};

QTEST_MAIN(TestSettingsDialog)
#include "test_settingsdialog.moc"
