#ifndef PALETTEBUTTON_H
#define PALETTEBUTTON_H

#include <QColor>
#include <QPushButton>

class PaletteButton : public QPushButton
{
    Q_OBJECT // Добавляем для корректной работы в системе метаобъектов Qt

public:
    explicit PaletteButton(const QColor &color, QWidget *parent = nullptr);
    ~PaletteButton() override;

    // Геттер для получения цвета, который хранит эта кнопка.
    // Тело написано прямо в заголовочном файле — это неявный inline, так можно.
    QColor color() const { return m_color; }

private:
    QColor m_color; // Цвет, который хранит кнопка
};

#endif // PALETTEBUTTON_H