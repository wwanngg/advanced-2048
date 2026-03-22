#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QSettings>
#include <algorithm>
#include <QRandomGenerator>

#include "../include/settingsdialog.h"
#include "../include/mainwindow.h"
#include "../include/constants.h"

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
        connect(m_settingsDialog, &SettingsDialog::settingsChanged, 
                this, &MainWindow::onSettingsChanged);
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
        for (std::size_t i{ 0 }; i < 4; ++i) {
            for (std::size_t j{ 0 }; j < 4; ++j) {
                if (!m_labelMap[i][j]) {
                    continue;
                }
                int labelNumber = m_labelMap[i][j]->text().toInt();
                m_labelMap[i][j]->setAlignment(Qt::AlignCenter);
                m_labelMap[i][j]->setStyleSheet(Constants::styles[labelNumber]);
            }
        }
        m_gametype = GameType::number;
    } else if (gametheme == "caixukun" && m_gametype != GameType::caixukun) {
        QVector<QString> temp( Constants::caixukunStyles.size() );
        std::size_t index{ 0 };
        for (const auto& str : Constants::caixukunStyles) {
            temp[index++] = str;
        }
        std::shuffle(temp.begin(), temp.end(), *QRandomGenerator::global());
        index = 0;
        for (auto& str : Constants::caixukunStyles) {
            str = temp[index++];
        }
        for (std::size_t i{ 0 }; i < 4; ++i) {
            for (std::size_t j{ 0 }; j < 4; ++j) {
                if (!m_labelMap[i][j]) {
                    continue;
                }
                int labelNumber = m_labelMap[i][j]->text().toInt();
                m_labelMap[i][j]->setAlignment(Qt::AlignBottom | Qt::AlignRight);
                m_labelMap[i][j]->setStyleSheet(Constants::caixukunStyles[labelNumber]);
            }
        }
        m_gametype = GameType::caixukun;
    }
}

void MainWindow::loadSettings() {
    QSettings settings("wwanngg", "2048advanced");
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