#include <QApplication>
#include <QKeyEvent>
#include <QPauseAnimation>
#include <QSequentialAnimationGroup>
#include <QSignalSpy>
#include <QSoundEffect>
#include <QWidget>
#include <QtCore>
#include <QtTest>

#include "../include/KeyboardHandler.h"
#include "../include/constants.h"

class TestKeyboardHandler : public QObject {
    Q_OBJECT

private:
    KeyboardHandler* m_handler;
    QWidget* m_testWidget;

    QSequentialAnimationGroup* createAnimation(int duration = 10) {
        auto* group = new QSequentialAnimationGroup();
        auto* pause = new QPauseAnimation(duration);
        group->addAnimation(pause);
        return group;
    }

private slots:
    void initTestCase() { qputenv("QT_QPA_PLATFORM", "offscreen"); }

    void init() {
        m_handler = new KeyboardHandler();
        m_testWidget = new QWidget();
        m_testWidget->installEventFilter(m_handler);
    }

    void cleanup() {
        delete m_testWidget;
        delete m_handler;
    }

    void testInitialState() {
        QVERIFY(!m_handler->m_isAnyAnimationRunning);
        QVERIFY(m_handler->m_keyAnimations.isEmpty());
    }

    void testRegisterSingleKey() {
        m_handler->registerKeyAnimation(Qt::Key_Left,
                                        [this]() { return createAnimation(); });

        QCOMPARE(m_handler->m_keyAnimations.size(), 1);
        QVERIFY(m_handler->m_keyAnimations.contains(Qt::Key_Left));
        QVERIFY(!m_handler->m_keyAnimations[Qt::Key_Left].isWaiting);
        QVERIFY(!m_handler->m_keyAnimations[Qt::Key_Left].currentAnimation);
    }

    void testRegisterMultipleKeys() {
        m_handler->registerKeyAnimation(Qt::Key_Left,
                                        [this]() { return createAnimation(); });
        m_handler->registerKeyAnimation(Qt::Key_Right,
                                        [this]() { return createAnimation(); });
        m_handler->registerKeyAnimation(Qt::Key_Up,
                                        [this]() { return createAnimation(); });

        QCOMPARE(m_handler->m_keyAnimations.size(), 3);
    }

    void testRegisterNullCreator() {
        m_handler->registerKeyAnimation(Qt::Key_Left, nullptr);
        QVERIFY(m_handler->m_keyAnimations.isEmpty());
    }

