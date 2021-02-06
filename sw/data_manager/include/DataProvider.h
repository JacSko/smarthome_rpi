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
#include "IMainWindowWrapper.h"
/* =============================
 *           Defines
 * =============================*/

class DataProvider : public IDataProvider
{

public:
   DataProvider(IMainWindowWrapper& main_window);
   ~DataProvider();
private:
   /* IDataProvider */
   bool run (const std::string& ip_address, uint16_t port, char c);
   bool stop();

#if defined (DATA_PROVIDER_FRIEND_TESTS)
   DATA_PROVIDER_FRIEND_TESTS
#endif
};

#endif
