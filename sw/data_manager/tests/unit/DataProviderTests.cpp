#include "gtest/gtest.h"
#include "gmock/gmock.h"

#define DATA_PROVIDER_FRIEND_TESTS \
   FRIEND_TEST(DataProviderFixture, thread_execution_tests);\
   friend class DataProviderFixture;

#include "DataProvider.h"
#include "logger_mock.hpp"
#include "MainWindowWrapperMock.h"
#include "SocketDriverMock.h"
#include "notification_types.h"
/* ============================= */
/**
 * @file DataProviderTests.cpp
 *
 * @brief Unit tests to verify behavior of DataProvider,
 *
 * @author Jacek Skowronek
 * @date 08/02/2021
 */
/* ============================= */

using namespace testing;

struct SleepMock
{
   MOCK_METHOD1(sleep, void(std::chrono::milliseconds));
};

std::unique_ptr<SleepMock> sleep_mock;

namespace thread
{
   __attribute__((weak)) void sleep (std::chrono::milliseconds ms)
   {
      sleep_mock->sleep(ms);
   }
}

struct DataProviderFixture : public testing::Test
{
   void SetUp()
   {
      mock_logger_init();
      sleep_mock.reset(new SleepMock());
      m_test_subject.reset(new DataProvider(m_window_mock, m_driver_mock));
   }
   void TearDown()
   {
      EXPECT_CALL(m_driver_mock, disconnect());
      EXPECT_CALL(m_driver_mock, removeListener(_));
      m_test_subject.reset(nullptr);
      sleep_mock.reset(nullptr);
      mock_logger_deinit();
   }
   SocketDriverMock m_driver_mock;
   MainWindowWrapperMock m_window_mock;
   std::unique_ptr<IDataProvider> m_test_subject;
};

struct DataProviderSocketListenerFixture : public testing::Test
{
   void SetUp()
   {
      mock_logger_init();
      sleep_mock.reset(new SleepMock());
      m_test_subject.reset(new DataProvider(m_window_mock, m_driver_mock));
   }
   void TearDown()
   {
      EXPECT_CALL(m_driver_mock, disconnect());
      EXPECT_CALL(m_driver_mock, removeListener(_));
      m_test_subject.reset(nullptr);
      sleep_mock.reset(nullptr);
      mock_logger_deinit();
   }
   SocketDriverMock m_driver_mock;
   MainWindowWrapperMock m_window_mock;
   std::unique_ptr<SocketListener> m_test_subject;
};


TEST_F(DataProviderFixture, run_module_tests)
{
   /**
    * <b>scenario</b>: Run module for first time.<br>
    * <b>expected</b>: Thread started, delimiter set, listener added, driver is asked about connection status.<br>
    * ************************************************
    */
   EXPECT_CALL(m_driver_mock, addListener(_));
   EXPECT_CALL(m_driver_mock, setDelimiter('\n'));
   EXPECT_CALL(m_driver_mock, isConnected()).WillRepeatedly(Return(true));
   EXPECT_CALL(*sleep_mock, sleep(_)).Times(AtLeast(1));
   EXPECT_TRUE(m_test_subject->run("", 2222, '\n'));

   /**
    * <b>scenario</b>: Module is running, run requested again<br>
    * <b>expected</b>: False returned.<br>
    * ************************************************
    */
   EXPECT_FALSE(m_test_subject->run("", 2222, '\n'));
}

