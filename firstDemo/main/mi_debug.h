/**************************************************************************
  * @file		: debug.h  
  * @brief		: 对文件的简单描述
  * @author		: esp32  
  * @copyright	: 版权信息
  * @version	: 版本
  * @note		: 注意事项
  * @data		: Mar 6, 2018
  * @history	: 历史记录
***************************************************************************/
#ifndef MAIN_INCLUDE_MI_DEBUG_H_
#define MAIN_INCLUDE_MI_DEBUG_H_

#if 1

#include "esp_log.h"

//#define MY_LOG_FORMAT(letter, format)  LOG_COLOR_ ## letter #letter __FILE__ " (%d) %s: " format LOG_RESET_COLOR "\n"

#define E( format, ... )  { esp_log_write(ESP_LOG_ERROR,   "Error", LOG_FORMAT(E, format), esp_log_timestamp(), 	"Error", 	##__VA_ARGS__); }
#define W( format, ... )  { esp_log_write(ESP_LOG_WARN,    "Warning",, LOG_FORMAT(W, format), esp_log_timestamp(), 	"Warning", 	##__VA_ARGS__); }
#define I( format, ... )  { esp_log_write(ESP_LOG_INFO,    "info", LOG_FORMAT(I, format), esp_log_timestamp(), 		"info", 	##__VA_ARGS__); }
#define D( format, ... )  { esp_log_write(ESP_LOG_DEBUG,   "Debug", LOG_FORMAT(D, format), esp_log_timestamp(), 	"Debug", 	##__VA_ARGS__); }
#define V( format, ... )  { esp_log_write(ESP_LOG_VERBOSE, "Verbose", LOG_FORMAT(V, format), esp_log_timestamp(), 	"Verbose", 	##__VA_ARGS__); }


#else


#define E(  format, ... )
#define W(  format, ... )
#define I(  format, ... )
#define D(  format, ... )
#define V(  format, ... )


#endif

#endif /* MAIN_INCLUDE_MI_DEBUG_H_ */
