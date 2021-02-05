#include "gtest/gtest.h"
#include "gmock/gmock.h"

#define SOCKDRV_FRIEND_TESTS \
   FRIEND_TEST(SocketDriverFixture, socket_read_tests);\
   friend class SocketDriverFixture;

#include "SocketDriver.h"
#include <sys/socket.h>
/* ============================= */
/**
 * @file SocketDriverTests.cpp
 *
 * @brief Unit tests to verify behavior of Socket Driver,
 *
 * @author Jacek Skowronek
 * @date 05/02/2021
 */
/* ============================= */

using namespace testing;

/* mock for system functions */
struct SystemCallMock
{
   MOCK_METHOD3(connect, int(int, const struct sockaddr *, socklen_t));
   MOCK_METHOD4(recv, ssize_t(int, void *, size_t, int));
   MOCK_METHOD4(send, ssize_t(int, const void *, size_t, int));
   MOCK_METHOD3(socket, int(int, int, int));
   MOCK_METHOD1(close, int(int));

};
SystemCallMock* sys_call_mock;

struct ListenerMock : public SocketListener
{
   MOCK_METHOD3(onSocketEvent, void(DriverEvent, const std::vector<uint8_t>&, size_t));
};

struct SocketDriverFixture : public testing::Test
{
   void SetUp()
   {
      sys_call_mock = new SystemCallMock;
      m_test_subject.reset(new SocketDriver());
   }
   void TearDown()
   {
      m_test_subject.reset(nullptr);
      delete sys_call_mock;
   }
   ListenerMock listener_mock;
   std::unique_ptr<ISocketDriver> m_test_subject;
};

/* system calls to replace with mocked on linking stage */
namespace system_call
{
__attribute__((weak)) int connect(int socket, const struct sockaddr *address, socklen_t address_len)
{
   return sys_call_mock->connect(socket, address, address_len);
}
__attribute__((weak)) ssize_t recv(int socket, void *buffer, size_t length, int flags)
{
   return sys_call_mock->recv(socket, buffer, length, flags);
}
__attribute__((weak)) ssize_t send(int socket, const void *message, size_t length, int flags)
{
   return sys_call_mock->send(socket, message, length, flags);
}
__attribute__((weak)) int socket(int domain, int type, int protocol)
{
   return sys_call_mock->socket(domain, type, protocol);
}
__attribute__((weak)) int close (int fd)
{
   return sys_call_mock->close(fd);
}

}

/**
 * @test Tests of connection and disconnection
 */
