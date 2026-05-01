#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QAudioOutput>
#include <QLabel>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMenu>
#include <QMenuBar>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QTextEdit>

#include "KeyboardHandler.h"

class SettingsDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

#ifdef UNIT_TEST
    friend class TestMainWindow;
    std::function<void()> showMessageCallback = nullptr;
#endif

private slots:
    void onExit();
    void onSettings();
    void onSettingsChanged();
    void onMusicStateChanged(bool play);

private:
    void createMenuBar();
    void loadSettings();
    void initMusicPlayer();

    QMenuBar* menuBar;
    QMenu* fileMenu;
    QAction* exitAction;

    SettingsDialog* m_settingsDialog;
    QTextEdit* m_textEdit;

    QAction* m_settingsAction;

    QMediaPlayer* m_musicPlayer;
    QAudioOutput* m_audioOutput;

    void setupUI();
    void setupAnimations();

    QParallelAnimationGroup* createRandom();

    QWidget* m_centralWidget;

    QSequentialAnimationGroup* createAnimationForKeyA();
    QSequentialAnimationGroup* createAnimationForKeyD();
    QSequentialAnimationGroup* createAnimationForKeyS();
    QSequentialAnimationGroup* createAnimationForKeyW();

    QPropertyAnimation* createMoveAnimation(QWidget* widget, const QRect& offset, int duration);
    QSequentialAnimationGroup* createMoveDisappearAnimation(QWidget* widget, const QRect& offset, int duration);
    QPropertyAnimation* createDisappearAnimation(QWidget* widget, const QRect& startRect, int duration);

    QVector<QVector<int>> m_intMap;
    QVector<QVector<QLabel*>> m_labelMap;
    QVector<QVector<int>> m_surviveCount;

    int m_score;

    KeyboardHandler* m_keyboardHandler;

    bool m_animationRunning;

    enum GameType {
        number,
        caixukun,
        chemistry,
    };

    GameType m_gametype;

    void addSurviveCount();
    bool validate(char direction);
    bool isEmpty();
    bool isGameStart;
};

#endif