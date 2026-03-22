#include <QRandomGenerator>
#include <QSequentialAnimationGroup>
#include <QEasingCurve>
#include <QLabel>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QPointer>


#include <functional>

#include "../include/constants.h"
#include "../include/mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_intMap(4, QVector<int>(4))
    , m_labelMap(4, QVector<QLabel*>(4))
    , m_score(0)
    , m_animationRunning(false)
    , m_settingsDialog(nullptr)
    , m_gametype(GameType::number) {
    createMenuBar();
    loadSettings();
    setupUI();
    setupAnimations();
    
    qApp->installEventFilter(m_keyboardHandler);
}

MainWindow::~MainWindow() {
    auto animations = findChildren<QAbstractAnimation*>();
    for (auto anim : animations) {
        anim->stop();
        anim->deleteLater();
    }

    if (m_keyboardHandler) {
        m_keyboardHandler->clearAllAnimations();
        qApp->removeEventFilter(m_keyboardHandler);
    }

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (m_labelMap[i][j]) {
                m_labelMap[i][j]->deleteLater();
                m_labelMap[i][j] = nullptr;
            }
        }
    }

    if (m_keyboardHandler) {
        m_keyboardHandler->deleteLater();
        m_keyboardHandler = nullptr;
    }
}

void MainWindow::setupUI() {
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    QParallelAnimationGroup* group = new QParallelAnimationGroup(m_centralWidget);
    group->addAnimation(createRandom());
    group->addAnimation(createRandom());
    
    connect(group, &QParallelAnimationGroup::finished, group, &QObject::deleteLater);
    group->start();
    
    setWindowTitle("2048 Score: 0");
    setFixedSize(Constants::windowSize, Constants::windowSize + menuBar->height());
}

void MainWindow::setupAnimations() {
    m_keyboardHandler = new KeyboardHandler(this);
    
    m_keyboardHandler->registerKeyAnimation(Qt::Key_A, [this]() {
        if (m_animationRunning) return (QSequentialAnimationGroup*)nullptr;
        return createAnimationForKeyA();
    });    

    m_keyboardHandler->registerKeyAnimation(Qt::Key_D, [this]() {
        if (m_animationRunning) return (QSequentialAnimationGroup*)nullptr;
        return createAnimationForKeyD();
    });

    m_keyboardHandler->registerKeyAnimation(Qt::Key_W, [this]() {
        if (m_animationRunning) return (QSequentialAnimationGroup*)nullptr;
        return createAnimationForKeyW();
    });

    m_keyboardHandler->registerKeyAnimation(Qt::Key_S, [this]() {
        if (m_animationRunning) return (QSequentialAnimationGroup*)nullptr;
        return createAnimationForKeyS();
    });
}

QPropertyAnimation* MainWindow::createMoveAnimation(QWidget *widget, const QRect& offset, int duration) {
    if (!widget) return nullptr;
    
    QPropertyAnimation *anim = new QPropertyAnimation(widget, "geometry");
    QRect startRect = widget->geometry();
    QRect endRect = offset;
    
    anim->setDuration(duration);
    anim->setStartValue(startRect);
    anim->setEndValue(endRect);
    anim->setEasingCurve(QEasingCurve::InOutQuad);
    
    return anim;
}

QPropertyAnimation* MainWindow::createDisappearAnimation(QWidget *widget, const QRect& startRect, int duration) {
    if (!widget) return nullptr;
    
    QPropertyAnimation *anim = new QPropertyAnimation(widget, "geometry");
    QRect endRect(
        startRect.center().x(),
        startRect.center().y(),
        0,
        0
    );
    anim->setStartValue(startRect);
    anim->setEndValue(endRect);
    anim->setEasingCurve(QEasingCurve::InOutQuad);
    anim->setDuration(duration);
    
    QPointer<QWidget> safeWidget = widget;
    connect(anim, &QPropertyAnimation::finished, [=]() {
        if (safeWidget) {
            safeWidget->hide();
            safeWidget->deleteLater();
        }
    });
    
    return anim;
}

