#ifndef _LOGGER_H_
#define _LOGGER_H_
/* ============================= */
/**
 * @file Logger.h
 *
 * @brief Allows to send debug traces to standard output.
 *
 * @details
 *    In current implementation, traces are sent to standard output. In future, logging to file should be implemented.
 *
 * TODO: logging to file
 * TODO: group handling - disable/enabled, etc
 *
 * @author Jacek Skowronek
 * @date 13/12/2020
 */
/* ============================= */

/* =============================
 *  Includes of common headers
 * =============================*/
#include <stdint.h>
/* =============================
 *  Includes of project headers
 * =============================*/
/* =============================
 *          Defines
 * =============================*/
#define LOGGER_GROUP_ENABLE  0x01
#define LOGGER_GROUP_DISABLE 0x00
/* =============================
 *       Data structures
 * =============================*/
enum LogGroup
{
   LOG_ERROR,         /**< Channel for error logs */
   LOG_SOCKDRV,       /**< Logs from socket driver */
   LOG_ENUM_MAX,
};

/**
 * @brief Initialize Logger module.
 * @return None.
 */
void logger_initialize();
/**
 * @brief Deinitialize Logger module.
 * @return None.
 */
void logger_deinitialize();
/**
 * @brief Sends log string.
 * @param[in] group - the group to which data is related
 * @param[in] prefix - short prefix added to log string
 * @param[in] fmt - printf-like format of string
 * @param[in] ... - list of arguments to format
 * @return None.
 */
void logger_send(LogGroup group, const char* prefix, const char* fmt, ...);
/**
 * @brief Sends log string conditionally.
 * @param[in] cond_bool - expression (log will be send if this is true.
 * @param[in] group - the group to which data is related
 * @param[in] prefix - short prefix added to log string
 * @param[in] fmt - printf-like format of string
 * @param[in] ... - list of arguments to format
 * @return None.
 */
void logger_send_if(uint8_t cond_bool, LogGroup group, const char* prefix, const char* fmt, ...);
/**
 * @brief Enable/Disable defined logger group.
 * @param[in] group - desired group
 * @param[in] state - group state(0-Disable, 1-Enable)
 * @return True on success, otherwise false.
 */
bool logger_set_group_state(LogGroup group, uint8_t state);
/**
 * @brief Get defined logger group state.
 * @param[in] group - desired group
 * @return Group state(0-Disable, 1-Enable).
 */
uint8_t logger_get_group_state(LogGroup group);


#endif
