#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialanimationgroup>

#include "KeyboardHandler.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupUI();
    void setupAnimations();

    QParallelAnimationGroup* createRandom();

    QWidget* m_centralWidget;

    QSequentialAnimationGroup* createAnimationForKeyA();
    QSequentialAnimationGroup* createAnimationForKeyD();
    QSequentialAnimationGroup* createAnimationForKeyS();
    QSequentialAnimationGroup* createAnimationForKeyW();

    
    QPropertyAnimation* createMoveAnimation(QWidget *widget, const QRect& offset, int duration);
    QSequentialAnimationGroup* createMoveDisappearAnimation(QWidget *widget, const QRect& offset, int duration);
    QPropertyAnimation* createDisappearAnimation(QWidget *widget, const QRect& startRect, int duration);

    QVector<QVector<int>> m_intMap;
    QVector<QVector<QLabel*>> m_labelMap;

    int m_score;
    
    KeyboardHandler *m_keyboardHandler;

    bool m_animationRunning;
};

#endif