/* =============================
 *   Includes of project headers
 * =============================*/
#include "SocketDriver.h"
/* =============================
 *   Includes of common headers
 * =============================*/
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>


/* namespace wrapper around system function to allow replace in unit tests */
namespace system_call
{
__attribute__((weak)) int connect(int socket, const struct sockaddr *address, socklen_t address_len)
{
   return ::connect(socket, address, address_len);
}
__attribute__((weak)) ssize_t recv(int socket, void *buffer, size_t length, int flags)
{
   return ::recv(socket, buffer, length, flags);
}
__attribute__((weak)) ssize_t send(int socket, const void *message, size_t length, int flags)
{
   return ::send(socket, message, length, flags);
}
__attribute__((weak)) int socket(int domain, int type, int protocol)
{
   return ::socket(domain, type, protocol);
}
__attribute__((weak)) int close (int fd)
{
   return ::close(fd);
}

}


SocketDriver::SocketDriver() :
m_server_address(""),
m_server_port(0),
m_is_connected(false),
m_delimiter('\n'),
m_recv_buffer(SOCKDRV_RECV_BUFFER_SIZE, 0),
m_recv_buffer_size(0),
m_thread_running(false),
m_sock_fd(0)
{
}
bool SocketDriver::connect(const std::string& ip_address, uint16_t port)
{
   bool result = false;

   do
   {
      if (ip_address.empty() || port == 0)
      {
         break;
      }
      m_sock_fd = system_call::socket(AF_INET, SOCK_STREAM, 0);
      if (m_sock_fd < 0)
      {
         break;
      }

      struct sockaddr_in serv_addr;
      serv_addr.sin_family = AF_INET;
      serv_addr.sin_port = htons(port);

      if(inet_pton(AF_INET, ip_address.c_str(), &serv_addr.sin_addr)<=0)
      {
         std::cout << "wrong ip\n";
          break;
      }

      if(system_call::connect(m_sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) >= 0)
      {
         m_server_address = ip_address;
         m_server_port = port;
         m_thread = std::thread(&SocketDriver::threadExecute, this);
         m_is_connected = true;
         notify_callbacks(DriverEvent::DRIVER_CONNECTED, {}, 0);
         while(!m_thread_running);
         result = true;
      }

   }while(0);

   if (!result)
   {
      disconnect();
   }

   return result;
}

void SocketDriver::threadExecute()
{
   m_recv_buffer_size = 0;
   int bytes_count = 0;
   m_thread_running = true;
   while(m_thread_running)
   {
      bytes_count = system_call::recv(m_sock_fd, &m_recv_buffer[0] + m_recv_buffer_size, SOCKDRV_RECV_BUFFER_SIZE, 0);
      if (bytes_count > 0)
      {
         m_recv_buffer_size += bytes_count;
         auto it = std::find(m_recv_buffer.begin(), (m_recv_buffer.begin() + m_recv_buffer_size), (uint8_t)m_delimiter);
         if (it != (m_recv_buffer.begin() + m_recv_buffer_size))
         {
            notify_callbacks(DriverEvent::DRIVER_DATA_RECV, std::vector<uint8_t>(m_recv_buffer.begin(), it), std::distance(m_recv_buffer.begin(), it));
            std::copy(it + 1, m_recv_buffer.begin() + m_recv_buffer_size, m_recv_buffer.begin());
            m_recv_buffer_size = std::distance(it + 1, m_recv_buffer.begin() + m_recv_buffer_size);
         }
      }
      else
      {
         notify_callbacks(DriverEvent::DRIVER_DISCONNECTED, {}, 0);
         m_thread_running = false;
         system_call::close(m_sock_fd);
         m_is_connected = false;
      }
   }
}
void SocketDriver::notify_callbacks(DriverEvent ev, const std::vector<uint8_t>& data, size_t count)
{
   std::lock_guard<std::mutex> lock (m_mutex);
   for (auto& l : m_listeners)
   {
      l->onSocketEvent(ev, data, count);
   }
}
bool SocketDriver::disconnect()
{
   bool result = false;
   if (m_thread.joinable())
   {
      m_thread_running = false;
      m_thread.join();
   }
   if (m_sock_fd > 0)
   {
      system_call::close(m_sock_fd);
      m_sock_fd = 0;
   }
   m_is_connected = false;
   return result;
}
bool SocketDriver::isConnected()
{
   return m_is_connected;
}
void SocketDriver::addListener(SocketListener* callback)
{
   std::lock_guard<std::mutex> lock (m_mutex);
   m_listeners.push_back(callback);
}
void SocketDriver::removeListener(SocketListener* callback)
{
   std::lock_guard<std::mutex> lock (m_mutex);
   std::remove_if(m_listeners.begin(), m_listeners.end(), [&](SocketListener* list){ return list == callback;});
}
bool SocketDriver::write(const std::vector<uint8_t>& data, size_t size)
{
   bool result = false;
   ssize_t bytes_to_write = size == 0? data.size() : size;
   if (bytes_to_write <= SOCKDRV_MAX_RW_SIZE)
   {
      ssize_t bytes_written = 0;
      ssize_t current_write = 0;
      result = true;
      while (bytes_to_write > 0)
      {
         //TODO prepare for error here (send not succeeds)
         current_write = system_call::send(m_sock_fd, data.data() + bytes_written, bytes_to_write, 0);
         if (current_write > 0)
         {
            bytes_written += current_write;
         }
         else
         {
            result = false;
            break;
         }
         bytes_to_write -= bytes_written;
      }
   }
   return result;
}
void SocketDriver::setDelimiter(char c)
{
   std::lock_guard<std::mutex> lock (m_mutex);
   m_delimiter = c;
}
SocketDriver::~SocketDriver()
{
   disconnect();
}
