# 高级语言程序设计大作业实验报告

## 作业题目

2048：增强与创新

## 开发环境

- 编辑器：Visual Studio Code
- 框架：Qt 6
- 构建工具：CMake
- 语言：C++17

## 课题要求

- 面向对象设计
- 单元测试
- 模型与视图分离
- 功能验证

## 项目概述

这是一个在经典 2048 游戏基础上的增强版本。除了传统的数字模式外，还加入了"蔡徐坤"和"化学元素"两种主题模式，并支持自定义棋盘大小、动画速度、动画曲线等个性化设置，让游戏不那么单调。

按 **W/A/S/D** 键来移动方块，相同数字的方块碰到一起会合并成更大的数字。目标是合出 2048（数字模式 / 蔡徐坤模式）或 Xe136（化学模式）。

## 已实现的功能

- 经典 2048 玩法（数字模式）
- 三种游戏主题：数字模式、蔡徐坤模式、化学元素模式
- 流畅的方块移动和合并动画
- 分数统计
- 背景音乐和音效（可在设置中开关）
- 可自定义棋盘大小（2×2 到 6×6）
- 可自定义动画时长（10 ms 到 10000 ms）
- 可自定义动画缓动曲线（35 种可选）
- 明暗主题切换
- 设置自动保存，下次启动自动恢复
- 单元测试覆盖核心逻辑

## 项目结构

```
advanced-2048/
├── include/                # 头文件
│   ├── mainwindow.h        # 主窗口
│   ├── KeyboardHandler.h   # 按键处理
│   ├── settingsdialog.h    # 设置对话框
│   └── constants.h         # 全局常量与主题样式
├── src/                    # 源文件
│   ├── main.cpp            # 程序入口
│   ├── mainwindow.cpp      # 主窗口实现（游戏逻辑与动画）
│   ├── KeyboardHandler.cpp # 按键处理实现
│   ├── settingsdialog.cpp  # 设置对话框实现
│   └── windowsetting.cpp   # 菜单栏、音乐、设置加载等
├── tests/                  # 单元测试
│   ├── test_mainwindow.cpp       # 主窗口测试
│   ├── test_keyboardhandler.cpp  # 按键处理测试
│   ├── test_settingsdialog.cpp   # 设置对话框测试
│   └── CMakeLists.txt
├── resources/              # 图片、音效、音乐资源
├── CMakeLists.txt
└── README.md / README.zh.md
```

## 主要流程

### 整体启动流程

程序启动后，`MainWindow` 的构造函数依次完成以下初始化：

1. **初始化音乐播放器** —— `initMusicPlayer()` 加载背景音乐并设为循环播放。
2. **创建菜单栏** —— `createMenuBar()` 添加"设置"和"退出"两个菜单项。
3. **加载设置** —— `loadSettings()` 调用 `onSettingsChanged()` 恢复上次保存的偏好设置（主题、棋盘大小、动画参数等），完成后将 `isGameStart` 置为 `true`。
4. **搭建游戏界面** —— `setupUI()` 创建棋盘，并随机在两个位置生成初始方块。
5. **注册按键动画** —— `setupAnimations()` 创建 `KeyboardHandler`，把 W/A/S/D 四个按键分别绑定到对应的动画函数上，然后将 `KeyboardHandler` 安装为全局事件过滤器。

### 核心数据结构

`MainWindow` 中维护了三个二维数组来描述棋盘状态：

- **`m_intMap`**：`int` 类型，默认值为 `0` 表示空格子；`2`、`4`、`8` 等表示该位置的数字。
- **`m_labelMap`**：`QLabel*` 类型，默认值为 `nullptr` 表示空格子；非空时指向该位置显示的方块控件。
- **`m_surviveCount`**：`int` 类型，默认值为 `-1` 表示空格子；非负整数表示该方块自生成以来经历的移动轮数。（化学模式下某些方块有存活时间限制，达到上限会自动消失）

其他关键成员：

- **`m_score`**：当前分数，每次合并时累加。
- **`m_animationRunning`**：当前是否有动画正在运行，用于防止动画冲突。
- **`m_gametype`**：`GameType` 枚举，取值 `number`（数字模式）、`caixukun`（蔡徐坤模式）或 `chemistry`（化学模式）。
- **`isGameStart`**：游戏是否已正式开始，用于区分初始化和设置变更时的行为。

