/* =============================
 *   Includes of project headers
 * =============================*/
#include "DataProvider.h"
#include "SocketDriver.h"
#include "Logger.h"
#include "notification_types.h"
#include "env_types.h"
/* =============================
 *   Includes of common headers
 * =============================*/

/* period between next connection attempts */
const uint16_t DRV_CONN_RETRY_PERIOD = 10000;

DataProvider::DataProvider(IMainWindowWrapper& main_window) :
m_main_window(main_window),
m_driver(new SocketDriver()),
m_thread_running(false),
m_port(0)
{
}

bool DataProvider::run(const std::string& ip_address, uint16_t port, char c)
{
   bool result = false;

   if (!m_thread_running)
   {
      logger_send(LOG_DATAPROV, __func__, "starting");
      m_server_address = ip_address;
      m_port = port;
      m_delimiter = c;
      m_driver->setDelimiter(c);
      m_driver->addListener(this);
      m_thread = std::thread(&DataProvider::executeThread, this);
      while(!m_thread_running);
      result = true;
   }

   return result;
}

void DataProvider::executeThread()
{
   m_thread_running = true;

   while(m_thread_running)
   {
      if (!m_driver->isConnected())
      {
         if (m_driver->connect(m_server_address, m_port))
         {
            logger_send(LOG_DATAPROV, __func__, "connected");
         }
         else
         {
            logger_send(LOG_DATAPROV, __func__, "retry");
         }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(DRV_CONN_RETRY_PERIOD));
   }

   return;
}
void DataProvider::onSocketEvent(DriverEvent ev, const std::vector<uint8_t>& data, size_t size)
{
   switch(ev)
   {
   case DriverEvent::DRIVER_CONNECTED:
      logger_send(LOG_DATAPROV, __func__, "sockdrv connected ev");
      break;
   case DriverEvent::DRIVER_DISCONNECTED:
      logger_send(LOG_DATAPROV, __func__, "sockdrv disconnected ev");
      break;
   case DriverEvent::DRIVER_DATA_RECV:
      parse_message(data, size);
      break;
   }
}
void DataProvider::parse_message(const std::vector<uint8_t>& data, size_t size)
{
   if (data.size() >= size)
   {
      const uint8_t exp_payload_size = data[NTF_GROUP_BYTES_COUNT_OFFSET];
      const uint8_t recv_payload_size = size - NTF_HEADER_SIZE;
      if (exp_payload_size == recv_payload_size)
      {
         switch ((NTF_GROUP)data[NTF_GROUP_OFFSET])
         {
         case NTF_GROUP_INPUTS:
            parse_input_event(data, size);
            break;
         case NTF_GROUP_ENV:
            parse_env_event(data, size);
            break;
         case NTF_GROUP_FAN:
            parse_fan_event(data, size);
            break;
         default:
            break;
         }
      }
      else
      {
         logger_send(LOG_ERROR, __func__, "invalid payload size, e: %u, r: %u", exp_payload_size, recv_payload_size);
      }
   }
}
bool DataProvider::parse_env_event(const std::vector<uint8_t>& data, size_t size)
{
   bool result = false;
   logger_send(LOG_DATAPROV, __func__, "got env event");
   if ((NTF_REQ_TYPE)data[NTF_GROUP_REQ_TYPE_OFFSET] == NTF_NTF &&
       (NTF_ENV_SUBCMDS)data[NTF_GROUP_SUBCMD_OFFSET] == NTF_ENV_SENSOR_DATA)
   {
      ENV_ITEM_ID id = (ENV_ITEM_ID)data[NTF_HEADER_SIZE];
      uint8_t hum_h = data[NTF_HEADER_SIZE + 2];
      uint8_t hum_l = data[NTF_HEADER_SIZE + 3];
      int8_t temp_h = data[NTF_HEADER_SIZE + 4];
      int8_t temp_l = data[NTF_HEADER_SIZE + 5];
      logger_send(LOG_DATAPROV, __func__, "env id %u, t:%u.%u, h %u.%u", (uint8_t)id, temp_h, temp_l, hum_h, hum_l);
      m_main_window.setEnvState(id, temp_h, temp_l, hum_h, hum_l);
      result = true;
   }
   return result;
}
bool DataProvider::parse_input_event(const std::vector<uint8_t>& data, size_t size)
{
   bool result = false;
   logger_send(LOG_DATAPROV, __func__, "got env event");
   if ((NTF_REQ_TYPE)data[NTF_GROUP_REQ_TYPE_OFFSET] == NTF_NTF &&
       (NTF_ENV_SUBCMDS)data[NTF_GROUP_SUBCMD_OFFSET] == NTF_INPUTS_STATE)
   {
      INPUT_ID id = (INPUT_ID) data[NTF_HEADER_SIZE];
      INPUT_STATE state = (INPUT_STATE) data[NTF_HEADER_SIZE + 1];
      logger_send(LOG_DATAPROV, __func__, "inp id %u, state %u", id, state);
      m_main_window.setInputState(id, state);
      result = true;
   }
   return result;
}
bool DataProvider::parse_fan_event(const std::vector<uint8_t>& data, size_t size)
{
   bool result = false;
   logger_send(LOG_DATAPROV, __func__, "fan ev recevied");
   if ((NTF_REQ_TYPE)data[NTF_GROUP_REQ_TYPE_OFFSET] == NTF_NTF &&
       (NTF_ENV_SUBCMDS)data[NTF_GROUP_SUBCMD_OFFSET] == NTF_FAN_STATE)
   {
      FAN_STATE state = (FAN_STATE) data[NTF_HEADER_SIZE];
      logger_send(LOG_DATAPROV, __func__, "fan state %u", state);
      m_main_window.setFanState(state);
      result = true;
   }
   return result;
}
bool DataProvider::stop()
{
   return disconnect_driver();
}
bool DataProvider::isConnected()
{
   return m_driver->isConnected();
}
bool DataProvider::disconnect_driver()
{
   bool result = false;
   logger_send(LOG_DATAPROV, __func__, "disconnecting");
   if (m_driver->isConnected())
   {
      m_driver->disconnect();
      m_driver->removeListener(this);
      result = true;
   }

   return result;
}
DataProvider::~DataProvider()
{
   if (m_thread_running)
   {
      m_thread_running = false;
      if (m_thread.joinable())
      {
         m_thread.join();
      }
   }
   disconnect_driver();
}
