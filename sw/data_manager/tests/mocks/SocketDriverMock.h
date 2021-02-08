#ifndef _SOCKET_DRIVER_MOCK_H_
#define _SOCKET_DRIVER_MOCK_H_

#include "gmock/gmock.h"
#include "ISocketDriver.h"


class SocketDriverMock : public ISocketDriver
{
public:
   MOCK_METHOD2(connect, bool(const std::string&, uint16_t));
   MOCK_METHOD0(disconnect, bool());
   MOCK_METHOD0(isConnected, bool());
   MOCK_METHOD1(addListener, void(SocketListener*));
   MOCK_METHOD1(setDelimiter, void(char c));
   MOCK_METHOD1(removeListener, void(SocketListener*));
   MOCK_METHOD2(write, bool(const std::vector<uint8_t>&, size_t));

};


#endif
