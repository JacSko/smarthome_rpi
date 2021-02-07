#ifndef _SOCKETDRIVER_H_
#define _SOCKETDRIVER_H_

/**
 * @file SocketDriver.h
 *
 * @brief
 *    Implementation of ISocketDriver interface.
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
#include "ISocketDriver.h"
/* =============================
 *           Defines
 * =============================*/
#define SOCKDRV_MAX_RW_SIZE 1024
#define SOCKDRV_RECV_BUFFER_SIZE 1024

class SocketDriver : public ISocketDriver
{
public:
   SocketDriver();
   ~SocketDriver();
private:
   /* ISocketDriver */
   bool connect(const std::string& ip_address, uint16_t port) override;
   bool disconnect() override;
   bool isConnected() override;
   void addListener(SocketListener* callback) override;
   void removeListener(SocketListener* callback) override;
   bool write(const std::vector<uint8_t>& data, size_t size = 0) override;
   void setDelimiter(char c);
   void threadExecute();
   void notify_callbacks(DriverEvent ev, const std::vector<uint8_t>& data, size_t count);

   std::string m_server_address;
   uint16_t m_server_port;
   std::atomic<bool> m_is_connected;
   char m_delimiter;
   std::vector<uint8_t> m_recv_buffer;
   size_t m_recv_buffer_size;
   std::thread m_thread;
   std::atomic<bool> m_thread_running;
   std::mutex m_mutex;
   int m_sock_fd;
   std::vector<SocketListener*> m_listeners;
#if defined (SOCKDRV_FRIEND_TESTS)
   SOCKDRV_FRIEND_TESTS
#endif
};

#endif
