#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // 1. Создаем приложение (обязательно для GUI)
    QApplication app(argc, argv);

    // 2. Создаем главное окно
    MainWindow window;

    // 3. Показываем его на экране
    window.show();

    // 4. ЗАПУСКАЕМ ЦИКЛ СОБЫТИЙ!
    return app.exec();
}