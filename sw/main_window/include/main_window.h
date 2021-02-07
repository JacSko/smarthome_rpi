#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <QtWidgets/QMainWindow>
#include "IMainWindowWrapper.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow, public IMainWindowWrapper
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    bool setEnvState (ENV_ITEM_ID id, int8_t temp_h, int8_t temp_l, uint8_t hum_h, uint8_t hum_l);
    bool setInputState(INPUT_ID id, INPUT_STATE state);
    bool setFanState(FAN_STATE state);

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