### 按键处理（`KeyboardHandler`）

`KeyboardHandler` 是整个游戏按键响应的中枢，内部设计如下：

**`KeyAnimationInfo` 结构体** —— 每注册一个按键，就会对应一个 `KeyAnimationInfo`，里面存了三个东西：

- `creator`：一个函数指针，调用它会返回一个 `QSequentialAnimationGroup*`（即该按键对应的动画组）。
- `isWaiting`：标记该按键是否在排队等待执行。
- `currentAnimation`：指向当前正在执行的动画。

**注册按键** —— `KeyboardHandler::registerKeyAnimation(key, animationCreator)` 在 `MainWindow::setupAnimations()` 中被调用，把 W/A/S/D 四个按键和各自的动画创建函数绑定在一起，存入 `m_keyAnimations`（一个 `QMap<int, KeyAnimationInfo>`）。

**按键拦截** —— `KeyboardHandler::eventFilter` 重写了 Qt 的事件过滤函数。当用户按下按键时：
- 如果按键在 `m_keyAnimations` 中有注册：
  - 当前没有动画在运行则直接调用 `startAnimation()` 开始动画。
  - 当前有动画在运行则把该按键的 `isWaiting` 设为 `true`（排队等待）。
- 如果按键没注册则交给默认的事件处理。

**动画启动** —— `startAnimation(info)` 调用 `creator` 拿到动画组，连接 `finished` 信号到 `onAnimationFinished`，然后启动动画。根据设置决定是否播放音效。

**动画结束处理** —— `onAnimationFinished()` 先清理刚结束的动画，然后检查 `m_keyAnimations` 中是否有排队等待的按键，有的话按顺序执行。如果没有任何等待的动画，把 `m_isAnyAnimationRunning` 设为 `false`。

简单说，`KeyboardHandler` 做的事情就是：**注册按键、拦截按键、执行动画、排队管理、按顺序逐个播放**。

### 移动与合并逻辑

游戏支持上下左右四个方向的移动。下面以**向左移动**为例说明合并规则，其他方向同理。

基本思路：从第二列开始（第一列已经是最左边了），每个方块向左找最远能到达的位置：
- 如果一路上都是空格子，就滑到最左边。
- 如果碰到一个数字相同的格子（且该格子本轮还没被合并过），就合并过去，数字翻倍，原位置消失。
- 如果碰到数字不同且不为零的格子，就停在这个格子的右边。

为了防止"连环合并"（比如 `2, 2, 4, 8` 向左不应该变成 `16, 0, 0, 0`），用一个 `merged` 二维布尔数组记录每个位置本轮是否已经被合并过。被合并过的位置不会再参与当轮的合并。

基于以上规则，提供了几个辅助函数：

- **`MainWindow::validate(direction)`** —— 判断某个方向是否可以移动（不改数据，只检查）。用于：移动前判断是否需要执行动画，以及失败条件的检查。
- **`MainWindow::isEmpty()`** —— 判断棋盘是否全空。
- **`MainWindow::addSurviveCount()`** —— 每次移动后，给所有存在方块的位置的存活轮数 +1。

### 动画系统

为了减少重复代码，设计了三个基础的动画工具函数：

```cpp
QPropertyAnimation* createMoveAnimation(QWidget* widget, const QRect& offset, int duration);
QPropertyAnimation* createDisappearAnimation(QWidget* widget, const QRect& startRect, int duration);
QSequentialAnimationGroup* createMoveDisappearAnimation(QWidget* widget, const QRect& offset, int duration);
```

分别实现：**移动到目标位置**、**原地缩小消失**、**先移动再消失**。

四个方向的动画分别由以下函数生成：

```cpp
QSequentialAnimationGroup* createAnimationForKeyA();  // 向左
QSequentialAnimationGroup* createAnimationForKeyD();  // 向右
QSequentialAnimationGroup* createAnimationForKeyW();  // 向上
QSequentialAnimationGroup* createAnimationForKeyS();  // 向下
```

以向左移动 (`createAnimationForKeyA`) 为例，流程如下：