    void testRegisterOverwriteKey() {
        int callCount = 0;
        auto creator = [&callCount, this]() {
            callCount++;
            return createAnimation();
        };

        m_handler->registerKeyAnimation(Qt::Key_Left, creator);
        QCOMPARE(m_handler->m_keyAnimations.size(), 1);

        m_handler->registerKeyAnimation(Qt::Key_Left, creator);
        QCOMPARE(m_handler->m_keyAnimations.size(), 1);
        QKeyEvent event(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
        m_handler->eventFilter(m_testWidget, &event);
        QCOMPARE(callCount, 1);
    }

    void testEventFilterNonKeyEvent() {
        m_handler->registerKeyAnimation(Qt::Key_Left,
                                        [this]() { return createAnimation(); });

        QEvent mouseEvent(QEvent::MouseButtonPress);
        bool result = m_handler->eventFilter(m_testWidget, &mouseEvent);
        QVERIFY(!result);
        QVERIFY(!m_handler->m_isAnyAnimationRunning);
    }

    void testEventFilterOnDifferentObject() {
        m_handler->registerKeyAnimation(Qt::Key_A,
                                        [this]() { return createAnimation(); });

        QWidget otherWidget;
        QKeyEvent event(QEvent::KeyPress, Qt::Key_A, Qt::NoModifier);
        bool result = m_handler->eventFilter(&otherWidget, &event);
        QVERIFY(result);
        QVERIFY(m_handler->m_isAnyAnimationRunning);
    }

    void testUnregisteredKeyPassesThrough() {
        m_handler->registerKeyAnimation(Qt::Key_Left,
                                        [this]() { return createAnimation(); });

        QKeyEvent event(QEvent::KeyPress, Qt::Key_Space, Qt::NoModifier);
        bool result = m_handler->eventFilter(m_testWidget, &event);
        QVERIFY(!result);
        QVERIFY(!m_handler->m_isAnyAnimationRunning);
    }

    void testRegisteredKeyTriggersAnimation() {
        int creatorCallCount = 0;
        m_handler->registerKeyAnimation(Qt::Key_Left, [&creatorCallCount, this]() {
            creatorCallCount++;
            return createAnimation();
        });

        QKeyEvent event(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
        bool result = m_handler->eventFilter(m_testWidget, &event);

        QVERIFY(result);
        QVERIFY(m_handler->m_isAnyAnimationRunning);
        QCOMPARE(creatorCallCount, 1);
        QVERIFY(!m_handler->m_keyAnimations[Qt::Key_Left].isWaiting);
        QVERIFY(m_handler->m_keyAnimations[Qt::Key_Left].currentAnimation);
    }

    void testRegisteredKeyAnimationCompletes() {
        m_handler->registerKeyAnimation(Qt::Key_Left,
                                        [this]() { return createAnimation(5); });

        QKeyEvent event(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
        m_handler->eventFilter(m_testWidget, &event);

        QSequentialAnimationGroup* anim =
            m_handler->m_keyAnimations[Qt::Key_Left].currentAnimation;
        QVERIFY(anim);

        QSignalSpy finishedSpy(anim, &QSequentialAnimationGroup::finished);
        QTRY_VERIFY_WITH_TIMEOUT(finishedSpy.count() > 0, 2000);

        QVERIFY(!m_handler->m_isAnyAnimationRunning);
    }

    void testSameKeyQueued() {
        m_handler->registerKeyAnimation(Qt::Key_Left,
                                        [this]() { return createAnimation(200); });

        QKeyEvent event(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
        m_handler->eventFilter(m_testWidget, &event);
        QVERIFY(m_handler->m_isAnyAnimationRunning);
        QVERIFY(!m_handler->m_keyAnimations[Qt::Key_Left].isWaiting);

        m_handler->eventFilter(m_testWidget, &event);
        QVERIFY(m_handler->m_keyAnimations[Qt::Key_Left].isWaiting);

        QSequentialAnimationGroup* anim =
            m_handler->m_keyAnimations[Qt::Key_Left].currentAnimation;
        QSignalSpy finishedSpy(anim, &QSequentialAnimationGroup::finished);
        QTRY_VERIFY_WITH_TIMEOUT(finishedSpy.count() > 0, 2000);

        QTRY_VERIFY_WITH_TIMEOUT(m_handler->m_isAnyAnimationRunning, 1000);
    }

    void testMultipleKeysQueued() {
        m_handler->registerKeyAnimation(Qt::Key_Left,
                                        [this]() { return createAnimation(200); });
        m_handler->registerKeyAnimation(Qt::Key_Right,
                                        [this]() { return createAnimation(5); });
        m_handler->registerKeyAnimation(Qt::Key_Up,
                                        [this]() { return createAnimation(5); });

        m_handler->eventFilter(
            m_testWidget,
            new QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier));
        m_handler->eventFilter(
            m_testWidget,
            new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier));
        m_handler->eventFilter(
            m_testWidget,
            new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier));

        QVERIFY(m_handler->m_keyAnimations[Qt::Key_Right].isWaiting);
        QVERIFY(m_handler->m_keyAnimations[Qt::Key_Up].isWaiting);
    }

    void testClearAllEmpty() {
        m_handler->clearAllAnimations();
        QVERIFY(m_handler->m_keyAnimations.isEmpty());
        QVERIFY(!m_handler->m_isAnyAnimationRunning);
    }

    void testClearAllWithRegistered() {
        m_handler->registerKeyAnimation(Qt::Key_Left,
                                        [this]() { return createAnimation(); });
        m_handler->registerKeyAnimation(Qt::Key_Right,
                                        [this]() { return createAnimation(); });
        QCOMPARE(m_handler->m_keyAnimations.size(), 2);

        m_handler->clearAllAnimations();
        QVERIFY(m_handler->m_keyAnimations.isEmpty());
        QVERIFY(!m_handler->m_isAnyAnimationRunning);
    }

    void testClearAllWithRunningAnimation() {
        m_handler->registerKeyAnimation(Qt::Key_Left,
                                        [this]() { return createAnimation(200); });

        QKeyEvent event(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
        m_handler->eventFilter(m_testWidget, &event);
        QVERIFY(m_handler->m_isAnyAnimationRunning);

        m_handler->clearAllAnimations();
        QVERIFY(m_handler->m_keyAnimations.isEmpty());
        QVERIFY(!m_handler->m_isAnyAnimationRunning);
    }

    void testClearAllWithQueuedAnimation() {
        m_handler->registerKeyAnimation(Qt::Key_Left,
                                        [this]() { return createAnimation(200); });
        m_handler->registerKeyAnimation(Qt::Key_Right,
                                        [this]() { return createAnimation(5); });

        m_handler->eventFilter(
            m_testWidget,
            new QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier));
        m_handler->eventFilter(
            m_testWidget,
            new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier));
        QVERIFY(m_handler->m_keyAnimations[Qt::Key_Right].isWaiting);

        m_handler->clearAllAnimations();
        QVERIFY(m_handler->m_keyAnimations.isEmpty());
        QVERIFY(!m_handler->m_isAnyAnimationRunning);
    }