TEST_F(SocketDriverFixture, connect_disconnect_tests)
{
   int SOCK_FD = 1;
   m_test_subject->addListener(&listener_mock);
   /**
    * <b>scenario</b>: IP address is empty.<br>
    * <b>expected</b>: Connection not started.<br>
    * ************************************************
    */
   EXPECT_FALSE(m_test_subject->connect("", 1111));

   /**
    * <b>scenario</b>: Port is equal to 0.<br>
    * <b>expected</b>: Connection not started.<br>
    * ************************************************
    */
   EXPECT_FALSE(m_test_subject->connect("www.test.pl", 0));

   /**
    * <b>scenario</b>: Cannot create socket endpoint.<br>
    * <b>expected</b>: Connection not started.<br>
    * ************************************************
    */
   EXPECT_CALL(*sys_call_mock, socket(AF_INET, SOCK_STREAM, 0)).WillOnce(Return(-1));
   EXPECT_FALSE(m_test_subject->connect("www.test.pl", 1111));

   /**
    * <b>scenario</b>: Invalid form of IP address.<br>
    * <b>expected</b>: Connection not started.<br>
    * ************************************************
    */
   EXPECT_CALL(*sys_call_mock, socket(AF_INET, SOCK_STREAM, 0)).WillOnce(Return(SOCK_FD));
   EXPECT_CALL(*sys_call_mock, close(SOCK_FD));
   EXPECT_FALSE(m_test_subject->connect("www.test.pl", 1111));

   /**
    * <b>scenario</b>: Cannot connect to server.<br>
    * <b>expected</b>: Connection not started.<br>
    * ************************************************
    */
   EXPECT_CALL(*sys_call_mock, socket(AF_INET, SOCK_STREAM, 0)).WillOnce(Return(SOCK_FD));
   EXPECT_CALL(*sys_call_mock, connect(SOCK_FD, _, _)).WillOnce(Return(-1));
   EXPECT_CALL(*sys_call_mock, close(SOCK_FD));
   EXPECT_FALSE(m_test_subject->connect("192.168.100.100", 1111));

   /**
    * <b>scenario</b>: Correct connection sequence.<br>
    * <b>expected</b>: Connection started.<br>
    * ************************************************
    */
   EXPECT_CALL(*sys_call_mock, socket(AF_INET, SOCK_STREAM, 0)).WillOnce(Return(SOCK_FD));
   EXPECT_CALL(*sys_call_mock, connect(SOCK_FD, _, _)).WillOnce(Return(1));
   EXPECT_CALL(listener_mock, onSocketEvent(DriverEvent::DRIVER_CONNECTED,_,_));
   EXPECT_CALL(*sys_call_mock, recv(_,_,_,_)).WillRepeatedly(Return(1));
   EXPECT_CALL(*sys_call_mock, close(SOCK_FD)).Times(1);
   EXPECT_TRUE(m_test_subject->connect("192.168.100.100", 1111));

   m_test_subject->disconnect();

   m_test_subject->removeListener(&listener_mock);
}

/**
 * @test Tests of writing data to socket
 */
TEST_F(SocketDriverFixture, socket_write_tests)
{
   uint8_t data_size = 20;
   std::vector<uint8_t> test_bytes = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
   /**
    * <b>scenario</b>: Data size to write is bigger than possible.<br>
    * <b>expected</b>: Data not written.<br>
    * ************************************************
    */
   EXPECT_FALSE(m_test_subject->write({}, SOCKDRV_MAX_RW_SIZE+1));

   /**
    * <b>scenario</b>: Send 50 bytes - bytes written to socket in 2 parts.<br>
    * <b>expected</b>: Data sent correctly.<br>
    * ************************************************
    */
   EXPECT_CALL(*sys_call_mock, send(_,_,_,_))
         .WillOnce(Invoke([&](int, const void *message, size_t length, int)->ssize_t
         {
            const uint8_t* data = static_cast<const uint8_t*>(message);
            EXPECT_EQ(data[0], 0);
            EXPECT_EQ(data[9], 9);
            EXPECT_EQ(length, 20);
            return 10;
         }))
         .WillOnce(Invoke([&](int, const void *message, size_t length, int)->ssize_t
         {
            const uint8_t* data = static_cast<const uint8_t*>(message);
            EXPECT_EQ(data[0], 10);
            EXPECT_EQ(data[9], 19);
            EXPECT_EQ(length, 10);
            return 10;
         }));

   EXPECT_TRUE(m_test_subject->write(test_bytes, data_size));

   /**
    * <b>scenario</b>: Send 50 bytes - bytes written to socket in 1 part.<br>
    * <b>expected</b>: Data sent correctly.<br>
    * ************************************************
    */
   EXPECT_CALL(*sys_call_mock, send(_,_,_,_))
         .WillOnce(Invoke([&](int, const void *message, size_t length, int)->ssize_t
         {
            const uint8_t* data = static_cast<const uint8_t*>(message);
            EXPECT_EQ(data[0], 0);
            EXPECT_EQ(data[19], 19);
            EXPECT_EQ(length, 20);
            return 20;
         }));

   EXPECT_TRUE(m_test_subject->write(test_bytes, data_size));

   /**
    * <b>scenario</b>: Cannot write byte to socket.<br>
    * <b>expected</b>: False returned.<br>
    * ************************************************
    */
   EXPECT_CALL(*sys_call_mock, send(_,_,_,_)).WillOnce(Return(0));
   EXPECT_FALSE(m_test_subject->write(test_bytes, data_size));

}

