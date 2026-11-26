#include "mainwindow.h"
#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRandomGenerator>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , m_mode(Mode::Cycle)         // Начинаем с режима "Цикл"
    , m_clickHandlerAlt(false)    // Начинаем с обычного обработчика клика
{
    setupUi(); // Строим интерфейс

    m_timer.setInterval(600); // Задаем интервал 600 мс
    // Таймер пока НЕ запущен, ждем нажатия кнопки "Старт"

    // Устанавливаем первоначальные соединения
    rebuildTimeoutConnection();
    reconnectCanvasHandler();

    setStatus("Готово. Нажмите «Старт/Стоп», чтобы запустить таймер.");
}

MainWindow::~MainWindow()
{
    qDebug() << "[DESTROY] MainWindow";
    m_timer.stop(); // Останавливаем таймер перед уничтожением окна
}

void MainWindow::setupUi()
{
    setWindowTitle("QTimer + лямбда-слоты + динамические соединения");
    resize(560, 640);

    m_rootLayout = new QVBoxLayout(this); // Главный вертикальный слой

    // 1. Добавляем холст
    m_canvas.setParent(this);
    m_rootLayout->addWidget(&m_canvas);

    // 2. Добавляем текстовые метки статуса
    m_statusLabel = new QLabel(this);
    m_statusLabel->setWordWrap(true);
    m_rootLayout->addWidget(m_statusLabel);

    m_intervalLabel = new QLabel(this);
    m_modeLabel = new QLabel(this);
    m_rootLayout->addWidget(m_intervalLabel);
    m_rootLayout->addWidget(m_modeLabel);

    // 3. Панель управления таймером (кнопки в ряд)
    auto *controls = new QHBoxLayout();
    auto *btnStart = new QPushButton("Старт/Стоп", this);
    auto *btnSlow = new QPushButton("Медленнее", this);
    auto *btnFast = new QPushButton("Быстрее", this);
    auto *btnMode = new QPushButton("Режим", this);

    controls->addWidget(btnStart);
    controls->addWidget(btnSlow);
    controls->addWidget(btnFast);
    controls->addWidget(btnMode);
    m_rootLayout->addLayout(controls);

    // Подключаем кнопки к слотам (лямбда не нужна, используем указатели на методы)
    connect(btnStart, &QPushButton::clicked, this, &MainWindow::toggleTimer);
    connect(btnSlow, &QPushButton::clicked, this, &MainWindow::decreaseInterval);
    connect(btnFast, &QPushButton::clicked, this, &MainWindow::increaseInterval);
    connect(btnMode, &QPushButton::clicked, this, &MainWindow::cycleMode);

    // 4. Панель динамической палитры
    auto *palHeader = new QLabel("Палитра (кнопки создаются динамически):", this);
    m_rootLayout->addWidget(palHeader);

    m_paletteGrid = new QGridLayout(); // Сетка для кнопок-цветов
    m_rootLayout->addLayout(m_paletteGrid);

    auto *palControls = new QHBoxLayout();
    auto *btnAdd = new QPushButton("+ Цвет", this);
    auto *btnClear = new QPushButton("Очистить палитру", this);
    auto *btnClick = new QPushButton("Сменить обработчик клика", this);

    palControls->addWidget(btnAdd);
    palControls->addWidget(btnClear);
    palControls->addWidget(btnClick);
    m_rootLayout->addLayout(palControls);

    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::addColorButton);
    connect(btnClear, &QPushButton::clicked, this, &MainWindow::clearPaletteButtons);
    connect(btnClick, &QPushButton::clicked, this, &MainWindow::toggleClickHandler);

    updateIntervalLabel();
    updateModeLabel();
}

// --- Слоты управления таймером ---

void MainWindow::toggleTimer()
{
    if (m_timer.isActive()) {
        m_timer.stop();
        setStatus("Таймер остановлен.");
    } else {
        m_timer.start();
        setStatus("Таймер запущен.");
    }
}

void MainWindow::decreaseInterval()
{
    int newInterval = m_timer.interval() - 100;
    if (newInterval < 100) newInterval = 100;
    m_timer.setInterval(newInterval);
    qDebug() << "[TIMER] interval ->" << newInterval;
    updateIntervalLabel();
}

void MainWindow::increaseInterval()
{
    int newInterval = m_timer.interval() + 100;
    m_timer.setInterval(newInterval);
    qDebug() << "[TIMER] interval ->" << newInterval;
    updateIntervalLabel();
}

void MainWindow::cycleMode()
{
    if (m_mode == Mode::Cycle) m_mode = Mode::Random;
    else if (m_mode == Mode::Random) m_mode = Mode::Blink;
    else m_mode = Mode::Cycle;

    qDebug() << "[TIMER] режим ->" << modeName();
    rebuildTimeoutConnection();
    updateModeLabel();
}

