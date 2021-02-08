#ifndef _DATA_PROVIDER_H_
#define _DATA_PROVIDER_H_

/**
 * @file DataProvider.h
 *
 * @brief
 *    Implementation of IDataProvider interface.
 *
 * @author Jacek Skowronek
 * @date   05/02/2021
 *
 */
/* =============================
 *   Includes of common headers
 * =============================*/
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
/* =============================
 *   Includes of project headers
 * =============================*/
#include "IDataProvider.h"
#include "ISocketDriver.h"
#include "IMainWindowWrapper.h"
/* =============================
 *           Defines
 * =============================*/

class DataProvider : public IDataProvider, public SocketListener
{

public:
   DataProvider(IMainWindowWrapper& main_window, ISocketDriver& driver);
   ~DataProvider();
private:
   /* IDataProvider */
   bool run (const std::string& ip_address, uint16_t port, char c) override;
   void stop() override;
   bool isConnected() override;

   /* SocketListener */
   void onSocketEvent(DriverEvent ev, const std::vector<uint8_t>& data, size_t size) override;

   void executeThread();
   void parse_message(const std::vector<uint8_t>& data, size_t size);
   bool parse_env_event(const std::vector<uint8_t>& data, size_t size);
   bool parse_input_event(const std::vector<uint8_t>& data, size_t size);
   bool parse_fan_event(const std::vector<uint8_t>& data, size_t size);

   IMainWindowWrapper& m_main_window;
   std::string m_server_address;
   uint16_t m_port;
   char m_delimiter;
   ISocketDriver& m_driver;
   std::atomic<bool> m_thread_running;
   std::thread m_thread;
   std::mutex m_mtx;
#if defined (DATA_PROVIDER_FRIEND_TESTS)
   DATA_PROVIDER_FRIEND_TESTS
#endif
};

#endif