TEST_F(DataProviderFixture, thread_execution_tests)
{
   DataProvider* m_test = static_cast<DataProvider*>(m_test_subject.get());
   /**
    * <b>scenario</b>: Thread started, connection established. <br>
    * <b>expected</b>: Thread keeps checking the connection status.<br>
    * ************************************************
    */
   EXPECT_CALL(*sleep_mock, sleep(_)).Times(2);
   EXPECT_CALL(m_driver_mock, connect(_,_)).WillOnce(Return(true));
   EXPECT_CALL(m_driver_mock, isConnected()).WillOnce(Return(false))
         .WillOnce(Invoke([&]() -> bool
         {
            m_test->m_thread_running = false;
            return true;
         }));
   static_cast<DataProvider*>(m_test_subject.get())->executeThread();
   /**
    * <b>scenario</b>: Connection with driver established.<br>
    * <b>expected</b>: No further connections requested.<br>
    * ************************************************
    */
   EXPECT_CALL(*sleep_mock, sleep(_));
   EXPECT_CALL(m_driver_mock, connect(_,_)).Times(0);
   EXPECT_CALL(m_driver_mock, isConnected()).WillOnce(Invoke([&]() -> bool
         {
            m_test->m_thread_running = false;
            return true;
         }));
   static_cast<DataProvider*>(m_test_subject.get())->executeThread();

   /**
    * <b>scenario</b>: Connection cannot be established. <br>
    * <b>expected</b>: Thread is keep trying to connect.<br>
    * ************************************************
    */
   EXPECT_CALL(*sleep_mock, sleep(_)).Times(3);
   EXPECT_CALL(m_driver_mock, connect(_,_)).WillOnce(Return(false))
                                           .WillOnce(Return(false))
       .WillOnce(Invoke([&](const std::string& addr, uint16_t port) ->bool
       {
         m_test->m_thread_running = false;
         return false;
       }));

   EXPECT_CALL(m_driver_mock, isConnected()).WillOnce(Return(false))
     .WillOnce(Return(false))
     .WillOnce(Return(false));

   m_test->executeThread();

}

TEST_F(DataProviderSocketListenerFixture, messasge_integrity_check_tests)
{
   const uint8_t PAYLOAD_SIZE = 2;
   const uint8_t DEFAULT_MESSAGE_SIZE = NTF_HEADER_SIZE + PAYLOAD_SIZE;
   std::vector<uint8_t> test_bytes(DEFAULT_MESSAGE_SIZE, 0);

   /**
    * <b>scenario</b>: Event received with incorrect message size. <br>
    * <b>expected</b>: Data not sent to main window.<br>
    * ************************************************
    */
   EXPECT_CALL(m_window_mock, setFanState(_)).Times(0);
   m_test_subject->onSocketEvent(DriverEvent::DRIVER_DATA_RECV, {}, 5);

   /**
    * <b>scenario</b>: Incorrect message length indicated in header. <br>
    * <b>expected</b>: Data not sent to main window.<br>
    * ************************************************
    */
   EXPECT_CALL(m_window_mock, setFanState(_)).Times(0);
   test_bytes[NTF_BYTES_COUNT_OFFSET] = PAYLOAD_SIZE + 1;
   m_test_subject->onSocketEvent(DriverEvent::DRIVER_DATA_RECV, test_bytes, DEFAULT_MESSAGE_SIZE);

   /**
    * <b>scenario</b>: Incorrect NTF_GROUP received. <br>
    * <b>expected</b>: Data not sent to main window.<br>
    * ************************************************
    */
   EXPECT_CALL(m_window_mock, setFanState(_)).Times(0);
   test_bytes[NTF_BYTES_COUNT_OFFSET] = PAYLOAD_SIZE;
   test_bytes[NTF_ID_OFFSET] = 0;
   m_test_subject->onSocketEvent(DriverEvent::DRIVER_DATA_RECV, test_bytes, DEFAULT_MESSAGE_SIZE);

}

TEST_F(DataProviderSocketListenerFixture, fan_events_handling_tests)
{
   const uint8_t PAYLOAD_SIZE = 1;
   const uint8_t DEFAULT_MESSAGE_SIZE = NTF_HEADER_SIZE + PAYLOAD_SIZE;
   std::vector<uint8_t> test_bytes(DEFAULT_MESSAGE_SIZE, 0);

   test_bytes[NTF_ID_OFFSET] = NTF_FAN_STATE;
   test_bytes[NTF_BYTES_COUNT_OFFSET] = PAYLOAD_SIZE;
   /**
    * <b>scenario</b>: Invalid REQ_TYPE received. <br>
    * <b>expected</b>: Data not sent to main window.<br>
    * ************************************************
    */
   EXPECT_CALL(m_window_mock, setFanState(_)).Times(0);
   test_bytes[NTF_ID_OFFSET] = NTF_FAN_STATE;
   test_bytes[NTF_REQ_TYPE_OFFSET] = 0;
   m_test_subject->onSocketEvent(DriverEvent::DRIVER_DATA_RECV, test_bytes, DEFAULT_MESSAGE_SIZE);

   /**
    * <b>scenario</b>: Correct fan state message received. <br>
    * <b>expected</b>: Data sent to main window.<br>
    * ************************************************
    */
   EXPECT_CALL(m_window_mock, setFanState(FAN_STATE_SUSPEND));
   test_bytes[NTF_REQ_TYPE_OFFSET] = NTF_NTF;
   test_bytes[NTF_HEADER_SIZE] = FAN_STATE_SUSPEND;
   m_test_subject->onSocketEvent(DriverEvent::DRIVER_DATA_RECV, test_bytes, DEFAULT_MESSAGE_SIZE);
}

