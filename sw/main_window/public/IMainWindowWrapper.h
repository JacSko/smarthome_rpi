#ifndef _IMAINWINDOWWRAPPER_H_
#define _IMAINWINDOWWRAPPER_H_

/**
 * @file IMainWindowWrapper.h
 *
 * @brief
 *    Interface responsible for control MainWindow GUI.
 * @details
 *    Using this interface it is possible set current temperatures, light states and so on.
 *
 * @author Jacek Skowronek
 * @date   07/02/2021
 *
 */

/* =============================
 *   Includes of common headers
 * =============================*/
#include <string>
/* =============================
 *   Includes of project headers
 * =============================*/
#include "env_types.h"
#include "inputs_types.h"
#include "fan_types.h"

class IMainWindowWrapper
{
public:
   /**
    * @brief Set temperature and humidity.
    * @param[in] id - id of the entity.
    * @param[in] temp_h - decimal part of temperature.
    * @param[in] temp_l - fraction part of temperature.
    * @param[in] hum_h - decimal part of humidity.
    * @param[in] hum_l - fraction part of temperature.
    * @return True if set successfully, otherwise false.
    */
   virtual bool setEnvState (ENV_ITEM_ID id, int8_t temp_h, int8_t temp_l, uint8_t hum_h, uint8_t hum_l) = 0;
   /**
    * @brief Set input state.
    * @param[in] id - id of the entity.
    * @param[in] state - state of id.
    * @return True if set successfully, otherwise false.
    */
   virtual bool setInputState(INPUT_ID id, INPUT_STATE state) = 0;
   /**
    * @brief Set fan state.
    * @param[in] state - fan state.
    * @return True if set successfully, otherwise false.
    */
   virtual bool setFanState(FAN_STATE state) = 0;

   virtual ~IMainWindowWrapper(){};
};


#endif