1. 连接动画结束信号到一个 lambda，负责将 `m_animationRunning` 置为 `false`、清理动画对象，并检查四个方向是否都无法移动（如果是则判定失败）。
2. 如果当前已有动画在运行（`m_animationRunning == true`），返回空动画组。
3. 如果棋盘为空（`isEmpty()` 为 `true`），只添加一个随机生成方块的动画然后返回。
4. 如果此方向不能移动，添加 100 ms 的延迟然后返回。

   > 为什么加延迟？因为如果不加，动画会瞬间完成。`KeyboardHandler::startAnimation` 中 `anim->start()` 对空动画立即返回，`finished` 信号马上触发，导致 `m_isAnyAnimationRunning` 被设为 `false`，紧接着后面的 `m_isAnyAnimationRunning = true` 才执行，结果就是状态混乱、界面卡死。加入延迟可以避免这个问题。

5. 调用 `addSurviveCount()` 增加所有方块的存活轮数，将 `m_animationRunning` 设为 `true`。
6. 遍历棋盘，按合并逻辑计算每个方块的目标位置和是否消失，收集到 `moves` 数组中（类型为 `MoveData`，包含目标控件指针、目标位置、是否消失三个字段）。
7. 把 `moves` 中的所有动画加入平行动画组（同一时间一起播放），然后和 `createRandom()`（生成新方块）依次加入顺序动画组，返回给 `KeyboardHandler` 调度执行。

### 随机生成方块（`createRandom`）

`createRandom()` 负责在空位置随机生成一个新方块（数字 2 或 4，概率各 50%），同时承担胜负判定：

1. 找所有空位置。如果没有空位置且四个方向都无法移动则**失败**，弹出提示并退出。
2. 如果棋盘上出现了目标数字（数字/蔡徐坤模式为 2048，化学模式为 Xe 136/512）则**胜利**，弹出提示并退出。
3. 随机选一个空位置，生成一个方块，带有一个从中心放大出现的弹出动画。

### 三种游戏主题

#### 数字模式（`number`）
经典 2048 玩法。方块显示数字，不同数字有不同的背景颜色，数字越大颜色越深。

#### 蔡徐坤模式（`caixukun`）
方块上显示数字，但背景是一系列图片。切换到此模式时图片顺序会随机打乱，所以每次看到的排列都不一样。合并逻辑和数字模式一致，目标也是达到 2048。

#### 化学模式（`chemistry`）
方块上显示的是化学元素符号，比如 H2、He4、Be8、O16 等。这个模式有两个特殊规则：

- **元素对应**：数字 2 对应 H2，4 对应 He4，8 对应 Be8……一直到 512 对应 Xe136。目标是合成 Xe136（即数字 512）。
- **存活时间**：某些元素有"半衰期"——存活轮数达到上限就会自动消失。比如 Be8(5) 表示这个 Be8 还能存活 5 轮，括号里的数字每轮减 1，减到 0 就会消失。括号内显示的是剩余轮数（上限 - 当前存活轮数）。没有时限的元素（如 H2、He4）不显示括号。

### 设置系统

点击菜单栏 **File → Settings**（或按 Ctrl+,）打开设置对话框，可以修改：

| 分类 | 设置项 | 可选值 |
|------|--------|--------|
| 游戏设置 | 游戏主题 | 数字模式 / 蔡徐坤模式 / 化学模式 |
| 游戏设置 | 颜色主题 | 浅色 / 深色 / 跟随系统 |
| 游戏设置 | 音效开关 | 开 / 关 |
| 游戏设置 | 背景音乐开关 | 开 / 关（仅蔡徐坤模式可用） |
| 个性化 | 棋盘列数 (X) | 2 ~ 6 |
| 个性化 | 棋盘行数 (Y) | 2 ~ 6 |
| 个性化 | 动画时长 | 10 ms ~ 10000 ms |
| 个性化 | 动画缓动曲线 | 35 种可选（`Linear`、`InOutQuad`、`OutBounce` 等） |

设置通过 `QSettings` 自动保存到系统配置中，下次启动游戏时会自动恢复。点击 **OK** 保存并关闭，点击 **Apply** 保存但不关闭，点击 **Cancel** 放弃修改。

> 注意：在游戏已经开始后缩小棋盘，会弹出警告提示，因为缩小棋盘会导致外围的方块丢失。

## 单元测试

