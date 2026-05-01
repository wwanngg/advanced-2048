#include <QAction>
#include <QApplication>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QVector>
#include <QtCore>
#include <QtTest>
#include <qtestcase.h>

#include "../include/constants.h"
#include "../include/mainwindow.h"

class TestMainWindow : public QObject {
    Q_OBJECT

private:
    MainWindow* m_window;

    void setupBoard(const QVector<QVector<int>>& board) {
        auto animations = m_window->findChildren<QAbstractAnimation*>();
        for (auto anim : animations) {
            anim->stop();
            anim->deleteLater();
        }

        for (int i = 0; i < Constants::imax; ++i) {
            for (int j = 0; j < Constants::jmax; ++j) {
                if (m_window->m_labelMap[i][j]) {
                    m_window->m_labelMap[i][j]->deleteLater();
                    m_window->m_labelMap[i][j] = nullptr;
                }
                m_window->m_intMap[i][j] = board[i][j];
                m_window->m_surviveCount[i][j] = (board[i][j] == 0) ? -1 : 0;

                if (board[i][j] != 0) {
                    auto* label = new QLabel(QString::number(board[i][j]),
                                             m_window->m_centralWidget);
                    label->setGeometry(j * Constants::labelSize,
                                       i * Constants::labelSize,
                                       Constants::labelSize,
                                       Constants::labelSize);
                    label->show();
                    m_window->m_labelMap[i][j] = label;
                }
            }
        }
    }

private slots:
    void initTestCase() {
        qputenv("QT_QPA_PLATFORM", "offscreen");
    }

    void init() {
        Constants::imax = 4;
        Constants::jmax = 4;
        Constants::windowSizeX = 600;
        Constants::windowSizeY = 600;
        Constants::duration = 800;
        Constants::animEasingCurve = QEasingCurve::InOutQuad;

        QSettings settings("wwanngg", "2048advanced");
        settings.remove("Game/ColorTheme");
        settings.remove("Game/GameTheme");
        settings.remove("Personalize/mapX");
        settings.remove("Personalize/mapY");
        settings.remove("Personalize/animationDuration");
        settings.remove("Personalize/animationEasingCurve");

        m_window = new MainWindow();
    }

    void cleanup() {
        delete m_window;
        m_window = nullptr;
    }

    void testIsEmpty_EmptyBoard() {
        QVector<QVector<int>> board(4, QVector<int>(4, 0));
        setupBoard(board);
        QVERIFY(m_window->isEmpty());
    }

