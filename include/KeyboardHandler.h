#ifndef KEYBOARDHANDLER_H
#define KEYBOARDHANDLER_H

#include <QObject>
#include <QMap>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <functional>
#include <QSequentialanimationgroup>

class KeyboardHandler : public QObject {
    Q_OBJECT
public:
    explicit KeyboardHandler(QObject *parent = nullptr);
    void clearAllAnimations();
    
    void registerKeyAnimation(int key, std::function<QSequentialAnimationGroup*()> animationCreator);
    
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onAnimationFinished();
    void startQueuedAnimation(int key);
    
private:
    struct KeyAnimationInfo {
        std::function<QSequentialAnimationGroup*()> creator;
        bool isWaiting;
        QSequentialAnimationGroup *currentAnimation;
    };
    
    void startAnimation(KeyAnimationInfo &info);
    void cleanupAnimation(KeyAnimationInfo &info);
    
    QMap<int, KeyAnimationInfo> m_keyAnimations;
    bool m_isAnyAnimationRunning;
};

#endif