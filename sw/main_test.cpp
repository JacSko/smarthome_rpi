#include "main_window.h"
#include "QtWidgets/QApplication"
#include "DataProvider.h"
#include "Logger.h"

int main(int argc, char *argv[])
{
   /* TODO load settings from file */
   logger_initialize();

   QApplication a(argc, argv);
   MainWindow w;
   std::unique_ptr<IDataProvider> data_provider(new DataProvider(w));
   data_provider->run("127.0.0.1", 2222, '\n');

   w.show();

   a.exec();

   return 1;
}
