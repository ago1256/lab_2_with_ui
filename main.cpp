#include <QApplication>
#include "mainwindow.h"
#include "dict/dict.h"
#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Инициализация глобального словаря
    if (!init_global_dict()) {
        std::cerr << "Не удалось инициализировать словарь" << std::endl;
        return 1;
    }
    
    // Тестирование словаря
    run_tests();
    
    MainWindow window;
    window.show();
    
    return app.exec();
}