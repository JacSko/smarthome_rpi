#ifndef _ISOCKETDRIVER_H_
#define _ISOCKETDRIVER_H_

/**
 * @file ISocketDriver.h
 *
 * @brief
 *    Interface to control Socket Driver - it is responsible for writing/reading data to/from socket.
 *
 * @details
 *    Data can be write to socket using write() method. Received data is provided through registered listeners.
 *    IP address cannot be empty. It is possible to provide either raw IP address and hostname. Port should be different than 0.
 *
 * @author Jacek Skowronek
 * @date   05/02/2021
 *
 */

/* =============================
 *   Includes of common headers
 * =============================*/
#include "stdint.h"
#include <string>
#include <vector>
#include <functional>

enum class DriverEvent
{
   DRIVER_CONNECTED,    /**< Driver connects successfully to server */
   DRIVER_DISCONNECTED, /**< Driver disconnected - server closed or error appears */
   DRIVER_DATA_RECV,    /**< New data received by driver */
};

class SocketListener
{
public:
   virtual ~SocketListener(){};
   /**
    * @brief Callback called when new data arrives.
    * @param[in] data - Reference to vector with new data.
    * @param[in] size - Size of the data.
    * @return None.
    */
   virtual void onSocketEvent(DriverEvent ev, const std::vector<uint8_t>& data, size_t size) = 0;
};

class ISocketDriver
{
public:

   /**
    * @brief Connect to server.
    * @param[in] ip_address - Address of the server (see @details).
    * @param[in] port - connection port (see @details).
    * @return True if connected successfully, otherwise false.
    */
   virtual bool connect(const std::string& ip_address, uint16_t port) = 0;
   /**
    * @brief Disconnect from server.
    * @return True if disconnected successfully, otherwise false.
    */
   virtual bool disconnect() = 0;
   /**
    * @brief Returns current connection status.
    * @return True if connected to server, otherwise false.
    */
   virtual bool isConnected() = 0;
   /**
    * @brief Adds listener to receive new data.
    * @return None.
    */
   virtual void addListener(SocketListener* callback) = 0;
   /**
    * @brief Removes listener.
    * @return None.
    */
   virtual void removeListener(SocketListener* callback) = 0;
   /**
    * @brief Writes data to socket.
    * @param[in] data - bytes to write.
    * @param[in] size - number of bytes to write - if not provided, data.size() bytes will be written.
    */
   virtual bool write(const std::vector<uint8_t>& data, size_t size = 0) = 0;
   /**
    * @brief Set data delimiter.
    * @param[in] c - delimiter char.
    */
   virtual void setDelimiter(char c) = 0;
   virtual ~ISocketDriver(){};
};


#endif