QSequentialAnimationGroup* MainWindow::createMoveDisappearAnimation(QWidget *widget, const QRect &offset, int duration) {
    if (!widget) return nullptr;
    
    QSequentialAnimationGroup* group = new QSequentialAnimationGroup(this);
    
    QPropertyAnimation* moveAnim = createMoveAnimation(widget, offset, duration / 2);
    QPropertyAnimation* disappearAnim = createDisappearAnimation(widget, offset, duration / 2);
    
    if (moveAnim) group->addAnimation(moveAnim);
    if (disappearAnim) group->addAnimation(disappearAnim);
    
    return group;
}

QSequentialAnimationGroup* MainWindow::createAnimationForKeyA() {
    if (m_animationRunning) {
        return new QSequentialAnimationGroup(this);
    }
    
    m_animationRunning = true;
    QParallelAnimationGroup *group = new QParallelAnimationGroup(this);

    QVector<QVector<bool>> merged(4, QVector<bool>(4, false));
    
    struct MoveData {
        QPointer<QLabel> label;
        QRect targetRect;
        bool shouldDisappear;
    };
    QVector<MoveData> moves;
    
    for (std::size_t i = 0; i < 4; ++i) {
        for (std::size_t j = 1; j < 4; ++j) {
            if (m_intMap[i][j] == 0 || !m_labelMap[i][j]) {
                continue;
            }
            
            QPointer<QLabel> currentLabel = m_labelMap[i][j];
            int currentValue = m_intMap[i][j];
            std::size_t target = j;
            bool shouldMerge = false;

            for (std::size_t k = j; k > 0; --k) {
                if (m_intMap[i][k-1] == 0) {
                    target = k-1;
                } else if (m_intMap[i][k-1] == currentValue && !merged[i][k-1]) {
                    target = k-1;
                    shouldMerge = true;
                    break;
                } else {
                    break;
                }
            }
            
            if (target != j || shouldMerge) {
                if (shouldMerge) {

                    m_intMap[i][j] = 0;
                    moves.append({currentLabel, 
                        QRect(static_cast<int>(target * Constants::labelSize), 
                             static_cast<int>(i * Constants::labelSize), 
                             Constants::labelSize, Constants::labelSize), 
                        true});
                    
                    m_intMap[i][target] *= 2;
                    if (m_labelMap[i][target]) {
                        m_labelMap[i][target]->setText(QString::number(m_intMap[i][target]));
                        int styleIndex = m_intMap[i][target] >= Constants::maxUsedNum ? 
                            Constants::maxUsedNum : m_intMap[i][target];
                        if (m_gametype == GameType::number) {
                            m_labelMap[i][target]->setStyleSheet(Constants::styles[styleIndex]);
                        } else if (m_gametype == GameType::caixukun) {
                            m_labelMap[i][target]->setStyleSheet(Constants::caixukunStyles[styleIndex]);
                        }
                    }
                    m_score += m_intMap[i][target];
                    merged[i][target] = true;
                } else {

                    std::swap(m_intMap[i][target], m_intMap[i][j]);
                    moves.append({currentLabel,
                        QRect(static_cast<int>(target * Constants::labelSize), 
                             static_cast<int>(i * Constants::labelSize), 
                             Constants::labelSize, Constants::labelSize),
                        false});
                    m_labelMap[i][target] = currentLabel;
                }
                m_labelMap[i][j] = nullptr;
            }
        }
    }

    for (const auto& move : moves) {
        if (move.label.isNull()) continue;
        
        if (move.shouldDisappear) {
            QSequentialAnimationGroup* seqAnim = createMoveDisappearAnimation(
                move.label, move.targetRect, Constants::duration);
            if (seqAnim && seqAnim->animationCount() > 0) {
                group->addAnimation(seqAnim);
            } else {
                delete seqAnim;
            }
        } else {
            QPropertyAnimation* anim = createMoveAnimation(
                move.label, move.targetRect, Constants::duration);
            if (anim) {
                group->addAnimation(anim);
            }
        }
    }

    setWindowTitle("2048 Score: " + QString::number(m_score));

    QSequentialAnimationGroup* seqGroup = new QSequentialAnimationGroup(this);
    seqGroup->addAnimation(group);

    if (group->animationCount() > 0) {
        seqGroup->addAnimation(createRandom());
    }

    connect(seqGroup, &QSequentialAnimationGroup::finished, [this, seqGroup]() {
        m_animationRunning = false;
        seqGroup->deleteLater();
    });
    
    if (seqGroup->animationCount() == 0) {
        m_animationRunning = false;
    }

    return seqGroup;
}