TEST_F(DataProviderSocketListenerFixture, inputs_events_handling_tests)
{
   const uint8_t PAYLOAD_SIZE = 2;
   const uint8_t DEFAULT_MESSAGE_SIZE = NTF_HEADER_SIZE + PAYLOAD_SIZE;
   std::vector<uint8_t> test_bytes(DEFAULT_MESSAGE_SIZE, 0);

   test_bytes[NTF_ID_OFFSET] = NTF_INPUTS_STATE;
   test_bytes[NTF_BYTES_COUNT_OFFSET] = PAYLOAD_SIZE;

   /**
    * <b>scenario</b>: Invalid REQ_TYPE received. <br>
    * <b>expected</b>: Data not sent to main window.<br>
    * ************************************************
    */
   EXPECT_CALL(m_window_mock, setInputState(_,_)).Times(0);
   test_bytes[NTF_REQ_TYPE_OFFSET] = 0;
   m_test_subject->onSocketEvent(DriverEvent::DRIVER_DATA_RECV, test_bytes, DEFAULT_MESSAGE_SIZE);

   /**
    * <b>scenario</b>: Correct fan state message received. <br>
    * <b>expected</b>: Data sent to main window.<br>
    * ************************************************
    */
   EXPECT_CALL(m_window_mock, setInputState(INPUT_BEDROOM_AC, INPUT_STATE_ACTIVE));
   test_bytes[NTF_REQ_TYPE_OFFSET] = NTF_NTF;
   test_bytes[NTF_HEADER_SIZE] = INPUT_BEDROOM_AC;
   test_bytes[NTF_HEADER_SIZE + 1] = INPUT_STATE_ACTIVE;
   m_test_subject->onSocketEvent(DriverEvent::DRIVER_DATA_RECV, test_bytes, DEFAULT_MESSAGE_SIZE);
}

TEST_F(DataProviderSocketListenerFixture, env_events_handling_tests)
{
   const uint8_t PAYLOAD_SIZE = 6;
   const uint8_t DEFAULT_MESSAGE_SIZE = NTF_HEADER_SIZE + PAYLOAD_SIZE;
   std::vector<uint8_t> test_bytes(DEFAULT_MESSAGE_SIZE, 0);

   test_bytes[NTF_ID_OFFSET] = NTF_ENV_SENSOR_DATA;
   test_bytes[NTF_BYTES_COUNT_OFFSET] = PAYLOAD_SIZE;
   /**
    * <b>scenario</b>: Invalid REQ_TYPE received. <br>
    * <b>expected</b>: Data not sent to main window.<br>
    * ************************************************
    */
   EXPECT_CALL(m_window_mock, setEnvState(_,_,_,_,_)).Times(0);
   test_bytes[NTF_REQ_TYPE_OFFSET] = 0;
   m_test_subject->onSocketEvent(DriverEvent::DRIVER_DATA_RECV, test_bytes, DEFAULT_MESSAGE_SIZE);

   /**
    * <b>scenario</b>: Correct fan state message received. <br>
    * <b>expected</b>: Data sent to main window.<br>
    * ************************************************
    */
   EXPECT_CALL(m_window_mock, setEnvState(ENV_BEDROOM, 23, 5, 50, 2));
   test_bytes[NTF_REQ_TYPE_OFFSET] = NTF_NTF;
   test_bytes[NTF_HEADER_SIZE] = ENV_BEDROOM;
   test_bytes[NTF_HEADER_SIZE + 1] = 0; /* sensor type - don't care */
   test_bytes[NTF_HEADER_SIZE + 2] = 50;
   test_bytes[NTF_HEADER_SIZE + 3] = 2;
   test_bytes[NTF_HEADER_SIZE + 4] = 23;
   test_bytes[NTF_HEADER_SIZE + 5] = 5;
   m_test_subject->onSocketEvent(DriverEvent::DRIVER_DATA_RECV, test_bytes, DEFAULT_MESSAGE_SIZE);
}












