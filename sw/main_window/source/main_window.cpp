#include "main_window.h"
#include "../../gui/ui_main_window.h"
#include "Logger.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(this, SIGNAL(requestEnvUpdate(ENV_ITEM_ID, int8_t, int8_t, uint8_t, uint8_t)),
                     this, SLOT(updateEnvState(ENV_ITEM_ID, int8_t, int8_t, uint8_t, uint8_t)));
    QObject::connect(this, SIGNAL(requestInputUpdate(INPUT_ID, INPUT_STATE)),
                     this, SLOT(updateInputState(INPUT_ID, INPUT_STATE)));
    QObject::connect(this, SIGNAL(requestFanUpdate(FAN_STATE)),
                     this, SLOT(updateFanState(FAN_STATE)));

    m_env_objects.push_back(EnvObject(ENV_BATHROOM, ui->sum_bath_temp, ui->sum_bath_hum));
    m_env_objects.push_back(EnvObject(ENV_BEDROOM, ui->sum_bed_temp, ui->sum_bed_hum));
    m_env_objects.push_back(EnvObject(ENV_WARDROBE, ui->sum_war_temp, ui->sum_war_hum));
    m_env_objects.push_back(EnvObject(ENV_KITCHEN, ui->sum_sal_temp, ui->sum_sal_hum));
    m_env_objects.push_back(EnvObject(ENV_STAIRS, ui->sum_stairs_temp, ui->sum_stairs_hum));
    m_env_objects.push_back(EnvObject(ENV_OUTSIDE, ui->sum_out_temp, ui->sum_out_hum));

    m_input_objects.push_back(InputObject(INPUT_WARDROBE_AC, ui->sum_war_light, Icons::LIGHT_ON, Icons::LIGHT_OFF));
    m_input_objects.push_back(InputObject(INPUT_WARDROBE_LED, ui->sum_war_led, Icons::LED_ON, Icons::LED_OFF));
    m_input_objects.push_back(InputObject(INPUT_BEDROOM_AC, ui->sum_bed_light, Icons::LIGHT_ON, Icons::LIGHT_OFF));
    m_input_objects.push_back(InputObject(INPUT_BATHROOM_AC, ui->sum_bath_light, Icons::LIGHT_ON, Icons::LIGHT_OFF));
    m_input_objects.push_back(InputObject(INPUT_BATHROOM_LED, ui->sum_bath_led, Icons::LED_ON, Icons::LED_OFF));
    m_input_objects.push_back(InputObject(INPUT_KITCHEN_AC, ui->light_kitchen_AC, Icons::LIGHT_ON, Icons::LIGHT_OFF));
    m_input_objects.push_back(InputObject(INPUT_KITCHEN_WALL, ui->light_kitchen_wall, Icons::LIGHT_ON, Icons::LIGHT_OFF));
    m_input_objects.push_back(InputObject(INPUT_STAIRS_AC, ui->sum_stairs_light, Icons::LIGHT_ON, Icons::LIGHT_OFF));
    m_input_objects.push_back(InputObject(INPUT_STAIRS_SENSOR, ui->sum_stairs_sensor, Icons::SENSOR_ON, Icons::SENSOR_OFF));
    m_input_objects.push_back(InputObject(INPUT_SOCKETS, ui->sum_stairs_sensor, Icons::POWER_ON, Icons::POWER_OFF));

    loadDefaults();
}

void MainWindow::loadDefaults()
{
   for (auto item : m_env_objects)
   {
      item.setData(0, 0, 0, 0);
   }
   for (auto item : m_input_objects)
   {
      item.setState(INPUT_STATE_INACTIVE);
   }

   ui->sum_time->setText(QString("xx:xx"));
   ui->sum_calendar->setText(QString("xx-xx-xxxx"));
   updateFanState(FAN_STATE_OFF);
}

void MainWindow::setEnvState (ENV_ITEM_ID id, int8_t temp_h, int8_t temp_l, uint8_t hum_h, uint8_t hum_l)
{
   emit requestEnvUpdate(id, temp_h, temp_l, hum_h, hum_l);
}
void MainWindow::setInputState(INPUT_ID id, INPUT_STATE state)
{
   emit requestInputUpdate(id, state);
}
void MainWindow::setFanState(FAN_STATE state)
{
   emit requestFanUpdate(state);
}
void MainWindow::updateEnvState(ENV_ITEM_ID id, int8_t temp_h, int8_t temp_l, uint8_t hum_h, uint8_t hum_l)
{
   auto it = std::find_if(m_env_objects.begin(), m_env_objects.end(), [&](EnvObject& obj){ return obj.m_id == id;});
   if (it != m_env_objects.end())
   {
      it->setData(temp_h, temp_l, hum_h, hum_l);
   }
   else
   {
      logger_send(LOG_ERROR, __func__, "not found %u", id);
   }
}
void MainWindow::updateInputState(INPUT_ID id, INPUT_STATE state)
{
   auto it = std::find_if(m_input_objects.begin(), m_input_objects.end(), [&](InputObject& obj){ return obj.m_id == id;});
   if (it != m_input_objects.end())
   {
      it->setState(state);
   }
   else
   {
      logger_send(LOG_ERROR, __func__, "not found %u", id);
   }
}
void MainWindow::updateFanState(FAN_STATE state)
{
   if (state == FAN_STATE_ON)
   {
      ui->sum_bath_fan->setStyleSheet(Icons::FAN_ON);
   }
   else
   {
      ui->sum_bath_fan->setStyleSheet(Icons::FAN_OFF);
   }
}
MainWindow::~MainWindow()
{
    delete ui;
}