    void testIsEmpty_NonEmpty() {
        QVector<QVector<int>> board = {
            {2, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);
        QVERIFY(!m_window->isEmpty());
    }

    void testValidateLeft_Invalid() {
        QVector<QVector<int>> board = {
            {2, 4, 8, 16}, {4, 8, 16, 32}, {8, 16, 32, 64}, {16, 32, 64, 128}};
        setupBoard(board);
        QVERIFY(!m_window->validate('a'));
    }

    void testValidateLeft_Valid_Gap() {
        QVector<QVector<int>> board = {
            {2, 0, 4, 8}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);
        QVERIFY(m_window->validate('a'));
    }

    void testValidateLeft_Valid_Merge() {
        QVector<QVector<int>> board = {
            {2, 2, 4, 8}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);
        QVERIFY(m_window->validate('a'));
    }

    void testValidateRight_Invalid() {
        QVector<QVector<int>> board = {
            {16, 8, 4, 2}, {32, 16, 8, 4}, {64, 32, 16, 8}, {128, 64, 32, 16}};
        setupBoard(board);
        QVERIFY(!m_window->validate('d'));
    }

    void testValidateRight_Valid_Gap() {
        QVector<QVector<int>> board = {
            {8, 4, 0, 2}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);
        QVERIFY(m_window->validate('d'));
    }

    void testValidateRight_Valid_Merge() {
        QVector<QVector<int>> board = {
            {2, 4, 8, 8}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);
        QVERIFY(m_window->validate('d'));
    }

    void testValidateUp_Invalid() {
        QVector<QVector<int>> board = {
            {2, 4, 8, 16}, {4, 8, 16, 32}, {8, 16, 32, 64}, {16, 32, 64, 128}};
        setupBoard(board);
        QVERIFY(!m_window->validate('w'));
    }

    void testValidateUp_Valid_Gap() {
        QVector<QVector<int>> board = {
            {2, 0, 0, 0}, {0, 0, 0, 0}, {4, 0, 0, 0}, {8, 0, 0, 0}};
        setupBoard(board);
        QVERIFY(m_window->validate('w'));
    }

    void testValidateUp_Valid_Merge() {
        QVector<QVector<int>> board = {
            {2, 0, 0, 0}, {2, 0, 0, 0}, {4, 0, 0, 0}, {8, 0, 0, 0}};
        setupBoard(board);
        QVERIFY(m_window->validate('w'));
    }

    void testValidateDown_Invalid() {
        QVector<QVector<int>> board = {
            {16, 32, 64, 128},
            {8, 16, 32, 64},
            {4, 8, 16, 32},
            {2, 4, 8, 16}};
        setupBoard(board);
        QVERIFY(!m_window->validate('s'));
    }

    void testValidateDown_Valid_Gap() {
        QVector<QVector<int>> board = {
            {2, 0, 0, 0}, {4, 0, 0, 0}, {0, 0, 0, 0}, {8, 0, 0, 0}};
        setupBoard(board);
        QVERIFY(m_window->validate('s'));
    }

    void testValidateDown_Valid_Merge() {
        QVector<QVector<int>> board = {
            {2, 0, 0, 0}, {4, 0, 0, 0}, {8, 0, 0, 0}, {8, 0, 0, 0}};
        setupBoard(board);
        QVERIFY(m_window->validate('s'));
    }

    void testillegalDirection() {
        QVERIFY(!m_window->validate(' '));
    }

    void testAddSurviveCount() {
        m_window->m_surviveCount[0][0] = -1;
        m_window->m_surviveCount[0][1] = 0;
        m_window->m_surviveCount[1][1] = 5;
        m_window->m_surviveCount[2][2] = 10;

        m_window->addSurviveCount();

        QCOMPARE(m_window->m_surviveCount[0][0], -1);
        QCOMPARE(m_window->m_surviveCount[0][1], 1);
        QCOMPARE(m_window->m_surviveCount[1][1], 6);
        QCOMPARE(m_window->m_surviveCount[2][2], 11);
    }

    void testMoveLeft_Slide() {
        QVector<QVector<int>> board = {
            {2, 0, 0, 0}, {0, 2, 0, 0}, {0, 0, 4, 0}, {0, 0, 0, 8}};
        setupBoard(board);

        auto* anim = m_window->createAnimationForKeyA();
        Q_UNUSED(anim);

        QCOMPARE(m_window->m_intMap[0][0], 2);
        QCOMPARE(m_window->m_intMap[1][0], 2);
        QCOMPARE(m_window->m_intMap[2][0], 4);
        QCOMPARE(m_window->m_intMap[3][0], 8);
        QCOMPARE(m_window->m_score, 0);
    }

    void testMoveLeft_Merge() {
        QVector<QVector<int>> board = {
            {2, 2, 0, 0}, {0, 0, 0, 0}, {4, 4, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);

        auto* anim = m_window->createAnimationForKeyA();
        Q_UNUSED(anim);

        QCOMPARE(m_window->m_intMap[0][0], 4);
        QCOMPARE(m_window->m_intMap[2][0], 8);
        QCOMPARE(m_window->m_score, 12);
    }

    void testMoveLeft_Running() {
        QVector<QVector<int>> board = {
            {2, 0, 0, 0}, {0, 2, 0, 0}, {0, 0, 4, 0}, {0, 0, 0, 8}};
        setupBoard(board);
        auto* anim = m_window->createAnimationForKeyA();
        QVERIFY(m_window->m_animationRunning);
        QSignalSpy finishedSpy(anim, &QSequentialAnimationGroup::finished);
        anim->start();
        QTRY_COMPARE_WITH_TIMEOUT(finishedSpy.count(), 1, 1200);
        QVERIFY(!m_window->m_animationRunning);
    }

    void testMoveLeft_Failed() {
        QVector<QVector<int>> board = {
            {2, 4, 8, 16}, {4, 2, 16, 8}, {2, 4, 8, 16}, {4, 2, 16, 8}};
        setupBoard(board);
        auto* anim = m_window->createAnimationForKeyA();
        QVERIFY(anim->animationCount() == 1);
        bool messageShown = false;
        bool windowClosed = false;
        m_window->showMessageCallback = [&]() {
            messageShown = true;
            windowClosed = true;
        };
        QSignalSpy finishedSpy(anim, &QSequentialAnimationGroup::finished);
        anim->start();
        QTRY_COMPARE_WITH_TIMEOUT(finishedSpy.count(), 1, 1200);
        QVERIFY(messageShown);
        QVERIFY(windowClosed);
        m_window->showMessageCallback = nullptr;
    }

    void testMoveLeft_Empty() {
        QVector<QVector<int>> board = {
            {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);
        auto* anim = m_window->createAnimationForKeyA();
        QVERIFY(anim->animationCount() == 1);
    }

    void testMoveLeft_OtherRunning() {
        QVector<QVector<int>> board = {
            {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);
        m_window->m_animationRunning = true;
        auto* anim = m_window->createAnimationForKeyA();
        QVERIFY(anim->animationCount() == 0);
        m_window->m_animationRunning = false;
    }

    void testMoveLeft_CaixukunStyle() {
        m_window->m_gametype = MainWindow::caixukun;
        QVector<QVector<int>> board = {
            {2, 2, 0, 0}, {2, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);

        auto* anim = m_window->createAnimationForKeyA();
        Q_UNUSED(anim);

        QCOMPARE(m_window->m_labelMap[0][0]->styleSheet(), Constants::caixukunStyles[4]);
        m_window->m_gametype = MainWindow::number;
    }

    void testMoveLeft_ChemistryStyle() {
        m_window->m_gametype = MainWindow::chemistry;
        QVector<QVector<int>> board = {
            {4, 4, 0, 0}, {8, 8, 0, 0}, {8, 0, 0, 0}, {0, 8, 16, 8}};
        setupBoard(board);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (board[i][j])
                    m_window->m_surviveCount[i][j] = 1;
            }
        }
        m_window->m_surviveCount[3][3] = 5;
        auto* anim = m_window->createAnimationForKeyA();
        Q_UNUSED(anim);
        QCOMPARE(m_window->m_labelMap[0][0]->text(), QString("Be8(5)"));
        QCOMPARE(m_window->m_labelMap[0][0]->styleSheet(), Constants::chemistryStyles[8]);
        QCOMPARE(m_window->m_labelMap[1][0]->text(), QString("O16"));
        QCOMPARE(m_window->m_labelMap[2][0]->text(), QString("Be8(3)"));
        QCOMPARE(m_window->m_labelMap[3][0]->text(), QString("Be8(3)"));
        QCOMPARE(m_window->m_labelMap[3][1]->text(), QString("O16"));
        QCOMPARE(m_window->m_intMap[3][2], 0);
        QCOMPARE(m_window->m_labelMap[3][2], nullptr);
        m_window->m_gametype = MainWindow::number;
    }

    void testMoveLeft_ChemistryStyle_more() {
        m_window->m_gametype = MainWindow::chemistry;
        QVector<QVector<int>> board = {
            {8, 0, 0, 0}, {16, 8, 0, 0}, {32, 16, 0, 0}, {32, 8, 0, 4}};
        setupBoard(board);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (board[i][j])
                    m_window->m_surviveCount[i][j] = 1;
            }
        }
        m_window->m_surviveCount[1][1] = 5;
        m_window->m_surviveCount[0][0] = 5;
        auto* anim = m_window->createAnimationForKeyA();
        Q_UNUSED(anim);
        QCOMPARE(m_window->m_intMap[0][0], 0);
        QCOMPARE(m_window->m_intMap[1][1], 0);
        QCOMPARE(m_window->m_labelMap[0][0], nullptr);
        QCOMPARE(m_window->m_labelMap[1][1], nullptr);
        QCOMPARE(m_window->m_labelMap[2][1]->text(), QString("O16"));
        QCOMPARE(m_window->m_labelMap[3][1]->text(), QString("Be8(3)"));
        m_window->m_gametype = MainWindow::number;
    }

    void testMoveLeft_ChemistryStyle_evenmore() {
        m_window->m_gametype = MainWindow::chemistry;
        QVector<QVector<int>> board = {
            {0, 0, 0, 0}, {0, 0, 0, 2}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (board[i][j])
                    m_window->m_surviveCount[i][j] = 1;
            }
        }
        auto* anim = m_window->createAnimationForKeyA();
        Q_UNUSED(anim);
        QCOMPARE(m_window->m_intMap[1][0], 2);
        m_window->m_gametype = MainWindow::number;
    }

