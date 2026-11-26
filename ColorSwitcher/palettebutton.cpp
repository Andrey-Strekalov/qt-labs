#include "palettebutton.h"
#include <QDebug>

PaletteButton::PaletteButton(const QColor &color, QWidget *parent)
    : QPushButton(parent)
    , m_color(color)
{
    // Настраиваем внешний вид кнопки
    setText(color.name()); // Пишем на кнопке HEX-код цвета (например, #ff0000)
    setToolTip(QStringLiteral("Добавить этот цвет в палитру холста"));

    // Логируем создание
    qDebug() << "[CREATE] PaletteButton" << color.name();
}

PaletteButton::~PaletteButton()
{
    // Логируем уничтожение
    qDebug() << "[DESTROY] PaletteButton" << m_color.name();
}