#ifndef _LOGGER_MOCK_H_
#define _LOGGER_MOCK_H_


#include <stdarg.h>
#include <chrono>
#include "Logger.h"
#include "gmock/gmock.h"

typedef struct LOG_GROUP
{
   uint8_t state;
   LogGroup id;
   const char* name;
} LOG_GROUP;
LOG_GROUP LOGGER_GROUPS[LOG_ENUM_MAX] = {
      {LOGGER_GROUP_ENABLE, LOG_ERROR, "ERROR"},
      {LOGGER_GROUP_ENABLE, LOG_SOCKDRV, "SOCKDRV"},
      {LOGGER_GROUP_ENABLE, LOG_DATAPROV, "DATAPROV"}};

struct loggerMock
{
   MOCK_METHOD0(logger_initialize, void());
   MOCK_METHOD2(logger_set_group_state, bool(LogGroup, uint8_t));
   MOCK_METHOD1(logger_get_group_state, uint8_t(LogGroup));
   MOCK_METHOD1(logger_send, void(LogGroup));
   MOCK_METHOD2(logger_send_if, void(uint8_t, LogGroup));
};

::testing::NiceMock<loggerMock>* logger_mock;

void mock_logger_init()
{
   logger_mock = new ::testing::NiceMock<loggerMock>;
}

void mock_logger_deinit()
{
   delete logger_mock;
}

void logger_initialize()
{
   logger_mock->logger_initialize();
}
bool logger_set_group_state(LogGroup group, uint8_t state)
{
   return logger_mock->logger_set_group_state(group, state);
}

uint8_t logger_get_group_state(LogGroup group)
{
   return logger_mock->logger_get_group_state(group);
}

void logger_send(LogGroup group, const char* prefix, const char* fmt, ...)
{
#ifndef LOGGER_DISABLE_OUTPUT
   va_list va;
   {
      std::vector<char> m_logger_buffer(1024, 0);
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
      printf("%s", m_logger_buffer.data());
   }
#endif
   logger_mock->logger_send(group);
}

void logger_send_if(uint8_t cond_bool, LogGroup group, const char* prefix, const char* fmt, ...)
{
   logger_mock->logger_send_if(cond_bool, group);
}

const char* logger_group_to_string(LogGroup group)
{
   const char* result;
   if (group < LOG_ENUM_MAX)
   {
      result = LOGGER_GROUPS[group].name;
   }
   return result;
}
LogGroup logger_string_to_group(const char* name)
{
   LogGroup result = LOG_ENUM_MAX;
   if (name)
   {
      for (uint8_t i = 0; i < LOG_ENUM_MAX; i++)
      {
         if (!strcmp(LOGGER_GROUPS[i].name, name))
         {
            result = LOGGER_GROUPS[i].id;
         }
      }
   }
   return result;
}

#endif
