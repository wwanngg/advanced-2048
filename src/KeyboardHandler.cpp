#include "../include/KeyboardHandler.h"


KeyboardHandler::KeyboardHandler(QObject *parent)
    : QObject(parent)
    , m_isAnyAnimationRunning(false) {}

void KeyboardHandler::registerKeyAnimation(int key, std::function<QSequentialAnimationGroup*()> animationCreator) {
    if (animationCreator) {
        KeyAnimationInfo info;
        info.creator = animationCreator;
        info.isWaiting = false;
        info.currentAnimation = nullptr;
        m_keyAnimations.insert(key, info);
    }
}

bool KeyboardHandler::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        int key = keyEvent->key();
        if (m_keyAnimations.contains(key)) {
            KeyAnimationInfo &info = m_keyAnimations[key];
            if (m_isAnyAnimationRunning) {
                info.isWaiting = true;
            } else {
                startAnimation(info);
            }
            return true;
        }
    }
    
    return QObject::eventFilter(obj, event);
}

void KeyboardHandler::onAnimationFinished() {
    QSequentialAnimationGroup *finishedGroup = qobject_cast<QSequentialAnimationGroup*>(sender());
    
    for (auto it = m_keyAnimations.begin(); it != m_keyAnimations.end(); ++it) {
        if (it.value().currentAnimation == finishedGroup) {
            cleanupAnimation(it.value());
            break;
        }
    }

    for (auto it = m_keyAnimations.begin(); it != m_keyAnimations.end(); ++it) {
        if (it.value().isWaiting) {
            int key = it.key();
            QMetaObject::invokeMethod(this, "startQueuedAnimation", Qt::QueuedConnection, Q_ARG(int, key));
            return;
        }
    }

    m_isAnyAnimationRunning = false;
}

void KeyboardHandler::startQueuedAnimation(int key) {
    if (!m_keyAnimations.contains(key)) return;
    KeyAnimationInfo &info = m_keyAnimations[key];
    if (info.isWaiting) {
        startAnimation(info);
    }
}

void KeyboardHandler::startAnimation(KeyAnimationInfo &info) {
    if (info.creator) {
        info.currentAnimation = info.creator();

        if (info.currentAnimation) {
            connect(info.currentAnimation, &QSequentialAnimationGroup::finished, 
                    this, &KeyboardHandler::onAnimationFinished);

            info.currentAnimation->start();
            info.isWaiting = false;
            m_isAnyAnimationRunning = true;
        }
    }
}

void KeyboardHandler::cleanupAnimation(KeyAnimationInfo &info) {
    if (info.currentAnimation) {
        info.currentAnimation->disconnect();
        info.currentAnimation->deleteLater();
        info.currentAnimation = nullptr;
    }
}

void KeyboardHandler::clearAllAnimations() {
    for (auto it = m_keyAnimations.begin(); it != m_keyAnimations.end(); ++it) {
        KeyAnimationInfo &info = it.value();
        info.isWaiting = false;
        info.creator = nullptr;
        if (info.currentAnimation) {
            info.currentAnimation->disconnect();
            info.currentAnimation->stop();
            info.currentAnimation->deleteLater();
            info.currentAnimation = nullptr;
        }
    }
    m_keyAnimations.clear();
    m_isAnyAnimationRunning = false;
}