QSequentialAnimationGroup* MainWindow::createAnimationForKeyD() {
    if (m_animationRunning) {
        return new QSequentialAnimationGroup(this);
    }
    
    m_animationRunning = true;
    QParallelAnimationGroup *group = new QParallelAnimationGroup(this);

    QVector<QVector<bool>> merged(4, QVector<bool>(4, false));
    
    struct MoveData {
        QPointer<QLabel> label;
        QRect targetRect;
        bool shouldDisappear;
    };
    QVector<MoveData> moves;
    
    for (std::size_t i = 0; i < 4; ++i) {
        for (int j = 2; j >= 0; --j) {
            std::size_t col = static_cast<std::size_t>(j);
            if (m_intMap[i][col] == 0 || !m_labelMap[i][col]) {
                continue;
            }
            
            QPointer<QLabel> currentLabel = m_labelMap[i][col];
            int currentValue = m_intMap[i][col];
            std::size_t target = col;
            bool shouldMerge = false;
            
            for (std::size_t k = col; k < 3; ++k) {
                if (m_intMap[i][k+1] == 0) {
                    target = k+1;
                } else if (m_intMap[i][k+1] == currentValue && !merged[i][k+1]) {
                    target = k+1;
                    shouldMerge = true;
                    break;
                } else {
                    break;
                }
            }
            
            if (target != col || shouldMerge) {
                if (shouldMerge) {

                    m_intMap[i][col] = 0;
                    moves.append({currentLabel, 
                        QRect(static_cast<int>(target * Constants::labelSize), 
                             static_cast<int>(i * Constants::labelSize), 
                             Constants::labelSize, Constants::labelSize), 
                        true});
                    
                    m_intMap[i][target] *= 2;
                    if (m_labelMap[i][target]) {
                        m_labelMap[i][target]->setText(QString::number(m_intMap[i][target]));
                        int styleIndex = m_intMap[i][target] >= Constants::maxUsedNum ? 
                            Constants::maxUsedNum : m_intMap[i][target];
                        if (m_gametype == GameType::number) {
                            m_labelMap[i][target]->setStyleSheet(Constants::styles[styleIndex]);
                        } else if (m_gametype == GameType::caixukun) {
                            m_labelMap[i][target]->setStyleSheet(Constants::caixukunStyles[styleIndex]);
                        }
                        
                    }
                    m_score += m_intMap[i][target];
                    merged[i][target] = true;
                } else {
                    std::swap(m_intMap[i][target], m_intMap[i][col]);
                    moves.append({currentLabel,
                        QRect(static_cast<int>(target * Constants::labelSize), 
                             static_cast<int>(i * Constants::labelSize), 
                             Constants::labelSize, Constants::labelSize),
                        false});
                    m_labelMap[i][target] = currentLabel;
                }
                m_labelMap[i][col] = nullptr;
            }
        }
    }

    for (const auto& move : moves) {
        if (move.label.isNull()) continue;
        
        if (move.shouldDisappear) {
            QSequentialAnimationGroup* seqAnim = createMoveDisappearAnimation(
                move.label, move.targetRect, Constants::duration);
            if (seqAnim && seqAnim->animationCount() > 0) {
                group->addAnimation(seqAnim);
            } else {
                delete seqAnim;
            }
        } else {
            QPropertyAnimation* anim = createMoveAnimation(
                move.label, move.targetRect, Constants::duration);
            if (anim) {
                group->addAnimation(anim);
            }
        }
    }

    setWindowTitle("2048 Score: " + QString::number(m_score));

    QSequentialAnimationGroup* seqGroup = new QSequentialAnimationGroup(this);
    seqGroup->addAnimation(group);

    if (group->animationCount() > 0) {
        seqGroup->addAnimation(createRandom());
    }

    connect(seqGroup, &QSequentialAnimationGroup::finished, [this, seqGroup]() {
        m_animationRunning = false;
        seqGroup->deleteLater();
    });
    
    if (seqGroup->animationCount() == 0) {
        m_animationRunning = false;
    }

    return seqGroup;
}