    void testMoveRight_Slide() {
        QVector<QVector<int>> board = {
            {0, 0, 0, 2}, {0, 0, 2, 0}, {0, 2, 0, 0}, {2, 0, 0, 0}};
        setupBoard(board);

        auto* anim = m_window->createAnimationForKeyD();
        Q_UNUSED(anim);

        QCOMPARE(m_window->m_intMap[0][3], 2);
        QCOMPARE(m_window->m_intMap[1][3], 2);
        QCOMPARE(m_window->m_intMap[2][3], 2);
        QCOMPARE(m_window->m_intMap[3][3], 2);
        QCOMPARE(m_window->m_score, 0);
    }

    void testMoveRight_Merge() {
        QVector<QVector<int>> board = {
            {0, 0, 2, 2}, {0, 0, 0, 0}, {0, 0, 8, 8}, {0, 0, 0, 0}};
        setupBoard(board);

        auto* anim = m_window->createAnimationForKeyD();
        Q_UNUSED(anim);

        QCOMPARE(m_window->m_intMap[0][3], 4);
        QCOMPARE(m_window->m_intMap[2][3], 16);
        QCOMPARE(m_window->m_score, 20);
    }

    void testMoveRight_Running() {
        QVector<QVector<int>> board = {
            {2, 0, 0, 0}, {0, 2, 0, 0}, {0, 0, 4, 0}, {0, 0, 0, 8}};
        setupBoard(board);
        auto* anim = m_window->createAnimationForKeyD();
        QVERIFY(m_window->m_animationRunning);
        QSignalSpy finishedSpy(anim, &QSequentialAnimationGroup::finished);
        anim->start();
        QTRY_COMPARE_WITH_TIMEOUT(finishedSpy.count(), 1, 1200);
        QVERIFY(!m_window->m_animationRunning);
    }

    void testMoveRight_Failed() {
        QVector<QVector<int>> board = {
            {2, 4, 8, 16}, {4, 2, 16, 8}, {2, 4, 8, 16}, {4, 2, 16, 8}};
        setupBoard(board);
        auto* anim = m_window->createAnimationForKeyD();
        QVERIFY(anim->animationCount() == 1);
        bool messageShown = false;
        bool windowClosed = false;
        m_window->showMessageCallback = [&]() {
            messageShown = true;
            windowClosed = true;
        };
        QSignalSpy finishedSpy(anim, &QSequentialAnimationGroup::finished);
        anim->start();
        QTRY_COMPARE_WITH_TIMEOUT(finishedSpy.count(), 1, 1200);
        QVERIFY(messageShown);
        QVERIFY(windowClosed);
        m_window->showMessageCallback = nullptr;
    }

