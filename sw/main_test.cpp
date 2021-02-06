#include <iostream>
#include "SocketDriver.h"
#include "Logger.h"

class Callback : public SocketListener
{
public:
   void onSocketEvent(DriverEvent ev, const std::vector<uint8_t>& data, size_t size)
   {
      std::cout << "got event " << (int)ev << ", size " << size <<std::endl;
   }
};


int main()
{
   logger_initialize();

   Callback callback;
   std::vector<uint8_t> test_data = {31,32,33,34,35,36, '\n'};
   std::unique_ptr<ISocketDriver> driver(new SocketDriver());

   driver->connect("127.0.0.1", 2222);
   driver->addListener(&callback);
   while(1)
   {
   }


}
