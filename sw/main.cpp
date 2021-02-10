#include "main_window.h"
#include "QtWidgets/QApplication"

#include "Logger.h"
#include "DataProvider.h"
#include "SocketDriver.h"

int main(int argc, char *argv[])
{
   logger_initialize();

   QApplication a(argc, argv);
   MainWindow w;
   std::unique_ptr<ISocketDriver> sock_driver(new SocketDriver());
   std::unique_ptr<IDataProvider> data_provider(new DataProvider(w, *sock_driver));
   data_provider->run("127.0.0.1", 2222, '\n');
   w.setWindowState(Qt::WindowFullScreen);
   w.show();

   return a.exec();
}
