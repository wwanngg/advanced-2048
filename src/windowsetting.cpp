#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QSettings>
#include <QTextStream>
#include <algorithm>

#include "../include/constants.h"
#include "../include/mainwindow.h"
#include "../include/settingsdialog.h"

void MainWindow::createMenuBar() {
    menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    fileMenu = new QMenu(tr("File(&F)"), this);
    menuBar->addMenu(fileMenu);

    m_settingsAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::HelpAbout), tr("Settings(&S)"), this);
    m_settingsAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Comma));
    connect(m_settingsAction, &QAction::triggered, this, &MainWindow::onSettings);
    fileMenu->addAction(m_settingsAction);

    fileMenu->addSeparator();

    exitAction = new QAction(tr("Exit(&X)"), this);
    exitAction->setShortcut(QKeySequence::Quit);
    exitAction->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::ApplicationExit));
    connect(exitAction, &QAction::triggered, this, &MainWindow::onExit);
    fileMenu->addAction(exitAction);
}

void MainWindow::onSettings() {
    if (!m_settingsDialog) {
        m_settingsDialog = new SettingsDialog(this);
        connect(m_settingsDialog, &SettingsDialog::settingsChanged, this, &MainWindow::onSettingsChanged);
        connect(m_settingsDialog, &SettingsDialog::musicStateChanged, this, &MainWindow::onMusicStateChanged);
    }

    m_settingsDialog->exec();
}