    void testClearAllThenReRegister() {
        m_handler->registerKeyAnimation(Qt::Key_Left,
                                        [this]() { return createAnimation(); });
        m_handler->clearAllAnimations();

        m_handler->registerKeyAnimation(Qt::Key_Left,
                                        [this]() { return createAnimation(); });
        QCOMPARE(m_handler->m_keyAnimations.size(), 1);

        QKeyEvent event(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
        m_handler->eventFilter(m_testWidget, &event);
        QVERIFY(m_handler->m_isAnyAnimationRunning);
    }

    void testCreatorNotCalledWhenKeyQueued() {
        int rightCallCount = 0;
        m_handler->registerKeyAnimation(Qt::Key_Left,
                                        [this]() { return createAnimation(200); });
        m_handler->registerKeyAnimation(Qt::Key_Right, [&rightCallCount, this]() {
            rightCallCount++;
            return createAnimation(5);
        });

        m_handler->eventFilter(
            m_testWidget,
            new QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier));
        m_handler->eventFilter(
            m_testWidget,
            new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier));

        QCOMPARE(rightCallCount, 0);
    }

    void testStartQueuedAnimationNonExistentKey() {
        m_handler->startQueuedAnimation(9999);
        QVERIFY(!m_handler->m_isAnyAnimationRunning);
    }

    void testStartQueuedAnimationNotWaiting() {
        m_handler->registerKeyAnimation(Qt::Key_Left,
                                        [this]() { return createAnimation(); });
        m_handler->startQueuedAnimation(Qt::Key_Left);
        QVERIFY(!m_handler->m_isAnyAnimationRunning);
    }

    void testStartQueuedAnimationTriggers() {
        int callCount = 0;
        m_handler->registerKeyAnimation(Qt::Key_Left, [&callCount, this]() {
            callCount++;
            return createAnimation(5);
        });

        m_handler->m_keyAnimations[Qt::Key_Left].isWaiting = true;

        m_handler->startQueuedAnimation(Qt::Key_Left);
        QCOMPARE(callCount, 1);
        QVERIFY(m_handler->m_isAnyAnimationRunning);
        QVERIFY(!m_handler->m_keyAnimations[Qt::Key_Left].isWaiting);
    }

    void testUnknownKeyDoesNotCrash() {
        QKeyEvent event(QEvent::KeyPress, Qt::Key_unknown, Qt::NoModifier);
        bool result = m_handler->eventFilter(m_testWidget, &event);
        QVERIFY(!result);
    }

    void testUnregisteredKeyRightReturnsFalse() {
        m_handler->registerKeyAnimation(Qt::Key_Left,
                                        [this]() { return createAnimation(); });

        QKeyEvent rightEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier);
        QVERIFY(!m_handler->eventFilter(m_testWidget, &rightEvent));

        QKeyEvent leftEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
        QVERIFY(m_handler->eventFilter(m_testWidget, &leftEvent));
    }

    void testMultipleKeysIndependent() {
        int leftCount = 0, rightCount = 0;
        m_handler->registerKeyAnimation(Qt::Key_Left, [&leftCount, this]() {
            leftCount++;
            return createAnimation(5);
        });
        m_handler->registerKeyAnimation(Qt::Key_Right, [&rightCount, this]() {
            rightCount++;
            return createAnimation(5);
        });

        m_handler->eventFilter(
            m_testWidget,
            new QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier));
        QCOMPARE(leftCount, 1);
        QCOMPARE(rightCount, 0);
        QSequentialAnimationGroup* anim =
            m_handler->m_keyAnimations[Qt::Key_Left].currentAnimation;
        QSignalSpy spy(anim, &QSequentialAnimationGroup::finished);
        QTRY_VERIFY_WITH_TIMEOUT(spy.count() > 0, 2000);

        m_handler->eventFilter(
            m_testWidget,
            new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier));
        QCOMPARE(rightCount, 1);
    }

    void testDestroyHandlerWithRunningAnimation() {
        m_handler->registerKeyAnimation(Qt::Key_Left,
                                        [this]() { return createAnimation(200); });

        m_handler->eventFilter(
            m_testWidget,
            new QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier));
        QVERIFY(m_handler->m_isAnyAnimationRunning);
    }

    void testRapidKeyPresses() {
        int callCount = 0;
        m_handler->registerKeyAnimation(Qt::Key_A, [&callCount, this]() {
            callCount++;
            return createAnimation(50);
        });

        for (int i = 0; i < 5; ++i) {
            m_handler->eventFilter(
                m_testWidget,
                new QKeyEvent(QEvent::KeyPress, Qt::Key_A, Qt::NoModifier));
        }

        QCOMPARE(callCount, 1);
        QVERIFY(m_handler->m_isAnyAnimationRunning);
        QVERIFY(m_handler->m_keyAnimations[Qt::Key_A].isWaiting);

        QSequentialAnimationGroup* anim =
            m_handler->m_keyAnimations[Qt::Key_A].currentAnimation;
        QSignalSpy spy(anim, &QSequentialAnimationGroup::finished);
        QTRY_VERIFY_WITH_TIMEOUT(spy.count() > 0, 2000);
        QTRY_COMPARE_WITH_TIMEOUT(callCount, 2, 1000);
    }

    void testStartAnimationEffectsDisabled() {
        Constants::isPlayEffects = false;
        m_handler->m_effect->setSource(QUrl());

        m_handler->registerKeyAnimation(Qt::Key_Left,
                                        [this]() { return createAnimation(); });

        QKeyEvent event(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
        m_handler->eventFilter(m_testWidget, &event);

        QVERIFY(m_handler->m_effect->source().isEmpty());
    }

    void testStartAnimationThemeDefault() {
        Constants::isPlayEffects = true;
        Constants::gameTheme = 0;
        m_handler->m_effect->setSource(QUrl());

        m_handler->registerKeyAnimation(Qt::Key_Left,
                                        [this]() { return createAnimation(); });

        QKeyEvent event(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
        m_handler->eventFilter(m_testWidget, &event);

        QCOMPARE(m_handler->m_effect->source(), QUrl("qrc:///resources/effect.wav"));
    }

    void testStartAnimationThemeCaixukun() {
        Constants::isPlayEffects = true;
        Constants::gameTheme = 1;
        m_handler->m_effect->setSource(QUrl());

        m_handler->registerKeyAnimation(Qt::Key_Left,
                                        [this]() { return createAnimation(); });

        QKeyEvent event(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
        m_handler->eventFilter(m_testWidget, &event);

        QCOMPARE(m_handler->m_effect->source(), QUrl("qrc:///resources/kuneffect.wav"));
    }
};

QTEST_MAIN(TestKeyboardHandler)
#include "test_keyboardhandler.moc"
