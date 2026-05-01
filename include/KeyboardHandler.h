#ifndef KEYBOARDHANDLER_H
#define KEYBOARDHANDLER_H

#include <QKeyEvent>
#include <QMap>
#include <QObject>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <functional>

class KeyboardHandler : public QObject {
    Q_OBJECT
public:
    explicit KeyboardHandler(QObject* parent = nullptr);
    void clearAllAnimations();

    void registerKeyAnimation(int key, std::function<QSequentialAnimationGroup*()> animationCreator);

#ifdef UNIT_TEST
    friend class TestMainWindow;
    friend class TestKeyboardHandler;
#endif

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void onAnimationFinished();
    void startQueuedAnimation(int key);

private:
    struct KeyAnimationInfo {
        std::function<QSequentialAnimationGroup*()> creator;
        bool isWaiting;
        QSequentialAnimationGroup* currentAnimation;
    };

    void startAnimation(KeyAnimationInfo& info);
    void cleanupAnimation(KeyAnimationInfo& info);

    QMap<int, KeyAnimationInfo> m_keyAnimations;
    bool m_isAnyAnimationRunning;
};

#endif