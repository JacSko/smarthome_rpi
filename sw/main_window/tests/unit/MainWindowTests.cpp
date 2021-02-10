#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "QtWidgets/QApplication"
#include "main_window.h"
#include "../../../gui/ui_main_window.h"
#include "logger_mock.hpp"
/* ============================= */
/**
 * @file MainWindowTests.cpp
 *
 * @brief Unit tests to verify behavior of MainWindow.
 *
 * @author Jacek Skowronek
 * @date 08/02/2021
 */
/* ============================= */

using namespace testing;

struct MainWindowFixture : public testing::Test
{
   MainWindowFixture():
   app(fake_argc, nullptr)
   {

   }
   void SetUp()
   {
      mock_logger_init();
      m_test_subject.reset(new MainWindow(nullptr));
      m_test_subject->show();
   }
   void TearDown()
   {
      m_test_subject.reset(nullptr);
      mock_logger_deinit();
   }
   int fake_argc = 0;
   QApplication app;
   std::unique_ptr<MainWindow> m_test_subject;
};

TEST_F(MainWindowFixture, set_env_state_tests)
{
   /**
    * <b>scenario</b>: Set data for ENV_BATHROOM.<br>
    * <b>expected</b>: Data presented on GUI updated.<br>
    * ************************************************
    */
   ASSERT_THAT(m_test_subject->ui->sum_bath_temp->text().toUtf8(), HasSubstr("0.0"));
   ASSERT_THAT(m_test_subject->ui->sum_bath_hum->text().toUtf8(), HasSubstr("0.0"));

   m_test_subject->setEnvState(ENV_BATHROOM, 24, 1, 45, 5);

   EXPECT_THAT(m_test_subject->ui->sum_bath_temp->text().toUtf8(), HasSubstr("24.1"));
   EXPECT_THAT(m_test_subject->ui->sum_bath_hum->text().toUtf8(), HasSubstr("45.5"));
}

TEST_F(MainWindowFixture, set_input_state_tests)
{
   /**
    * <b>scenario</b>: Set BATHROOM_AC to STATE_ACTIVE.<br>
    * <b>expected</b>: Data presented on GUI updated.<br>
    * ************************************************
    */
   ASSERT_STREQ(m_test_subject->ui->sum_bath_light->styleSheet().toUtf8(), Icons::LIGHT_OFF.toUtf8());
   m_test_subject->setInputState(INPUT_BATHROOM_AC, INPUT_STATE_ACTIVE);
   EXPECT_STREQ(m_test_subject->ui->sum_bath_light->styleSheet().toUtf8(), Icons::LIGHT_ON.toUtf8());
   /**
    * <b>scenario</b>: Set BATHROOM_AC to STATE_INACTIVE.<br>
    * <b>expected</b>: Data presented on GUI updated.<br>
    * ************************************************
    */
   m_test_subject->setInputState(INPUT_BATHROOM_AC, INPUT_STATE_INACTIVE);
   EXPECT_STREQ(m_test_subject->ui->sum_bath_light->styleSheet().toUtf8(), Icons::LIGHT_OFF.toUtf8());

   /**
    * <b>scenario</b>: Set BATHROOM_LED to STATE_ACTIVE.<br>
    * <b>expected</b>: Data presented on GUI updated.<br>
    * ************************************************
    */
   ASSERT_STREQ(m_test_subject->ui->sum_bath_led->styleSheet().toUtf8(), Icons::LED_OFF.toUtf8());
   m_test_subject->setInputState(INPUT_BATHROOM_LED, INPUT_STATE_ACTIVE);
   EXPECT_STREQ(m_test_subject->ui->sum_bath_led->styleSheet().toUtf8(), Icons::LED_ON.toUtf8());
   /**
    * <b>scenario</b>: Set BATHROOM_LED to STATE_INACTIVE.<br>
    * <b>expected</b>: Data presented on GUI updated.<br>
    * ************************************************
    */
   m_test_subject->setInputState(INPUT_BATHROOM_LED, INPUT_STATE_INACTIVE);
   EXPECT_STREQ(m_test_subject->ui->sum_bath_led->styleSheet().toUtf8(), Icons::LED_OFF.toUtf8());
}

TEST_F(MainWindowFixture, set_fan_state_tests)
{
   /**
    * <b>scenario</b>: Set FAN_STATE_ON.<br>
    * <b>expected</b>: Data presented on GUI updated.<br>
    * ************************************************
    */
   ASSERT_STREQ(m_test_subject->ui->sum_bath_fan->styleSheet().toUtf8(), Icons::FAN_OFF.toUtf8());
   m_test_subject->setFanState(FAN_STATE_ON);
   EXPECT_STREQ(m_test_subject->ui->sum_bath_fan->styleSheet().toUtf8(), Icons::FAN_ON.toUtf8());
   /**
    * <b>scenario</b>: Set FAN_STATE_OFF.<br>
    * <b>expected</b>: Data presented on GUI updated.<br>
    * ************************************************
    */
   m_test_subject->setFanState(FAN_STATE_SUSPEND);
   EXPECT_STREQ(m_test_subject->ui->sum_bath_fan->styleSheet().toUtf8(), Icons::FAN_OFF.toUtf8());
}