void MainWindow::onSettingsChanged() {
    QSettings settings("wwanngg", "2048advanced");

    QString theme = settings.value("Game/ColorTheme", "system").toString();
    if (theme == "dark") {
        setStyleSheet("QMainWindow { background-color: #2b2b2b; }"
                      "QTextEdit { background-color: #3c3c3c; color: #ffffff; }"
                      "QMenuBar { background-color: #3c3c3c; color: #ffffff; }");
    } else if (theme == "light") {
        setStyleSheet("QMainWindow { background-color: #f0f0f0; }"
                      "QTextEdit { background-color: #ffffff; color: #000000; }"
                      "QMenuBar { background-color: #f0f0f0; color: #000000; }");
    } else {
        setStyleSheet("");
    }

    QString gametheme = settings.value("Game/GameTheme", "number").toString();
    if (gametheme == "number" && m_gametype != GameType::number) {
        for (int i = 0; i < Constants::imax; ++i) {
            for (int j = 0; j < Constants::jmax; ++j) {
                if (!m_labelMap[i][j]) {
                    continue;
                }
                int labelNumber = m_intMap[i][j];
                m_labelMap[i][j]->setAlignment(Qt::AlignCenter);
                m_labelMap[i][j]->setStyleSheet(Constants::styles[labelNumber]);
                m_labelMap[i][j]->setText(QString::number(labelNumber));
            }
        }
        m_gametype = GameType::number;
    } else if (gametheme == "caixukun" && m_gametype != GameType::caixukun) {
        QVector<QString> temp(Constants::caixukunStyles.size());
        int index = 0;
        for (const auto& str : Constants::caixukunStyles) {
            temp[index++] = str;
        }
        std::shuffle(temp.begin(), temp.end(), *QRandomGenerator::global());
        index = 0;
        for (auto& str : Constants::caixukunStyles) {
            str = temp[index++];
        }
        for (int i = 0; i < Constants::imax; ++i) {
            for (int j = 0; j < Constants::jmax; ++j) {
                if (!m_labelMap[i][j]) {
                    continue;
                }
                int labelNumber = m_intMap[i][j];
                m_labelMap[i][j]->setAlignment(Qt::AlignBottom | Qt::AlignRight);
                m_labelMap[i][j]->setStyleSheet(Constants::caixukunStyles[labelNumber]);
                m_labelMap[i][j]->setText(QString::number(labelNumber));
            }
        }
        m_gametype = GameType::caixukun;
    } else if (gametheme == "chemistry" && m_gametype != GameType::chemistry) {
        for (int i = 0; i < Constants::imax; ++i) {
            for (int j = 0; j < Constants::jmax; ++j) {
                if (!m_labelMap[i][j]) {
                    continue;
                }
                int labelNumber = m_intMap[i][j];
                if (m_intMap[i][j] > Constants::chemisrtySuccess || (Constants::chemistryConfig[labelNumber] != -1 && m_surviveCount[i][j] >= Constants::chemistryConfig[labelNumber])) {
                    m_intMap[i][j] = 0;
                    m_surviveCount[i][j] = -1;
                    m_labelMap[i][j]->hide();
                    m_labelMap[i][j]->deleteLater();
                    m_labelMap[i][j] = nullptr;
                    continue;
                }
                m_labelMap[i][j]->setAlignment(Qt::AlignCenter);
                m_labelMap[i][j]->setStyleSheet(Constants::chemistryStyles[labelNumber]);
                if (Constants::chemistryConfig[labelNumber] == -1) {
                    m_labelMap[i][j]->setText(Constants::chemistryString[labelNumber]);
                } else {
                    m_labelMap[i][j]->setText(Constants::chemistryString[labelNumber] + "(" + QString::number(Constants::chemistryConfig[labelNumber] - m_surviveCount[i][j]) + ")");
                }
            }
        }
        m_gametype = GameType::chemistry;
    }
    if (m_gametype == GameType::number) {
        Constants::gameTheme = 0;
    } else if (m_gametype == GameType::caixukun) {
        Constants::gameTheme = 1;
    } else if (m_gametype == GameType::chemistry) {
        Constants::gameTheme = 2;
    }

    bool isPlayEffects = settings.value("Game/isPlayEffect", true).toBool();
    bool isPlayBGM = settings.value("Game/isPlayBGM", true).toBool();
    Constants::isPlayEffects = isPlayEffects;
    Constants::isPlayBGM = isPlayBGM;

    if (isPlayBGM && Constants::gameTheme == 1) {
        if (!m_musicPlayer->isPlaying()) {
            m_musicPlayer->play();
        }
    } else {
        if (m_musicPlayer->isPlaying()) {
            m_musicPlayer->stop();
        }
    }

    QString mapX = settings.value("Personalize/mapX", "4").toString();
    QString mapY = settings.value("Personalize/mapY", "4").toString();

    int mapXnum = mapX.toInt();
    int mapYnum = mapY.toInt();

    if (isGameStart && (mapXnum * Constants::labelSize < Constants::windowSizeX || mapYnum * Constants::labelSize < Constants::windowSizeY)) {
#ifndef UNIT_TEST
        QMessageBox::warning(this, "Warning", "This operation may cause some tiles to disappear");
#else
        if (showMessageCallback) {
            showMessageCallback();
        }
#endif
    }

    int oldRows = static_cast<int>(m_intMap.size());
    int oldCols = oldRows > 0 ? static_cast<int>(m_intMap[0].size()) : 0;
    int newRows = mapYnum;
    int newCols = mapXnum;

    for (int i = 0; i < oldRows; ++i) {
        for (int j = 0; j < oldCols; ++j) {
            if (i >= newRows || j >= newCols) {
                if (m_labelMap[i][j]) {
                    m_labelMap[i][j]->deleteLater();
                    m_labelMap[i][j] = nullptr;
                }
            }
        }
    }

    QVector<QVector<int>> newIntMap(newRows, QVector<int>(newCols, 0));
    QVector<QVector<int>> newSurviveCount(newRows, QVector<int>(newCols, -1));
    QVector<QVector<QLabel*>> newLabelMap(newRows, QVector<QLabel*>(newCols, nullptr));

    int copyRows = std::min(oldRows, newRows);
    int copyCols = std::min(oldCols, newCols);
    for (int i = 0; i < copyRows; ++i) {
        for (int j = 0; j < copyCols; ++j) {
            newIntMap[i][j] = m_intMap[i][j];
            newSurviveCount[i][j] = m_surviveCount[i][j];
            newLabelMap[i][j] = m_labelMap[i][j];
        }
    }

    m_intMap = std::move(newIntMap);
    m_surviveCount = std::move(newSurviveCount);
    m_labelMap = std::move(newLabelMap);

    Constants::windowSizeX = mapXnum * Constants::labelSize;
    Constants::windowSizeY = mapYnum * Constants::labelSize;
    Constants::imax = mapYnum;
    Constants::jmax = mapXnum;
    setFixedSize(Constants::windowSizeX, Constants::windowSizeY + menuBar->height());

    QString animDuration = settings.value("Personalize/animationDuration", "800ms").toString();
    int duration = animDuration.chopped(2).toInt();
    Constants::duration = duration;

    QString animEasingCurve = settings.value("Personalize/animationEasingCurve", "InOutQuad").toString();
    if (animEasingCurve == "Linear")
        Constants::animEasingCurve = QEasingCurve::Linear;
    else if (animEasingCurve == "InQuad")
        Constants::animEasingCurve = QEasingCurve::InQuad;
    else if (animEasingCurve == "OutQuad")
        Constants::animEasingCurve = QEasingCurve::OutQuad;
    else if (animEasingCurve == "InOutQuad")
        Constants::animEasingCurve = QEasingCurve::InOutQuad;
    else if (animEasingCurve == "OutInQuad")
        Constants::animEasingCurve = QEasingCurve::OutInQuad;
    else if (animEasingCurve == "InCubic")
        Constants::animEasingCurve = QEasingCurve::InCubic;
    else if (animEasingCurve == "OutCubic")
        Constants::animEasingCurve = QEasingCurve::OutCubic;
    else if (animEasingCurve == "InOutCubic")
        Constants::animEasingCurve = QEasingCurve::InOutCubic;
    else if (animEasingCurve == "OutInCubic")
        Constants::animEasingCurve = QEasingCurve::OutInCubic;
    else if (animEasingCurve == "InQuart")
        Constants::animEasingCurve = QEasingCurve::InQuart;
    else if (animEasingCurve == "OutQuart")
        Constants::animEasingCurve = QEasingCurve::OutQuart;
    else if (animEasingCurve == "InOutQuart")
        Constants::animEasingCurve = QEasingCurve::InOutQuart;
    else if (animEasingCurve == "OutInQuart")
        Constants::animEasingCurve = QEasingCurve::OutInQuart;
    else if (animEasingCurve == "InQuint")
        Constants::animEasingCurve = QEasingCurve::InQuint;
    else if (animEasingCurve == "OutQuint")
        Constants::animEasingCurve = QEasingCurve::OutQuint;
    else if (animEasingCurve == "InOutQuint")
        Constants::animEasingCurve = QEasingCurve::InOutQuint;
    else if (animEasingCurve == "OutInQuint")
        Constants::animEasingCurve = QEasingCurve::OutInQuint;
    else if (animEasingCurve == "InSine")
        Constants::animEasingCurve = QEasingCurve::InSine;
    else if (animEasingCurve == "OutSine")
        Constants::animEasingCurve = QEasingCurve::OutSine;
    else if (animEasingCurve == "InOutSine")
        Constants::animEasingCurve = QEasingCurve::InOutSine;
    else if (animEasingCurve == "OutInSine")
        Constants::animEasingCurve = QEasingCurve::OutInSine;
    else if (animEasingCurve == "InExpo")
        Constants::animEasingCurve = QEasingCurve::InExpo;
    else if (animEasingCurve == "OutExpo")
        Constants::animEasingCurve = QEasingCurve::OutExpo;
    else if (animEasingCurve == "InOutExpo")
        Constants::animEasingCurve = QEasingCurve::InOutExpo;
    else if (animEasingCurve == "OutInExpo")
        Constants::animEasingCurve = QEasingCurve::OutInExpo;
    else if (animEasingCurve == "InCirc")
        Constants::animEasingCurve = QEasingCurve::InCirc;
    else if (animEasingCurve == "OutCirc")
        Constants::animEasingCurve = QEasingCurve::OutCirc;
    else if (animEasingCurve == "InOutCirc")
        Constants::animEasingCurve = QEasingCurve::InOutCirc;
    else if (animEasingCurve == "OutInCirc")
        Constants::animEasingCurve = QEasingCurve::OutInCirc;
    else if (animEasingCurve == "InElastic")
        Constants::animEasingCurve = QEasingCurve::InElastic;
    else if (animEasingCurve == "OutElastic")
        Constants::animEasingCurve = QEasingCurve::OutElastic;
    else if (animEasingCurve == "InBack")
        Constants::animEasingCurve = QEasingCurve::InBack;
    else if (animEasingCurve == "OutBack")
        Constants::animEasingCurve = QEasingCurve::OutBack;
    else if (animEasingCurve == "InBounce")
        Constants::animEasingCurve = QEasingCurve::InBounce;
    else if (animEasingCurve == "OutBounce")
        Constants::animEasingCurve = QEasingCurve::OutBounce;
}

void MainWindow::loadSettings() {
    QSettings settings("wwanngg", "2048advanced");
    onSettingsChanged();
    isGameStart = true;
}

void MainWindow::onExit() {
    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              tr("Confirm Exit"),
                                                              tr("Are you sure you want to exit the application?"),
                                                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        close();
    }
}

void MainWindow::initMusicPlayer() {
    m_musicPlayer = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_musicPlayer->setAudioOutput(m_audioOutput);

    m_musicPlayer->setSource(QUrl("qrc:///resources/music.mp3"));
    m_audioOutput->setVolume(0.3f);

    connect(m_musicPlayer, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            m_musicPlayer->setPosition(0);
            m_musicPlayer->play();
        }
    });
}

void MainWindow::onMusicStateChanged(bool play) {
    if (play) {
        if (!m_musicPlayer->isPlaying()) {
            m_musicPlayer->play();
        }
    } else {
        if (m_musicPlayer->isPlaying()) {
            m_musicPlayer->stop();
        }
    }
}