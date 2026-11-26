#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMetaObject>
#include <QPointer>
#include <QTimer>
#include <QWidget>
#include <QList>

// Подключаем наши классы
#include "colorwidget.h"
#include "palettebutton.h"

// Предварительное объявление классов (чтобы не тянуть тяжелые заголовки в .h)
class QLabel;
class QVBoxLayout;
class QGridLayout;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    // Слоты для кнопок управления
    void toggleTimer();          // Старт/Стоп таймера
    void decreaseInterval();     // Уменьшить интервал
    void increaseInterval();     // Увеличить интервал
    void cycleMode();            // Переключить режим (Цикл -> Случайный -> Мигание)

    // Слоты для работы с динамической палитрой
    void addColorButton();       // Динамическое создание кнопки-цвета и соединения
    void clearPaletteButtons();  // Разрыв динамических соединений и удаление кнопок
    void toggleClickHandler();   // Переподключение сигнала холста

private:
    enum class Mode { Cycle, Random, Blink }; // Режимы работы таймера

    void setupUi();                  // Создание интерфейса
    void rebuildTimeoutConnection(); // Пересоздание соединения таймера
    void reconnectCanvasHandler();   // Пересоздание соединения холста

    // Вспомогательные методы для обновления текста на экране
    void setStatus(const QString &text);
    void updateIntervalLabel();
    void updateModeLabel();
    QString modeName() const;

    // --- Переменные ---
    QTimer m_timer;              // Наш таймер
    ColorWidget m_canvas;        // Наш холст (создается на стеке, живет вместе с окном)
    Mode m_mode;                 // Текущий режим
    bool m_clickHandlerAlt;      // Флаг: какой обработчик клика сейчас активен

    // Хранилища соединений (те самые "квитанции" для disconnect)
    QMetaObject::Connection m_timeoutConn;
    QMetaObject::Connection m_clickConn;

    // Указатели на элементы интерфейса
    QVBoxLayout *m_rootLayout;
    QGridLayout *m_paletteGrid;
    QLabel *m_statusLabel;
    QLabel *m_intervalLabel;
    QLabel *m_modeLabel;

    // Списки для динамических элементов
    QList<QPointer<PaletteButton>> m_paletteButtons;     // Безопасные указатели на кнопки
    QList<QMetaObject::Connection> m_dynamicConnections; // Соединения этих кнопок
};

#endif // MAINWINDOW_H