QSequentialAnimationGroup* MainWindow::createAnimationForKeyW() {
    if (m_animationRunning) {
        return new QSequentialAnimationGroup(this);
    }
    
    m_animationRunning = true;
    QParallelAnimationGroup *group = new QParallelAnimationGroup(this);

    QVector<QVector<bool>> merged(4, QVector<bool>(4, false));
    
    struct MoveData {
        QPointer<QLabel> label;
        QRect targetRect;
        bool shouldDisappear;
    };
    QVector<MoveData> moves;
    
    for (std::size_t j = 0; j < 4; ++j) {
        for (std::size_t i = 1; i < 4; ++i) {
            if (m_intMap[i][j] == 0 || !m_labelMap[i][j]) {
                continue;
            }
            
            QPointer<QLabel> currentLabel = m_labelMap[i][j];
            int currentValue = m_intMap[i][j];
            std::size_t target = i;
            bool shouldMerge = false;
            
            for (std::size_t k = i; k > 0; --k) {
                if (m_intMap[k-1][j] == 0) {
                    target = k-1;
                } else if (m_intMap[k-1][j] == currentValue && !merged[k-1][j]) {
                    target = k-1;
                    shouldMerge = true;
                    break;
                } else {
                    break;
                }
            }
            
            if (target != i || shouldMerge) {
                if (shouldMerge) {

                    m_intMap[i][j] = 0;
                    moves.append({currentLabel, 
                        QRect(static_cast<int>(j * Constants::labelSize), 
                             static_cast<int>(target * Constants::labelSize), 
                             Constants::labelSize, Constants::labelSize), 
                        true});
                    
                    m_intMap[target][j] *= 2;
                    if (m_labelMap[target][j]) {
                        m_labelMap[target][j]->setText(QString::number(m_intMap[target][j]));
                        int styleIndex = m_intMap[target][j] >= Constants::maxUsedNum ? 
                            Constants::maxUsedNum : m_intMap[target][j];
                        if (m_gametype == GameType::number) {
                            m_labelMap[target][j]->setStyleSheet(Constants::styles[styleIndex]);
                        } else if (m_gametype == GameType::caixukun) {
                            m_labelMap[target][j]->setStyleSheet(Constants::caixukunStyles[styleIndex]);
                        }
                        
                    }
                    m_score += m_intMap[target][j];
                    merged[target][j] = true;
                } else {
                    std::swap(m_intMap[target][j], m_intMap[i][j]);
                    moves.append({currentLabel,
                        QRect(static_cast<int>(j * Constants::labelSize), 
                             static_cast<int>(target * Constants::labelSize), 
                             Constants::labelSize, Constants::labelSize),
                        false});
                    m_labelMap[target][j] = currentLabel;
                }
                m_labelMap[i][j] = nullptr;
            }
        }
    }

    for (const auto& move : moves) {
        if (move.label.isNull()) continue;
        
        if (move.shouldDisappear) {
            QSequentialAnimationGroup* seqAnim = createMoveDisappearAnimation(
                move.label, move.targetRect, Constants::duration);
            if (seqAnim && seqAnim->animationCount() > 0) {
                group->addAnimation(seqAnim);
            } else {
                delete seqAnim;
            }
        } else {
            QPropertyAnimation* anim = createMoveAnimation(
                move.label, move.targetRect, Constants::duration);
            if (anim) {
                group->addAnimation(anim);
            }
        }
    }

    setWindowTitle("2048 Score: " + QString::number(m_score));

    QSequentialAnimationGroup* seqGroup = new QSequentialAnimationGroup(this);
    seqGroup->addAnimation(group);

    if (group->animationCount() > 0) {
        seqGroup->addAnimation(createRandom());
    }

    connect(seqGroup, &QSequentialAnimationGroup::finished, [this, seqGroup]() {
        m_animationRunning = false;
        seqGroup->deleteLater();
    });
    
    if (seqGroup->animationCount() == 0) {
        m_animationRunning = false;
    }

    return seqGroup;
}

