#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <QtWidgets/QMainWindow>
#include "QtWidgets/QLabel"
#include "QtWidgets/QPushButton"
#include "IMainWindowWrapper.h"
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


namespace Icons {

    static QString LIGHT_OFF = "border-image: url(:/bitmaps/bulb_off.svg);";
    static QString LIGHT_ON = "border-image: url(:/bitmaps/bulb_on.svg);";
    static QString FAN_OFF = "border-image: url(:/bitmaps/fan_off.svg);";
    static QString FAN_ON = "border-image: url(:/bitmaps/fan_on.svg);";
    static QString LED_OFF = "border-image: url(:/bitmaps/led_off.svg);";
    static QString LED_ON = "border-image: url(:/bitmaps/led_on.svg);";
    static QString THERMOMETER = "border-image: url(:/bitmaps/thermometer.svg);";
    static QString HUMIDITY = "border-image: url(:/bitmaps/humidity.svg);";
    static QString CONN_ON = "border-image: url(:/bitmaps/connection_on.svg);";
    static QString CONN_OFF = "border-image: url(:/bitmaps/connection_off.svg);";
    static QString POWER_ON = "border-image: url(:/bitmaps/electricity_on.svg);";
    static QString POWER_OFF = "border-image: url(:/bitmaps/electricity_off.svg);";
    static QString WATER_ON = "border-image: url(:/bitmaps/water_on.svg);";
    static QString WATER_OFF = "border-image: url(:/bitmaps/water_off.svg);";
    static QString SENSOR_ON = "border-image: url(:/bitmaps/move_sensor_on.svg);";
    static QString SENSOR_OFF = "border-image: url(:/bitmaps/move_sensor_off.svg);";
    static QString CLOCK = "border-image: url(:/bitmaps/clock.svg);";
    static QString CALENDAR = "border-image: url(:/bitmaps/calendar.svg);";

}

class MainWindow : public QMainWindow, public IMainWindowWrapper
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
#ifndef UNIT_TESTS
private:
#endif
    Ui::MainWindow *ui;

    struct EnvObject
    {
       EnvObject(ENV_ITEM_ID id, QLabel* temp_label, QLabel* hum_label):
          m_id(id),
          m_temp_label(temp_label),
          m_hum_label(hum_label)
       {

       }
       void setData(int8_t temp_h, int8_t temp_l, uint8_t hum_h, uint8_t hum_l)
       {
          if (m_temp_label && m_hum_label)
          {
             m_temp_label->setText(QString("%1.%2°C").arg(QString::number(temp_h), QString::number(temp_l)));
             m_hum_label->setText(QString("%1.%2%").arg(QString::number(hum_h), QString::number(hum_l)));
          }
       }
       ENV_ITEM_ID m_id;
       QLabel* m_temp_label;
       QLabel* m_hum_label;
    };

    struct InputObject
    {
       InputObject(INPUT_ID id, QPushButton* btn, QString& active_ss, QString& inactive_ss):
       m_id(id),
       active_style_sheet(active_ss),
       inactive_style_sheet(inactive_ss),
       m_button(btn)
       {
       }
       void setState(INPUT_STATE state)
       {
          if (m_button)
          {
             m_button->setStyleSheet(state == INPUT_STATE_ACTIVE? active_style_sheet : inactive_style_sheet);
          }
       }
       INPUT_ID m_id;
       QString active_style_sheet;
       QString inactive_style_sheet;
       QPushButton* m_button;
    };

    std::vector<EnvObject> m_env_objects;
    std::vector<InputObject> m_input_objects;
    void loadDefaults();
    void setEnvState (ENV_ITEM_ID id, int8_t temp_h, int8_t temp_l, uint8_t hum_h, uint8_t hum_l);
    void setInputState(INPUT_ID id, INPUT_STATE state);
    void setFanState(FAN_STATE state);




signals:
   void requestEnvUpdate(ENV_ITEM_ID id, int8_t temp_h, int8_t temp_l, uint8_t hum_h, uint8_t hum_l);
   void requestInputUpdate(INPUT_ID id, INPUT_STATE state);
   void requestFanUpdate(FAN_STATE state);

public slots:
   void updateEnvState(ENV_ITEM_ID id, int8_t temp_h, int8_t temp_l, uint8_t hum_h, uint8_t hum_l);
   void updateInputState(INPUT_ID id, INPUT_STATE state);
   void updateFanState(FAN_STATE state);

};
#endif
