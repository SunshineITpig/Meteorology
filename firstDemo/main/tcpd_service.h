/**************************************************************************
  * @file		: tcpd_service.h  
  * @brief		: 对文件的简单描述
  * @author		: esp32  
  * @copyright	: 版权信息
  * @version	: 版本
  * @note		: 注意事项
  * @data		: Mar 21, 2018
  * @history	: 历史记录
***************************************************************************/
#ifndef MAIN_SERVICES_INCLUDE_TCPD_SERVICE_H_
#define MAIN_SERVICES_INCLUDE_TCPD_SERVICE_H_

#define TCPD_TCP_PRIO		TCP_PRIO_MIN
#define TCPD_DEBUG			LWIP_DBG_ON
#define TCPD_SERVER_PORT	8004
/** The poll delay is X*500ms */
#define TCPD_POLL_INTERVAL                 4

enum tcpCmdList{
	TCP_RECV_CMD_RECV_SETTIME = 1,
	TCP_RECV_CMD_TO_SETTIME = 2,
	TCP_RECV_CMD_RECV_GETDATA = 3,
	TCP_RECV_CMD_TO_GETDATA = 4,
};

struct tcpDataRecv{
//	char head[2];  //TD
	int cmd;
	int len;
	int data;
};

struct tcpDataSend{
//	char head[2];  //TA
	int cmd;
	int len;
	int temper;
	int hum;
	int soilHum;
	int soiltemper;
};

extern volatile int setTimeData;

extern void tcpd_service_init();

#endif /* MAIN_SERVICES_INCLUDE_TCPD_SERVICE_H_ */
