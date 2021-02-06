/* =============================
 *   Includes of common headers
 * =============================*/
#include <stdlib.h>
#include <stdarg.h>
#include <string>
#include <iostream>
#include <vector>
#include <chrono>
/* =============================
 *  Includes of project headers
 * =============================*/
#include "Logger.h"
/* =============================
 *          Defines
 * =============================*/
#define LOGGER_BUFFER_SIZE 1024
/* =============================
 *   Internal module functions
 * =============================*/
void logger_notify_data();
/* =============================
 *       Internal types
 * =============================*/
typedef struct LOG_GROUP
{
   uint8_t state;
   LogGroup id;
   const char* name;
} LOG_GROUP;
/* =============================
 *      Module variables
 * =============================*/
std::vector<char> m_logger_buffer;
LOG_GROUP LOGGER_GROUPS[LOG_ENUM_MAX] = {
      {LOGGER_GROUP_ENABLE, LOG_ERROR,   "ERROR"   },
      {LOGGER_GROUP_ENABLE, LOG_SOCKDRV, "SOCKDRV" } };

void logger_initialize()
{
   m_logger_buffer.resize(LOGGER_BUFFER_SIZE);
}

void logger_deinitialize()
{
   m_logger_buffer.clear();
}
bool logger_set_group_state(LogGroup group, uint8_t state)
{
   bool result = false;

   if (group < LOG_ENUM_MAX)
   {
      if (LOGGER_GROUPS[group].state != state)
      {
         LOGGER_GROUPS[group].state = state;
         result = true;
      }
   }
   return result;
}
uint8_t logger_get_group_state(LogGroup group)
{
   uint8_t result = 255;
   if (group < LOG_ENUM_MAX)
   {
      result = LOGGER_GROUPS[group].state;
   }
   return result;
}
void logger_notify_data()
{
   std::cout << std::string(m_logger_buffer.data());
}
void logger_send(LogGroup group, const char* prefix, const char* fmt, ...)
{
   if (group < LOG_ENUM_MAX)
   {
      if (LOGGER_GROUPS[group].state == LOGGER_GROUP_ENABLE)
      {
         va_list va;
         {
            auto currentTime = std::chrono::system_clock::now();
            auto millis = (currentTime.time_since_epoch().count() / 1000000) % 1000;
            std::time_t tt = std::chrono::system_clock::to_time_t ( currentTime );
            auto timeinfo = localtime (&tt);
            int idx = strftime (m_logger_buffer.data(),80,"[%F %H:%M:%S",timeinfo);
            idx = sprintf(m_logger_buffer.data(), "%s:%03d] %s - ",m_logger_buffer.data(),(int)millis, prefix);
            va_start(va, fmt);
            {
                idx += vsprintf(m_logger_buffer.data() + idx, fmt, va);
            }
            va_end(va);
            m_logger_buffer[idx++] = '\n';
            m_logger_buffer[idx] = 0x00;
            logger_notify_data();
         }
      }
   }
}
void logger_send_if(uint8_t cond_bool, LogGroup group, const char* prefix, const char* fmt, ...)
{
   if (cond_bool != 0 && group < LOG_ENUM_MAX)
   {
      if (LOGGER_GROUPS[group].state == LOGGER_GROUP_ENABLE)
      {
         va_list va;
         {
            auto currentTime = std::chrono::system_clock::now();
            auto millis = (currentTime.time_since_epoch().count() / 1000000) % 1000;
            std::time_t tt = std::chrono::system_clock::to_time_t ( currentTime );
            auto timeinfo = localtime (&tt);
            int idx = strftime (m_logger_buffer.data(),80,"[%F %H:%M:%S",timeinfo);
            idx = sprintf(m_logger_buffer.data(), "%s:%03d] %s - ",m_logger_buffer.data(),(int)millis, prefix);
            va_start(va, fmt);
            {
               idx = vsprintf(m_logger_buffer.data() + idx, fmt, va);
            }
            va_end(va);
            m_logger_buffer[idx++] = '\n';
            m_logger_buffer[idx] = 0x00;
            logger_notify_data();
         }
      }
   }
}
