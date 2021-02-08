#include "main_window.h"
#include "../../gui/ui_main_window.h"

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

}
void MainWindow::updateInputState(INPUT_ID id, INPUT_STATE state)
{

}
void MainWindow::updateFanState(FAN_STATE state)
{

}
MainWindow::~MainWindow()
{
    delete ui;
}
