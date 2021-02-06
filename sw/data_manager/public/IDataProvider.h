#ifndef _IDATAPROVIDER_H_
#define _IDATAPROVIDER_H_

/**
 * @file IDataProvider.h
 *
 * @brief
 *    Interface responsible for receiving, parsing and forwarding parsed data to upper layer.
 *
 * @details
 *    Module is parsing the recevied messages from socket using SmartHomeTypes which are common for both applications (sender and receiver).
 *    After calling run() method, module keeps connecting to server since it is available.
 *    The MainWindowControl have to be passed during construction, to allow updating GUI.
 *
 * @author Jacek Skowronek
 * @date   05/02/2021
 *
 */

/* =============================
 *   Includes of common headers
 * =============================*/
#include <string>


class IDataProvider
{
public:

   /**
    * @brief Starts execution of DataProvider.
    * @param[in] ip_address - Address of the remote server.
    * @param[in] port - connection port.
    * @param[in] c - message delimiter char.
    * @return True if connected successfully, otherwise false.
    */
   virtual bool run (const std::string& ip_address, uint16_t port, char c) = 0;
   /**
    * @brief Stops execution of DataProvider.
    * @return True if stopped, otherwise false.
    */
   virtual bool stop() = 0;
   /**
    * @brief Returns current status.
    * @return True if module is running, otherwise false.
    */
   virtual bool isRunning() = 0;

   virtual ~IDataProvider(){};
};


#endif