QSequentialAnimationGroup* MainWindow::createAnimationForKeyS() {
    if (m_animationRunning) {
        return new QSequentialAnimationGroup(this);
    }
    
    m_animationRunning = true;
    QParallelAnimationGroup *group = new QParallelAnimationGroup(this);

    QVector<QVector<bool>> merged(4, QVector<bool>(4, false));
    
    struct MoveData {
        QPointer<QLabel> label;
        QRect targetRect;
        bool shouldDisappear;
    };
    QVector<MoveData> moves;
    
    for (std::size_t j = 0; j < 4; ++j) {
        for (int i = 2; i >= 0; --i) {
            std::size_t row = static_cast<std::size_t>(i);
            if (m_intMap[row][j] == 0 || !m_labelMap[row][j]) {
                continue;
            }
            
            QPointer<QLabel> currentLabel = m_labelMap[row][j];
            int currentValue = m_intMap[row][j];
            std::size_t target = row;
            bool shouldMerge = false;
            
            for (std::size_t k = row; k < 3; ++k) {
                if (m_intMap[k+1][j] == 0) {
                    target = k+1;
                } else if (m_intMap[k+1][j] == currentValue && !merged[k+1][j]) {
                    target = k+1;
                    shouldMerge = true;
                    break;
                } else {
                    break;
                }
            }
            
            if (target != row || shouldMerge) {
                if (shouldMerge) {

                    m_intMap[row][j] = 0;
                    moves.append({currentLabel, 
                        QRect(static_cast<int>(j * Constants::labelSize), 
                             static_cast<int>(target * Constants::labelSize), 
                             Constants::labelSize, Constants::labelSize), 
                        true});
                    
                    m_intMap[target][j] *= 2;
                    if (m_labelMap[target][j]) {
                        m_labelMap[target][j]->setText(QString::number(m_intMap[target][j]));
                        int styleIndex = m_intMap[target][j] >= Constants::maxUsedNum ? 
                            Constants::maxUsedNum : m_intMap[target][j];
                        if (m_gametype == GameType::number) {
                            m_labelMap[target][j]->setStyleSheet(Constants::styles[styleIndex]);
                        } else if (m_gametype == GameType::caixukun) {
                            m_labelMap[target][j]->setStyleSheet(Constants::caixukunStyles[styleIndex]);
                        }
                        
                    }
                    m_score += m_intMap[target][j];
                    merged[target][j] = true;
                } else {

                    std::swap(m_intMap[target][j], m_intMap[row][j]);
                    moves.append({currentLabel,
                        QRect(static_cast<int>(j * Constants::labelSize), 
                             static_cast<int>(target * Constants::labelSize), 
                             Constants::labelSize, Constants::labelSize),
                        false});
                    m_labelMap[target][j] = currentLabel;
                }
                m_labelMap[row][j] = nullptr;
            }
        }
    }

    for (const auto& move : moves) {
        if (move.label.isNull()) continue;
        
        if (move.shouldDisappear) {
            QSequentialAnimationGroup* seqAnim = createMoveDisappearAnimation(
                move.label, move.targetRect, Constants::duration);
            if (seqAnim && seqAnim->animationCount() > 0) {
                group->addAnimation(seqAnim);
            } else {
                delete seqAnim;
            }
        } else {
            QPropertyAnimation* anim = createMoveAnimation(
                move.label, move.targetRect, Constants::duration);
            if (anim) {
                group->addAnimation(anim);
            }
        }
    }

    setWindowTitle("2048 Score: " + QString::number(m_score));

    QSequentialAnimationGroup* seqGroup = new QSequentialAnimationGroup(this);
    seqGroup->addAnimation(group);

    if (group->animationCount() > 0) {
        seqGroup->addAnimation(createRandom());
    }

    connect(seqGroup, &QSequentialAnimationGroup::finished, [this, seqGroup]() {
        m_animationRunning = false;
        seqGroup->deleteLater();
    });
    
    if (seqGroup->animationCount() == 0) {
        m_animationRunning = false;
    }

    return seqGroup;
}