// --- Динамические соединения ---

void MainWindow::rebuildTimeoutConnection()
{
    disconnect(m_timeoutConn);

    if (m_mode == Mode::Cycle) {
        m_timeoutConn = connect(&m_timer, &QTimer::timeout, this, [this]() {
            qDebug() << "[TIMER] timeout, режим =" << modeName()
                << ", цвет =" << m_canvas.currentColor().name();
            m_canvas.stepForward();
        });
    } else if (m_mode == Mode::Random) {
        m_timeoutConn = connect(&m_timer, &QTimer::timeout, this, [this]() {
            qDebug() << "[TIMER] timeout, режим =" << modeName()
                << ", цвет =" << m_canvas.currentColor().name();
            m_canvas.setRandomColor();
        });
    } else if (m_mode == Mode::Blink) {
        m_timeoutConn = connect(&m_timer, &QTimer::timeout, this, [this]() {
            qDebug() << "[TIMER] timeout, режим =" << modeName()
                << ", цвет =" << m_canvas.currentColor().name();
            m_canvas.toggleBlink();
        });
    }
}

void MainWindow::addColorButton()
{
    // Генерируем случайный цвет
    QColor randomColor = QColor::fromRgb(QRandomGenerator::global()->generate());

    // Создаем кнопку динамически. Родитель - this (MainWindow).
    PaletteButton *btn = new PaletteButton(randomColor, this);

    // Вычисляем позицию в сетке (3 колонки)
    int row = m_paletteButtons.size() / 3;
    int col = m_paletteButtons.size() % 3;
    m_paletteGrid->addWidget(btn, row, col);

    // Сохраняем QPointer на кнопку в список
    m_paletteButtons.append(btn);

    // ДИНАМИЧЕСКОЕ СОЕДИНЕНИЕ
    // При клике на эту кнопку добавляем её цвет в палитру холста.
    // Захватываем [this, btn] (btn по значению, это указатель).
    QMetaObject::Connection conn = connect(btn, &QPushButton::clicked, this, [this, btn]() {
        m_canvas.addColor(btn->color());
        setStatus(QString("Добавлен цвет: %1").arg(btn->color().name()));
    });

    // Сохраняем "квитанцию" о соединении
    m_dynamicConnections.append(conn);
}

void MainWindow::clearPaletteButtons()
{
    // 1. Разрываем все динамические соединения
    for (const QMetaObject::Connection &conn : m_dynamicConnections) {
        disconnect(conn);
    }
    m_dynamicConnections.clear();

    // 2. Удаляем сами кнопки из памяти
    for (QPointer<PaletteButton> &btn : m_paletteButtons) {
        if (!btn.isNull()) {
            delete btn.data(); // Безопасное удаление
        }
    }
    m_paletteButtons.clear();
    setStatus("Палитра очищена, соединения разорваны.");
}

void MainWindow::toggleClickHandler()
{
    disconnect(m_clickConn); // Отключаем старый обработчик
    m_clickHandlerAlt = !m_clickHandlerAlt; // Меняем флаг на противоположный
    reconnectCanvasHandler(); // Подключаем новый
}

void MainWindow::reconnectCanvasHandler()
{
    qDebug() << "[CANVAS] переподключение colorChanged ->"
             << (m_clickHandlerAlt ? "альтернативный обработчик" : "обычный обработчик");

    if (m_clickHandlerAlt) {
        m_clickConn = connect(&m_canvas, &ColorWidget::colorChanged, this, [this](const QColor &c) {
            setWindowTitle(QString("Цвет: %1").arg(c.name()));
            setStatus(QString("Альт-обработчик: цвет изменен на %1").arg(c.name()));
        });
    } else {
        setWindowTitle("QTimer + лямбда-слоты + динамические соединения");
        m_clickConn = connect(&m_canvas, &ColorWidget::colorChanged, this, [this](const QColor &c) {
            setStatus(QString("Обычный обработчик: цвет изменен на %1").arg(c.name()));
        });
    }
}

// --- Вспомогательные методы ---

void MainWindow::setStatus(const QString &text) { m_statusLabel->setText(text); }
void MainWindow::updateIntervalLabel() { m_intervalLabel->setText(QString("Интервал: %1 мс").arg(m_timer.interval())); }
void MainWindow::updateModeLabel() { m_modeLabel->setText(QString("Режим: %1").arg(modeName())); }

QString MainWindow::modeName() const {
    switch (m_mode) {
    case Mode::Cycle: return "Цикл";
    case Mode::Random: return "Случайный";
    case Mode::Blink: return "Мигание";
    }
    return "Неизвестно";
}