/**
 * @test Tests of reading data to socket
 */
TEST_F(SocketDriverFixture, socket_read_tests)
{
   m_test_subject->addListener(&listener_mock);
   /**
    * <b>scenario</b>: Complete message received in two chunks.<br>
    * <b>expected</b>: Data merged and callback called.<br>
    * ************************************************
    */
   EXPECT_CALL(*sys_call_mock, recv(_,_,_,_))
         .WillOnce(Invoke([&](int, void *buffer, size_t, int)->ssize_t
         {
            uint8_t* buf = static_cast<uint8_t*>(buffer);
            for (uint8_t i = 0; i < 5; i++)
            {
               buf[i] = i;
            }
            return 5;
         }))
         .WillOnce(Invoke([&](int, void *buffer, size_t, int)->ssize_t
         {
            uint8_t* buf = static_cast<uint8_t*>(buffer);
            buf[0] = '\n';
            static_cast<SocketDriver*>(m_test_subject.get())->m_thread_running = false;
            return 2;
         }));
   EXPECT_CALL(listener_mock, onSocketEvent(_,_,_)).WillOnce(Invoke([&](DriverEvent ev, const std::vector<uint8_t>& data, size_t size)
         {
            EXPECT_EQ(size, 5);
            EXPECT_EQ(ev, DriverEvent::DRIVER_DATA_RECV);
            EXPECT_THAT(data, ElementsAre(0,1,2,3,4));
         }));
   static_cast<SocketDriver*>(m_test_subject.get())->threadExecute();
   m_test_subject->removeListener(&listener_mock);

   /**
    * <b>scenario</b>: Delimiter received in the middle of the data.<br>
    * <b>expected</b>: Rest of the message should be kept and waits for next chunk.<br>
    * ************************************************
    */
   EXPECT_CALL(*sys_call_mock, recv(_,_,_,_))
         .WillOnce(Invoke([&](int, void *buffer, size_t, int)->ssize_t
         {
            uint8_t* buf = static_cast<uint8_t*>(buffer);
            for (uint8_t i = 0; i < 15; i++)
            {
               buf[i] = i;
            }
            return 15;
         }))
         .WillOnce(Invoke([&](int, void *buffer, size_t, int)->ssize_t
         {
            uint8_t* buf = static_cast<uint8_t*>(buffer);
            buf[0] = '\n';
            static_cast<SocketDriver*>(m_test_subject.get())->m_thread_running = false;
            return 1;
         }));
   EXPECT_CALL(listener_mock, onSocketEvent(_,_,_))
   .WillOnce(Invoke([&](DriverEvent ev, const std::vector<uint8_t>& data, size_t size)
         {
            EXPECT_EQ(size, 10);
            EXPECT_EQ(ev, DriverEvent::DRIVER_DATA_RECV);
            EXPECT_THAT(data, ElementsAre(0,1,2,3,4,5,6,7,8,9));
         }))
   .WillOnce(Invoke([&](DriverEvent ev, const std::vector<uint8_t>& data, size_t size)
         {
            EXPECT_EQ(size, 4);
            EXPECT_EQ(ev, DriverEvent::DRIVER_DATA_RECV);
            EXPECT_THAT(data, ElementsAre(11,12,13,14));
         }));

   static_cast<SocketDriver*>(m_test_subject.get())->threadExecute();

   /**
    * <b>scenario</b>: Driver error occurs - e.g. server has been closed suddenly.<br>
    * <b>expected</b>: Callback notified.<br>
    * ************************************************
    */
   EXPECT_CALL(*sys_call_mock, recv(_,_,_,_)).WillOnce(Return(0));
   EXPECT_CALL(listener_mock, onSocketEvent(DriverEvent::DRIVER_DISCONNECTED,_,_));
   EXPECT_CALL(*sys_call_mock, close(_));
   static_cast<SocketDriver*>(m_test_subject.get())->threadExecute();

   m_test_subject->removeListener(&listener_mock);

}