QParallelAnimationGroup* MainWindow::createRandom() {
    QVector<QPair<std::size_t, std::size_t>> emptyLocations;
    for (std::size_t i = 0; i < 4; ++i) {
        for (std::size_t j = 0; j < 4; ++j) {
            if (m_intMap[i][j] == 0) {
                emptyLocations.push_back({i, j});
            }
        }
    }

    if (emptyLocations.isEmpty()) {
        return new QParallelAnimationGroup(this);
    }

    auto pair = emptyLocations[QRandomGenerator::global()->bounded(emptyLocations.size())];
    int num = QRandomGenerator::global()->bounded(2) == 0 ? 2 : 4;

    QLabel* label = new QLabel(QString::number(num), m_centralWidget);
    if (m_gametype == GameType::number) {
        label->setStyleSheet(Constants::styles[num]);
        label->setAlignment(Qt::AlignCenter);
    } else if (m_gametype == GameType::caixukun) {
        label->setStyleSheet(Constants::caixukunStyles[num]);
        label->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    }
    
    
    label->setWindowOpacity(0.0);
    
    QParallelAnimationGroup *group = new QParallelAnimationGroup(this);

    QRect finalRect(
        static_cast<int>(pair.second * Constants::labelSize), 
        static_cast<int>(pair.first * Constants::labelSize), 
        Constants::labelSize, 
        Constants::labelSize);
    QPoint centerPoint = finalRect.center();
    QRect startRect = QRect(centerPoint.x(), centerPoint.y(), 1, 1);

    label->setGeometry(startRect);

    QPropertyAnimation *geoAnim = new QPropertyAnimation(label, "geometry");
    geoAnim->setDuration(Constants::duration / 2);
    geoAnim->setStartValue(startRect);
    geoAnim->setEndValue(finalRect);
    geoAnim->setEasingCurve(QEasingCurve::OutBack);
        
    QPropertyAnimation *opacityAnim = new QPropertyAnimation(label, "windowOpacity");
    opacityAnim->setDuration(Constants::duration / 2);
    opacityAnim->setStartValue(0.0);
    opacityAnim->setEndValue(1.0);
    opacityAnim->setEasingCurve(QEasingCurve::Linear);
        
    group->addAnimation(geoAnim);
    group->addAnimation(opacityAnim);

    label->show();

    m_intMap[pair.first][pair.second] = num;
    m_labelMap[pair.first][pair.second] = label;

    connect(group, &QParallelAnimationGroup::finished, group, &QObject::deleteLater);

    return group;
}