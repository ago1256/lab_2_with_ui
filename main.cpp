#include <QApplication>
#include "mainwindow.h"
#include "dict/dict.h"
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    if (!init_global_dict()) {
        std::cerr << "Error with dictionary" << std::endl;
        return 1;
    }
    
    std::cout << "Dictionary loaded: " << global_dict.size() << " words\n";
    
    MainWindow window;
    window.show();
    
    return app.exec();
}

//алгоритм бойера мура хорспула+
//сериализация разобраться+
//при добавление и поиске слова приводить к одному регистру+