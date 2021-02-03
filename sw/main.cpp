#include <iostream>
#include "main_window.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);
   MainWindow w;
   w.show();

   a.exec();

   std::cout << "Hello World\n" << std::endl;
   return 1;
}