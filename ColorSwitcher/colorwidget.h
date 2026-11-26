#ifndef COLORWIDGET_H
#define COLORWIDGET_H

#include <QColor>
#include <QVector>
#include <QWidget>

class ColorWidget : public QWidget
{
    Q_OBJECT // Обязательно! Без этого МОС не создаст метаинформацию для нашего сигнала

public:
    explicit ColorWidget(QWidget *parent = nullptr);
    ~ColorWidget() override;

    void addColor(const QColor &c);
    void resetPalette();
    QColor currentColor() const;

    // Методы для управления цветом (вызываются из MainWindow)
    void stepForward();   // Следующий цвет
    void stepBackward();  // Предыдущий цвет
    void setRandomColor();// Случайный цвет
    void toggleBlink();   // Мигание

signals:
    // Собственный сигнал: цвет изменился.
    // Демонстрирует связку "событие -> сигнал" (клик мыши -> сигнал для окна)
    void colorChanged(const QColor &color);

protected:
    // Переопределяем события
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QVector<QColor> m_palette; // Палитра цветов
    int m_index;               // Индекс текущего цвета
    bool m_blinkOn;            // Флаг мигания
    QColor m_blinkBackup;      // Цвет для восстановления после мигания

    static QVector<QColor> defaultPalette();
    void emitCurrent();        // Вспомогательный метод для отправки сигнала
};

#endif // COLORWIDGET_H