`tests/` 目录下包含三个测试文件，使用 Qt Test 框架：

- **`test_mainwindow.cpp`** —— 测试主窗口的移动逻辑（四方向滑动、合并、失败判定）、动画创建（含空指针保护）、随机生成（含胜负判定）、设置切换（主题、棋盘大小、动画参数、音效开关）、菜单栏、退出确认等，共 100 个测试用例。
- **`test_keyboardhandler.cpp`** —— 测试按键注册、事件过滤、动画执行、排队机制、清理重置、音效播放等，共 31 个测试用例。
- **`test_settingsdialog.cpp`** —— 测试设置对话框的控件初始化、默认值、读写设置、保存/取消/应用按钮行为、往返保存加载、复选框状态等，共 40 个测试用例。

总计 171 个测试用例，覆盖了游戏的核心逻辑和边界情况。

运行测试：

```bash
cd build
cmake ..
cmake --build .
ctest
```

Linux 环境中可以使用 `lcov` 检测覆盖率，该项目实现了 100% 的行覆盖率。

```bash
lcov --capture --directory . --output-file coverage.info
lcov --extract coverage.info "*/src/*" -o extracted.info --ignore-errors inconsistent
genhtml extracted.info -o coverage_report
```

## 构建与运行

### 环境要求

- Qt 6.0 或更高版本
- CMake 3.16 或更高版本
- 支持 C++17 的编译器

### 构建步骤

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

若使用 Visual Studio 进行构建，则在安装 Qt Visual Studio Tools 扩展后打开项目目录即可构建。

若使用 Qt Creator 进行构建，则点击菜单中的文件 -> 打开文件或项目，选择项目目录中的 `CMakeLists.txt` 即可构建。

### 运行游戏

```bash
./advanced-2048        # Linux / Mac
advanced-2048.exe      # Windows
```

## 操作说明

| 按键 | 功能 |
|------|------|
| W | 向上移动 |
| A | 向左移动 |
| S | 向下移动 |
| D | 向右移动 |
| Ctrl+, | 打开设置 |

## 收获

### 单元测试

在测试时需要修改和访问类中的 `private` 成员时，不需要为了单元测试而改为 `public` ，而是可以使用宏定义的方式，即在进行单元测试的时候，定义宏 `UNIT_TEST` ，使用 `#ifdef UNIT_TEST` 与 `#endif` 对友元类的声明进行包裹。

```cpp
#ifdef UNIT_TEST
    friend class TestMainWindow;
#endif
```

在对一些会导致直接关闭程序的代码进行测试时，可以采用宏隔离的方式，对于这些代码使用 `#ifndef UNIT_TEST` 与 `#endif` 进行包裹。同时使用 `std::function` 代替那些代码进行测试。

```cpp
connect(seqGroup, &QSequentialAnimationGroup::finished, [this, seqGroup]() {
    m_animationRunning = false;
    seqGroup->deleteLater();
    if (!validate('w') && !validate('a') && !validate('s') && !validate('d')) {
#ifndef UNIT_TEST
        QMessageBox::information(this, "Message", "You lose.");
        QTimer::singleShot(0, this, &QWidget::close);
#else
        // std::function<void()> showMessageCallback = nullptr;
        if (showMessageCallback) {
            showMessageCallback();
        }
#endif
    }
});
```

```cpp
bool messageShown = false;
bool windowClosed = false;
m_window->showMessageCallback = [&]() {
    messageShown = true;
    windowClosed = true;
};
QVERIFY(messageShown);
QVERIFY(windowClosed);
```

### 指针管理

项目中大量的地方使用了指针，很容易解空指针或已删除资源的指针。为解决这些问题，需要在删除资源后及时将指针赋值为 `nullptr` ，同时在尝试解指针前使用 `if` 判断是否为空指针。

## AI 工具使用说明

- `KeyboardHandler` 类的设计由 DeepSeek V3.2/V4 完成；
- 单元测试近一半的测试用例由 DeepSeek V3.2/V4 与 Gemini 3.1 Pro 完成；
- DeepSeek V3.2/V4 同时承担了查阅 Qt 文档的作用。

## 致谢

- 原始 2048 游戏：[Gabriele Cirulli](https://github.com/gabrielecirulli/2048)
- Qt 框架社区
