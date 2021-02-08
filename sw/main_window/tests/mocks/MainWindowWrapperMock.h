#ifndef _MAINWINDOWWRAPPERMOCK_H_
#define _MAINWINDOWWRAPPERMOCK_H_

#include "gmock/gmock.h"
#include "IMainWindowWrapper.h"


class MainWindowWrapperMock : public IMainWindowWrapper
{
public:
   MOCK_METHOD5(setEnvState, bool(ENV_ITEM_ID, int8_t, int8_t, uint8_t, uint8_t));
   MOCK_METHOD2(setInputState, bool(INPUT_ID, INPUT_STATE));
   MOCK_METHOD1(setFanState, bool(FAN_STATE));

};


#endif