    void testMoveRight_Empty() {
        QVector<QVector<int>> board = {
            {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);
        auto* anim = m_window->createAnimationForKeyD();
        QVERIFY(anim->animationCount() == 1);
    }

    void testMoveRight_OtherRunning() {
        QVector<QVector<int>> board = {
            {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);
        m_window->m_animationRunning = true;
        auto* anim = m_window->createAnimationForKeyD();
        QVERIFY(anim->animationCount() == 0);
        m_window->m_animationRunning = false;
    }

    void testMoveRight_CaixukunStyle() {
        m_window->m_gametype = MainWindow::caixukun;
        QVector<QVector<int>> board = {
            {2, 2, 0, 0}, {2, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);

        auto* anim = m_window->createAnimationForKeyD();
        Q_UNUSED(anim);

        QCOMPARE(m_window->m_labelMap[0][3]->styleSheet(), Constants::caixukunStyles[4]);
        m_window->m_gametype = MainWindow::number;
    }

    void testMoveRight_ChemistryStyle() {
        m_window->m_gametype = MainWindow::chemistry;
        QVector<QVector<int>> board = {
            {0, 0, 4, 4}, {0, 0, 8, 8}, {0, 0, 0, 8}, {8, 16, 8, 0}};
        setupBoard(board);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (board[i][j])
                    m_window->m_surviveCount[i][j] = 1;
            }
        }
        m_window->m_surviveCount[3][0] = 4;
        auto* anim = m_window->createAnimationForKeyD();
        Q_UNUSED(anim);
        QCOMPARE(m_window->m_labelMap[0][3]->text(), QString("Be8(5)"));
        QCOMPARE(m_window->m_labelMap[0][3]->styleSheet(), Constants::chemistryStyles[8]);
        QCOMPARE(m_window->m_labelMap[1][3]->text(), QString("O16"));
        QCOMPARE(m_window->m_labelMap[2][3]->text(), QString("Be8(3)"));
        QCOMPARE(m_window->m_labelMap[3][3]->text(), QString("Be8(3)"));
        QCOMPARE(m_window->m_labelMap[3][2]->text(), QString("O16"));
        QCOMPARE(m_window->m_intMap[3][1], 0);
        QCOMPARE(m_window->m_labelMap[3][1], nullptr);
        m_window->m_gametype = MainWindow::number;
    }

    void testMoveRight_ChemistryStyle_more() {
        m_window->m_gametype = MainWindow::chemistry;
        QVector<QVector<int>> board = {
            {0, 0, 0, 8}, {0, 0, 8, 16}, {0, 0, 16, 32}, {4, 0, 8, 32}};
        setupBoard(board);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (board[i][j])
                    m_window->m_surviveCount[i][j] = 1;
            }
        }
        m_window->m_surviveCount[1][2] = 5;
        m_window->m_surviveCount[0][3] = 5;
        auto* anim = m_window->createAnimationForKeyD();
        Q_UNUSED(anim);
        QCOMPARE(m_window->m_intMap[0][3], 0);
        QCOMPARE(m_window->m_intMap[1][2], 0);
        QCOMPARE(m_window->m_labelMap[0][3], nullptr);
        QCOMPARE(m_window->m_labelMap[1][2], nullptr);
        QCOMPARE(m_window->m_labelMap[2][2]->text(), QString("O16"));
        QCOMPARE(m_window->m_labelMap[3][2]->text(), QString("Be8(3)"));
        m_window->m_gametype = MainWindow::number;
    }

    void testMoveUp_Slide() {
        QVector<QVector<int>> board = {
            {2, 0, 0, 0}, {0, 2, 0, 0}, {0, 0, 2, 0}, {0, 0, 0, 2}};
        setupBoard(board);

        auto* anim = m_window->createAnimationForKeyW();
        Q_UNUSED(anim);

        QCOMPARE(m_window->m_intMap[0][0], 2);
        QCOMPARE(m_window->m_intMap[0][1], 2);
        QCOMPARE(m_window->m_intMap[0][2], 2);
        QCOMPARE(m_window->m_intMap[0][3], 2);
        QCOMPARE(m_window->m_score, 0);
    }

    void testMoveUp_Merge() {
        QVector<QVector<int>> board = {
            {2, 0, 0, 0}, {2, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);

        auto* anim = m_window->createAnimationForKeyW();
        Q_UNUSED(anim);

        QCOMPARE(m_window->m_intMap[0][0], 4);
        QCOMPARE(m_window->m_score, 4);
    }

    void testMoveUp_Running() {
        QVector<QVector<int>> board = {
            {2, 0, 0, 0}, {0, 2, 0, 0}, {0, 0, 4, 0}, {0, 0, 0, 8}};
        setupBoard(board);
        auto* anim = m_window->createAnimationForKeyW();
        QVERIFY(m_window->m_animationRunning);
        QSignalSpy finishedSpy(anim, &QSequentialAnimationGroup::finished);
        anim->start();
        QTRY_COMPARE_WITH_TIMEOUT(finishedSpy.count(), 1, 1200);
        QVERIFY(!m_window->m_animationRunning);
    }

    void testMoveUp_Failed() {
        QVector<QVector<int>> board = {
            {2, 4, 8, 16}, {4, 2, 16, 8}, {2, 4, 8, 16}, {4, 2, 16, 8}};
        setupBoard(board);
        auto* anim = m_window->createAnimationForKeyW();
        QVERIFY(anim->animationCount() == 1);
        bool messageShown = false;
        bool windowClosed = false;
        m_window->showMessageCallback = [&]() {
            messageShown = true;
            windowClosed = true;
        };
        QSignalSpy finishedSpy(anim, &QSequentialAnimationGroup::finished);
        anim->start();
        QTRY_COMPARE_WITH_TIMEOUT(finishedSpy.count(), 1, 1200);
        QVERIFY(messageShown);
        QVERIFY(windowClosed);
        m_window->showMessageCallback = nullptr;
    }

    void testMoveUp_Empty() {
        QVector<QVector<int>> board = {
            {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);
        auto* anim = m_window->createAnimationForKeyW();
        QVERIFY(anim->animationCount() == 1);
    }

    void testMoveUp_OtherRunning() {
        QVector<QVector<int>> board = {
            {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);
        m_window->m_animationRunning = true;
        auto* anim = m_window->createAnimationForKeyW();
        QVERIFY(anim->animationCount() == 0);
        m_window->m_animationRunning = false;
    }

    void testMoveUp_CaixukunStyle() {
        m_window->m_gametype = MainWindow::caixukun;
        QVector<QVector<int>> board = {
            {2, 2, 0, 0}, {2, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);

        auto* anim = m_window->createAnimationForKeyW();
        Q_UNUSED(anim);

        QCOMPARE(m_window->m_labelMap[0][0]->styleSheet(), Constants::caixukunStyles[4]);
        m_window->m_gametype = MainWindow::number;
    }

    void testMoveUp_ChemistryStyle() {
        m_window->m_gametype = MainWindow::chemistry;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                m_window->m_intMap[i][j] = 0;
            }
        }
        QVector<QVector<int>> board = {
            {4, 8, 8, 0}, {4, 8, 0, 8}, {0, 0, 0, 16}, {0, 0, 0, 8}};
        setupBoard(board);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (board[i][j])
                    m_window->m_surviveCount[i][j] = 1;
            }
        }
        m_window->m_surviveCount[3][3] = 4;
        auto* anim = m_window->createAnimationForKeyW();
        Q_UNUSED(anim);
        QCOMPARE(m_window->m_labelMap[0][0]->text(), QString("Be8(5)"));
        QCOMPARE(m_window->m_labelMap[0][0]->styleSheet(), Constants::chemistryStyles[8]);
        QCOMPARE(m_window->m_labelMap[0][1]->text(), QString("O16"));
        QCOMPARE(m_window->m_labelMap[0][2]->text(), QString("Be8(3)"));
        QCOMPARE(m_window->m_labelMap[0][3]->text(), QString("Be8(3)"));
        QCOMPARE(m_window->m_labelMap[1][3]->text(), QString("O16"));
        QCOMPARE(m_window->m_intMap[2][3], 0);
        QCOMPARE(m_window->m_labelMap[2][3], nullptr);
        m_window->m_gametype = MainWindow::number;
    }

    void testMoveUp_ChemistryStyle_more() {
        m_window->m_gametype = MainWindow::chemistry;
        QVector<QVector<int>> board = {
            {8, 16, 32, 32}, {0, 8, 16, 8}, {0, 0, 0, 0}, {0, 0, 0, 4}};
        setupBoard(board);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (board[i][j])
                    m_window->m_surviveCount[i][j] = 1;
            }
        }
        m_window->m_surviveCount[1][1] = 5;
        m_window->m_surviveCount[0][0] = 5;
        auto* anim = m_window->createAnimationForKeyW();
        Q_UNUSED(anim);
        QCOMPARE(m_window->m_intMap[0][0], 0);
        QCOMPARE(m_window->m_intMap[1][1], 0);
        QCOMPARE(m_window->m_labelMap[0][0], nullptr);
        QCOMPARE(m_window->m_labelMap[1][1], nullptr);
        QCOMPARE(m_window->m_labelMap[1][2]->text(), QString("O16"));
        QCOMPARE(m_window->m_labelMap[1][3]->text(), QString("Be8(3)"));
        m_window->m_gametype = MainWindow::number;
    }

    void testMoveUp_ChemistryStyle_evenmore() {
        m_window->m_gametype = MainWindow::chemistry;
        QVector<QVector<int>> board = {
            {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 2, 0, 0}};
        setupBoard(board);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (board[i][j])
                    m_window->m_surviveCount[i][j] = 1;
            }
        }
        auto* anim = m_window->createAnimationForKeyW();
        Q_UNUSED(anim);
        QCOMPARE(m_window->m_intMap[0][1], 2);
        m_window->m_gametype = MainWindow::number;
    }

    void testMoveDown_Slide() {
        QVector<QVector<int>> board = {
            {2, 0, 0, 0}, {0, 2, 0, 0}, {0, 0, 2, 0}, {0, 0, 0, 2}};
        setupBoard(board);

        auto* anim = m_window->createAnimationForKeyS();
        Q_UNUSED(anim);

        QCOMPARE(m_window->m_intMap[3][0], 2);
        QCOMPARE(m_window->m_intMap[3][1], 2);
        QCOMPARE(m_window->m_intMap[3][2], 2);
        QCOMPARE(m_window->m_intMap[3][3], 2);
        QCOMPARE(m_window->m_score, 0);
    }

    void testMoveDown_Merge() {
        QVector<QVector<int>> board = {
            {0, 0, 0, 0}, {0, 0, 0, 0}, {8, 0, 0, 0}, {8, 0, 0, 0}};
        setupBoard(board);

        auto* anim = m_window->createAnimationForKeyS();
        Q_UNUSED(anim);

        QCOMPARE(m_window->m_intMap[3][0], 16);
        QCOMPARE(m_window->m_score, 16);
    }

    void testMoveDown_Running() {
        QVector<QVector<int>> board = {
            {2, 0, 0, 0}, {0, 2, 0, 0}, {0, 0, 4, 0}, {0, 0, 0, 8}};
        setupBoard(board);
        auto* anim = m_window->createAnimationForKeyS();
        QVERIFY(m_window->m_animationRunning);
        QSignalSpy finishedSpy(anim, &QSequentialAnimationGroup::finished);
        anim->start();
        QTRY_COMPARE_WITH_TIMEOUT(finishedSpy.count(), 1, 1200);
        QVERIFY(!m_window->m_animationRunning);
    }

    void testMoveDown_Failed() {
        QVector<QVector<int>> board = {
            {2, 4, 8, 16}, {4, 2, 16, 8}, {2, 4, 8, 16}, {4, 2, 16, 8}};
        setupBoard(board);
        auto* anim = m_window->createAnimationForKeyS();
        QVERIFY(anim->animationCount() == 1);
        bool messageShown = false;
        bool windowClosed = false;
        m_window->showMessageCallback = [&]() {
            messageShown = true;
            windowClosed = true;
        };
        QSignalSpy finishedSpy(anim, &QSequentialAnimationGroup::finished);
        anim->start();
        QTRY_COMPARE_WITH_TIMEOUT(finishedSpy.count(), 1, 1200);
        QVERIFY(messageShown);
        QVERIFY(windowClosed);
        m_window->showMessageCallback = nullptr;
    }

    void testMoveDown_Empty() {
        QVector<QVector<int>> board = {
            {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);
        auto* anim = m_window->createAnimationForKeyS();
        QVERIFY(anim->animationCount() == 1);
    }

    void testMoveDown_OtherRunning() {
        QVector<QVector<int>> board = {
            {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);
        m_window->m_animationRunning = true;
        auto* anim = m_window->createAnimationForKeyS();
        QVERIFY(anim->animationCount() == 0);
        m_window->m_animationRunning = false;
    }

    void testMoveDown_CaixukunStyle() {
        m_window->m_gametype = MainWindow::caixukun;
        QVector<QVector<int>> board = {
            {2, 2, 0, 0}, {2, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);

        auto* anim = m_window->createAnimationForKeyS();
        Q_UNUSED(anim);

        QCOMPARE(m_window->m_labelMap[3][0]->styleSheet(), Constants::caixukunStyles[4]);
        m_window->m_gametype = MainWindow::number;
    }

    void testMoveDown_ChemistryStyle() {
        m_window->m_gametype = MainWindow::chemistry;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                m_window->m_intMap[i][j] = 0;
            }
        }
        QVector<QVector<int>> board = {
            {0, 0, 0, 8}, {0, 0, 0, 16}, {4, 8, 0, 8}, {4, 8, 8, 0}};
        setupBoard(board);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (board[i][j])
                    m_window->m_surviveCount[i][j] = 1;
            }
        }
        m_window->m_surviveCount[0][3] = 4;
        auto* anim = m_window->createAnimationForKeyS();
        Q_UNUSED(anim);
        QCOMPARE(m_window->m_labelMap[3][0]->text(), QString("Be8(5)"));
        QCOMPARE(m_window->m_labelMap[3][0]->styleSheet(), Constants::chemistryStyles[8]);
        QCOMPARE(m_window->m_labelMap[3][1]->text(), QString("O16"));
        QCOMPARE(m_window->m_labelMap[3][2]->text(), QString("Be8(3)"));
        QCOMPARE(m_window->m_labelMap[3][3]->text(), QString("Be8(3)"));
        QCOMPARE(m_window->m_labelMap[2][3]->text(), QString("O16"));
        QCOMPARE(m_window->m_intMap[1][3], 0);
        QCOMPARE(m_window->m_labelMap[1][3], nullptr);
        m_window->m_gametype = MainWindow::number;
    }

    void testMoveDown_ChemistryStyle_more() {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                m_window->m_intMap[i][j] = 0;
            }
        }
        m_window->m_gametype = MainWindow::chemistry;
        QVector<QVector<int>> board = {
            {0, 0, 0, 4}, {0, 0, 0, 0}, {0, 8, 16, 8}, {8, 16, 32, 32}};
        setupBoard(board);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (board[i][j])
                    m_window->m_surviveCount[i][j] = 1;
            }
        }
        m_window->m_surviveCount[2][1] = 5;
        m_window->m_surviveCount[3][0] = 5;
        auto* anim = m_window->createAnimationForKeyS();
        Q_UNUSED(anim);
        QCOMPARE(m_window->m_intMap[3][0], 0);
        QCOMPARE(m_window->m_intMap[2][1], 0);
        QCOMPARE(m_window->m_labelMap[3][0], nullptr);
        QCOMPARE(m_window->m_labelMap[2][1], nullptr);
        QCOMPARE(m_window->m_labelMap[2][2]->text(), QString("O16"));
        QCOMPARE(m_window->m_labelMap[2][3]->text(), QString("Be8(3)"));
        m_window->m_gametype = MainWindow::number;
    }

    void testMoveDown_ChemistryStyle_evenmore() {
        m_window->m_gametype = MainWindow::chemistry;
        QVector<QVector<int>> board = {
            {0, 2, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (board[i][j])
                    m_window->m_surviveCount[i][j] = 1;
            }
        }
        auto* anim = m_window->createAnimationForKeyS();
        Q_UNUSED(anim);
        QCOMPARE(m_window->m_intMap[3][1], 2);
        m_window->m_gametype = MainWindow::number;
    }

    void testChainMerge() {
        QVector<QVector<int>> board = {
            {2, 2, 4, 4}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);

        auto* anim = m_window->createAnimationForKeyA();
        Q_UNUSED(anim);

        QCOMPARE(m_window->m_intMap[0][0], 4);
        QCOMPARE(m_window->m_intMap[0][1], 8);
        QCOMPARE(m_window->m_score, 12);
    }

    void testcreateMoveAnimationWithNullptr() {
        QVector<QVector<int>> board = {
            {2, 2, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);

        auto* anim = m_window->createMoveAnimation(m_window->m_labelMap[0][0],
                                                   QRect(0, 0, Constants::labelSize, Constants::labelSize),
                                                   Constants::duration);
        QVERIFY(anim);

        auto* nullanim = m_window->createMoveAnimation(nullptr,
                                                       QRect(0, 0, Constants::labelSize, Constants::labelSize),
                                                       Constants::duration);
        QVERIFY(!nullanim);
    }

    void testcreateDisappearAnimation() {
        QVector<QVector<int>> board = {
            {2, 2, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);

        QWidget* targetWidget = m_window->m_labelMap[0][0];
        QRect startRect = targetWidget->geometry();
        QPointer<QWidget> safePointer = targetWidget;
        QPropertyAnimation* anim = m_window->createDisappearAnimation(
            safePointer, startRect, 300);

        QSignalSpy finishedSpy(anim, &QPropertyAnimation::finished);
        anim->start();
        QTRY_COMPARE_WITH_TIMEOUT(finishedSpy.count(), 1, 1000);
        m_window->m_labelMap[0][0] = nullptr;

        QVERIFY(safePointer.isNull());
    }

    void testcreateDisappearAnimation_nullptr() {
        auto* nullanim = m_window->createDisappearAnimation(nullptr, QRect(0, 0, Constants::labelSize, Constants::labelSize), Constants::duration);
        QVERIFY(!nullanim);
    }

    void testcreateMoveDisappearAnimation_nullptr() {
        auto* nullanim = m_window->createMoveDisappearAnimation(nullptr, QRect(0, 0, Constants::labelSize, Constants::labelSize), Constants::duration);
        QVERIFY(!nullanim);
    }

    void testCreateRandom() {
        QVector<QVector<int>> board = {
            {2, 4, 8, 16}, {4, 2, 16, 8}, {2, 4, 8, 16}, {4, 2, 16, 8}};
        setupBoard(board);
        auto* anim = m_window->createRandom();
        bool messageShown = false;
        bool windowClosed = false;
        m_window->showMessageCallback = [&]() {
            messageShown = true;
            windowClosed = true;
        };
        QSignalSpy finishedSpy(anim, &QSequentialAnimationGroup::finished);
        anim->start();
        QTRY_COMPARE_WITH_TIMEOUT(finishedSpy.count(), 1, 1200);
        QVERIFY(messageShown);
        QVERIFY(windowClosed);
        QCOMPARE(anim->animationCount(), 0);
        m_window->showMessageCallback = nullptr;
    }

    void testCreateRandom_numberWin() {
        m_window->m_gametype = MainWindow::caixukun;
        QVector<QVector<int>> board = {
            {Constants::maxUsedNum, 0, 4, 16}, {4, 2, 16, 8}, {2, 4, 8, 16}, {4, 2, 16, 8}};
        setupBoard(board);
        auto* anim = m_window->createRandom();
        bool messageShown = false;
        bool windowClosed = false;
        m_window->showMessageCallback = [&]() {
            messageShown = true;
            windowClosed = true;
        };
        QSignalSpy finishedSpy(anim, &QSequentialAnimationGroup::finished);
        anim->start();
        QTRY_COMPARE_WITH_TIMEOUT(finishedSpy.count(), 1, 1200);
        QVERIFY(messageShown);
        QVERIFY(windowClosed);
        QCOMPARE(anim->animationCount(), 0);
        m_window->showMessageCallback = nullptr;
        m_window->m_gametype = MainWindow::number;
    }

    void testCreateRandom_chemistryWin() {
        m_window->m_gametype = MainWindow::chemistry;
        QVector<QVector<int>> board = {
            {Constants::chemisrtySuccess, 0, 4, 16}, {4, 2, 16, 8}, {2, 4, 8, 16}, {4, 2, 16, 8}};
        setupBoard(board);
        auto* anim = m_window->createRandom();
        bool messageShown = false;
        bool windowClosed = false;
        m_window->showMessageCallback = [&]() {
            messageShown = true;
            windowClosed = true;
        };
        QSignalSpy finishedSpy(anim, &QSequentialAnimationGroup::finished);
        anim->start();
        QTRY_COMPARE_WITH_TIMEOUT(finishedSpy.count(), 1, 1200);
        QVERIFY(messageShown);
        QVERIFY(windowClosed);
        QCOMPARE(anim->animationCount(), 0);
        m_window->showMessageCallback = nullptr;
        m_window->m_gametype = MainWindow::number;
    }

    void test_KeyAnimationInfo_lambda() {
        for (const auto& info : m_window->m_keyboardHandler->m_keyAnimations) {
            m_window->m_animationRunning = true;
            QVERIFY(!info.creator());
            m_window->m_animationRunning = false;
            QVERIFY(info.creator());
        }
    }

    void testLoadSettings_SetsIsGameStart() {
        m_window->isGameStart = false;
        m_window->loadSettings();
        QVERIFY(m_window->isGameStart);
    }

    void testOnSettingsChanged_ColorThemeDark() {
        QSettings settings("wwanngg", "2048advanced");
        settings.setValue("Game/ColorTheme", "dark");
        settings.setValue("Game/GameTheme", "number");

        m_window->onSettingsChanged();

        QVERIFY(m_window->styleSheet().contains("#2b2b2b"));
    }

    void testOnSettingsChanged_ColorThemeLight() {
        QSettings settings("wwanngg", "2048advanced");
        settings.setValue("Game/ColorTheme", "light");
        settings.setValue("Game/GameTheme", "number");

        m_window->onSettingsChanged();

        QVERIFY(m_window->styleSheet().contains("#f0f0f0"));
    }

    void testOnSettingsChanged_ColorThemeSystem() {
        QSettings settings("wwanngg", "2048advanced");
        settings.setValue("Game/ColorTheme", "system");
        settings.setValue("Game/GameTheme", "number");

        m_window->onSettingsChanged();

        QVERIFY(m_window->styleSheet().isEmpty());
    }

    void testOnSettingsChanged_GameThemeNumber() {
        QVector<QVector<int>> board = {
            {2, 0, 0, 0}, {4, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);

        m_window->m_gametype = MainWindow::caixukun;

        QSettings settings("wwanngg", "2048advanced");
        settings.setValue("Game/GameTheme", "number");
        settings.setValue("Game/ColorTheme", "system");

        m_window->onSettingsChanged();

        QCOMPARE(m_window->m_gametype, MainWindow::number);
        QCOMPARE(m_window->m_labelMap[0][0]->text(), QString("2"));
        QCOMPARE(m_window->m_labelMap[1][0]->text(), QString("4"));

        m_window->m_gametype = MainWindow::number;
    }

    void testOnSettingsChanged_GameThemeCaixukun() {
        QVector<QVector<int>> board = {
            {2, 0, 0, 0}, {4, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);

        m_window->m_gametype = MainWindow::number;

        QSettings settings("wwanngg", "2048advanced");
        settings.setValue("Game/GameTheme", "caixukun");
        settings.setValue("Game/ColorTheme", "system");

        m_window->onSettingsChanged();

        QCOMPARE(m_window->m_gametype, MainWindow::caixukun);

        m_window->m_gametype = MainWindow::number;
    }

    void testOnSettingsChanged_GameThemeChemistry() {
        QVector<QVector<int>> board = {
            {2, 0, 0, 0}, {4, 0, 0, 0}, {0, 8, 0, 0}, {8, 0, 0, 0}};
        setupBoard(board);
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                if (board[i][j])
                    m_window->m_surviveCount[i][j] = 1;

        m_window->m_surviveCount[2][1] = 5;

        m_window->m_gametype = MainWindow::number;

        QSettings settings("wwanngg", "2048advanced");
        settings.setValue("Game/GameTheme", "chemistry");
        settings.setValue("Game/ColorTheme", "system");

        m_window->onSettingsChanged();

        QCOMPARE(m_window->m_gametype, MainWindow::chemistry);
        QCOMPARE(m_window->m_labelMap[0][0]->text(), QString("H2"));
        QCOMPARE(m_window->m_labelMap[2][1], nullptr);
        QCOMPARE(m_window->m_labelMap[3][0]->text(), QString("Be8(4)"));

        m_window->m_gametype = MainWindow::number;
    }

    void testOnSettingsChanged_MapResize_Smaller() {
        QVector<QVector<int>> board = {
            {2, 4, 8, 16}, {1, 2, 3, 4}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);

        m_window->isGameStart = true;

        QSettings settings("wwanngg", "2048advanced");
        settings.setValue("Personalize/mapX", "2");
        settings.setValue("Personalize/mapY", "2");
        settings.setValue("Game/ColorTheme", "system");
        settings.setValue("Game/GameTheme", "number");

        bool messageShown = false;
        m_window->showMessageCallback = [&]() {
            messageShown = true;
        };

        m_window->onSettingsChanged();

        QVERIFY(messageShown);

        QCOMPARE(m_window->m_intMap.size(), 2);
        QCOMPARE(m_window->m_intMap[0].size(), 2);
        QCOMPARE(m_window->m_intMap[0][0], 2);
        QCOMPARE(m_window->m_intMap[0][1], 4);
        QCOMPARE(m_window->m_intMap[1][0], 1);
        QCOMPARE(m_window->m_intMap[1][1], 2);
        QCOMPARE(Constants::imax, 2);
        QCOMPARE(Constants::jmax, 2);

        m_window->showMessageCallback = nullptr;
        m_window->isGameStart = false;
    }

    void testOnSettingsChanged_MapResize_Larger() {
        QVector<QVector<int>> board = {
            {2, 4, 0, 0}, {8, 16, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        setupBoard(board);

        m_window->isGameStart = false;

        QSettings settings("wwanngg", "2048advanced");
        settings.setValue("Personalize/mapX", "6");
        settings.setValue("Personalize/mapY", "6");
        settings.setValue("Game/ColorTheme", "system");
        settings.setValue("Game/GameTheme", "number");

        m_window->onSettingsChanged();

        QCOMPARE(m_window->m_intMap.size(), 6);
        QCOMPARE(m_window->m_intMap[0].size(), 6);
        QCOMPARE(m_window->m_intMap[0][0], 2);
        QCOMPARE(m_window->m_intMap[1][1], 16);
        QCOMPARE(m_window->m_intMap[4][4], 0);
    }

    void testOnSettingsChanged_Duration() {
        QSettings settings("wwanngg", "2048advanced");
        settings.setValue("Personalize/animationDuration", "5000ms");
        settings.setValue("Game/ColorTheme", "system");
        settings.setValue("Game/GameTheme", "number");

        int prevDuration = Constants::duration;
        m_window->onSettingsChanged();

        QCOMPARE(Constants::duration, 5000);
        Constants::duration = prevDuration;
    }

    void testOnSettingsChanged_EasingCurve() {
        QSettings settings("wwanngg", "2048advanced");
        settings.setValue("Personalize/animationEasingCurve", "OutBounce");
        settings.setValue("Game/ColorTheme", "system");
        settings.setValue("Game/GameTheme", "number");

        auto prevCurve = Constants::animEasingCurve;
        m_window->onSettingsChanged();

        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::OutBounce));
        Constants::animEasingCurve = prevCurve;
    }

    void testOnSettingsChanged_DefaultEasingCurve() {
        Constants::animEasingCurve = QEasingCurve::OutBounce;

        QSettings settings("wwanngg", "2048advanced");
        settings.setValue("Game/ColorTheme", "system");
        settings.setValue("Game/GameTheme", "number");

        m_window->onSettingsChanged();

        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::InOutQuad));
    }

    void testOnSettingsChanged_AllEasingCurve() {
        QSettings settings("wwanngg", "2048advanced");

        settings.setValue("Personalize/animationEasingCurve", "Linear");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::Linear));

        settings.setValue("Personalize/animationEasingCurve", "InQuad");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::InQuad));

        settings.setValue("Personalize/animationEasingCurve", "OutQuad");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::OutQuad));

        settings.setValue("Personalize/animationEasingCurve", "InOutQuad");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::InOutQuad));

        settings.setValue("Personalize/animationEasingCurve", "OutInQuad");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::OutInQuad));

        settings.setValue("Personalize/animationEasingCurve", "InCubic");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::InCubic));

        settings.setValue("Personalize/animationEasingCurve", "OutCubic");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::OutCubic));

        settings.setValue("Personalize/animationEasingCurve", "InOutCubic");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::InOutCubic));

        settings.setValue("Personalize/animationEasingCurve", "OutInCubic");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::OutInCubic));

        settings.setValue("Personalize/animationEasingCurve", "InQuart");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::InQuart));

        settings.setValue("Personalize/animationEasingCurve", "OutQuart");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::OutQuart));

        settings.setValue("Personalize/animationEasingCurve", "InOutQuart");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::InOutQuart));

        settings.setValue("Personalize/animationEasingCurve", "OutInQuart");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::OutInQuart));

        settings.setValue("Personalize/animationEasingCurve", "InQuint");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::InQuint));

        settings.setValue("Personalize/animationEasingCurve", "OutQuint");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::OutQuint));

        settings.setValue("Personalize/animationEasingCurve", "InOutQuint");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::InOutQuint));

        settings.setValue("Personalize/animationEasingCurve", "OutInQuint");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::OutInQuint));

        settings.setValue("Personalize/animationEasingCurve", "InSine");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::InSine));

        settings.setValue("Personalize/animationEasingCurve", "OutSine");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::OutSine));

        settings.setValue("Personalize/animationEasingCurve", "InOutSine");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::InOutSine));

        settings.setValue("Personalize/animationEasingCurve", "OutInSine");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::OutInSine));

        settings.setValue("Personalize/animationEasingCurve", "InExpo");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::InExpo));

        settings.setValue("Personalize/animationEasingCurve", "OutExpo");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::OutExpo));

        settings.setValue("Personalize/animationEasingCurve", "InOutExpo");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::InOutExpo));

        settings.setValue("Personalize/animationEasingCurve", "OutInExpo");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::OutInExpo));

        settings.setValue("Personalize/animationEasingCurve", "InCirc");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::InCirc));

        settings.setValue("Personalize/animationEasingCurve", "OutCirc");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::OutCirc));

        settings.setValue("Personalize/animationEasingCurve", "InOutCirc");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::InOutCirc));

        settings.setValue("Personalize/animationEasingCurve", "OutInCirc");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::OutInCirc));

        settings.setValue("Personalize/animationEasingCurve", "InElastic");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::InElastic));

        settings.setValue("Personalize/animationEasingCurve", "OutElastic");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::OutElastic));

        settings.setValue("Personalize/animationEasingCurve", "InBack");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::InBack));

        settings.setValue("Personalize/animationEasingCurve", "OutBack");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::OutBack));

        settings.setValue("Personalize/animationEasingCurve", "InBounce");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::InBounce));

        settings.setValue("Personalize/animationEasingCurve", "OutBounce");
        m_window->onSettingsChanged();
        QCOMPARE(static_cast<int>(Constants::animEasingCurve.type()),
                 static_cast<int>(QEasingCurve::OutBounce));
    }

    void testCreateMenuBar_HasMenuBar() {
        QVERIFY(m_window->menuBar);
    }

    void testCreateMenuBar_HasSettingsAction() {
        QMenuBar* bar = m_window->menuBar;
        bool found = false;
        for (auto* action : bar->actions()) {
            if (action->menu()) {
                for (auto* menuAction : action->menu()->actions()) {
                    if (menuAction->text().contains("Settings",
                                                    Qt::CaseInsensitive)) {
                        found = true;
                        break;
                    }
                }
            }
            if (found)
                break;
        }
        QVERIFY(found);
    }

    void testOnSettings_OpensDialog() {
        QTimer::singleShot(0, [&]() {
            QWidget* activeDialog = QApplication::activeModalWidget();
            if (activeDialog) {
                activeDialog->close();
            }
        });

        m_window->onSettings();
        QVERIFY(m_window->m_settingsDialog != nullptr);

        QTimer::singleShot(0, [&]() {
            QWidget* activeDialog = QApplication::activeModalWidget();
            if (activeDialog) {
                activeDialog->close();
            }
        });

        m_window->onSettings();
    }

    void testOnExit_Yes_ClosesWindow() {
        QTimer::singleShot(0, [&]() {
            QWidget* activeDialog = QApplication::activeModalWidget();
            if (QMessageBox* msgBox = qobject_cast<QMessageBox*>(activeDialog)) {
                if (auto btn = msgBox->button(QMessageBox::Yes)) {
                    btn->click();
                } else {
                    msgBox->close();
                }
            } else if (activeDialog) {
                activeDialog->close();
            }
        });

        m_window->onExit();
    }

    void testOnExit_No_KeepsWindowOpen() {
        QTimer::singleShot(0, [&]() {
            QWidget* activeDialog = QApplication::activeModalWidget();
            if (QMessageBox* msgBox = qobject_cast<QMessageBox*>(activeDialog)) {
                if (auto btn = msgBox->button(QMessageBox::No)) {
                    btn->click();
                } else {
                    msgBox->close();
                }
            } else if (activeDialog) {
                activeDialog->close();
            }
        });

        m_window->onExit();
    }
};

QTEST_MAIN(TestMainWindow)
#include "test_mainwindow.moc"
