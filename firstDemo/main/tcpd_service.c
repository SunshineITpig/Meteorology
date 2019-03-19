/**************************************************************************
 * @file		: tcpd_service.c
 * @brief		: 对文件的简单描述
 * @author		: esp32
 * @copyright	: 版权信息
 * @version	: 版本
 * @note		: 注意事项
 * @data		: Mar 21, 2018
 * @history	: 历史记录
 ***************************************************************************/

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */
#include "esp_log.h"
#include "debug.h"

#include "lwip/tcp.h"
#include "lwip/err.h"

#include "tcpd_service.h"
#include "mi_debug.h"
#include "i2c_main.h"
/* USER CODE END Includes */

volatile int setTimeData = 5;
//volatile float temperature = 0;
//volatile float humidity = 0;
//volatile float soilMoisture = 0;
//volatile float soiltemper = 0;

/* Private variables ---------------------------------------------------------*/
static const char* TAG = __FILE__;

/* USER CODE BEGIN PV */
struct tcpd_service_state {
	struct tcp_pcb *pcb;
	struct pbuf *req;
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */
static err_t tcp_service_poll(void *arg, struct tcp_pcb *tpcb);
/* USER CODE END PFP */


/**
 * The connection shall be actively closed (using RST to close from fault states).
 * Reset the sent- and recv-callbacks.
 *
 * @param pcb the tcp pcb to reset callbacks
 * @param hs connection state to free
 */
static err_t tcpd_service_close_or_abort_conn(struct tcp_pcb *pcb,
		struct tcpd_service_state *hs, u8_t abort_conn) {
	err_t err;
	LWIP_DEBUGF(TCPD_DEBUG, ("0 Closing connection %p\n", (void*)pcb));

	tcp_arg(pcb, NULL);
	tcp_recv(pcb, NULL);
	tcp_err(pcb, NULL);
	tcp_poll(pcb, NULL, 0);
	tcp_sent(pcb, NULL);

	if (abort_conn) {
		tcp_abort(pcb);
		return ERR_OK;
	}

	LWIP_DEBUGF(TCPD_DEBUG, ("1 Closing connection %p\n", (void*)pcb));
	err = tcp_close(pcb);
	if (err != ERR_OK) {
		LWIP_DEBUGF(TCPD_DEBUG, ("2 Error %d closing %p\n", err, (void*)pcb));
		/* error closing, try again later in poll */
		tcp_poll(pcb, tcp_service_poll, TCPD_POLL_INTERVAL);
	}

	if (hs != NULL) {
		if (hs->req) {
			LWIP_DEBUGF(TCPD_DEBUG, ("3 Closing connection %p\n", (void*)pcb));
			pbuf_free(hs->req);
			hs->req = NULL;
		}
		LWIP_DEBUGF(TCPD_DEBUG, ("4 Closing connection %p\n", (void*)pcb));
		mem_free(hs);
	}

	return err;
}

/**
 * The connection shall be actively closed.
 * Reset the sent- and recv-callbacks.
 *
 * @param pcb the tcp pcb to reset callbacks
 * @param hs connection state to free
 */
static err_t
tcpd_service_close_conn(struct tcp_pcb *pcb, struct tcpd_service_state *hs)
{
   return tcpd_service_close_or_abort_conn(pcb, hs, 0);
}

static err_t tcp_service_sent(void *arg, struct tcp_pcb *tpcb,u16_t len){
	struct tcpd_service_state *tcpd_s = (struct tcpd_service_state *)arg;

	return ERR_OK;
}

static err_t tcp_service_poll(void *arg, struct tcp_pcb *tpcb){
	struct tcpd_service_state *tcpd_s = (struct tcpd_service_state *)arg;

	if (tpcb->state == CLOSE_WAIT) {
		LWIP_DEBUGF(TCPD_DEBUG, (" tpcb == NULL \n"));
		tcpd_service_close_conn(tcpd_s->pcb, tcpd_s);
	}
	return ERR_OK;
}

static void  tcp_service_err(void *arg, err_t err){
	struct tcpd_service_state *tcpd_s = (struct tcpd_service_state *)arg;
	LWIP_DEBUGF(TCPD_DEBUG, ("Error, http_recv: hs is NULL, close\n"));

	tcpd_service_close_conn(tcpd_s->pcb, tcpd_s);
}

void dataProcessingRecvFuntion( struct tcp_pcb *tpcb ,char *buf)
{
	struct tcpDataRecv *recv = (struct tcpDataRecv *)buf;
    char sendData[50]= {0};
    struct tcpDataSend *send = (struct tcpDataRecv *)sendData;
	int temp = 0;
	int hum = 0;
	int soiltemp = 0;
	int soilhum = 0;

	printf("recv->cmd = %d\n",ntohl(recv->cmd));
	printf("recv->len = %d\n",ntohl(recv->len));
	printf("recv->data = %d\n",ntohl(recv->data));

	if(ntohl(recv->cmd) == TCP_RECV_CMD_RECV_SETTIME){
		setTimeData = ntohl(recv->data);
		printf("!!! set time  =  %d !!!\n",setTimeData);

	}else if(ntohl(recv->cmd) == TCP_RECV_CMD_RECV_GETDATA){
		printf("!!! request data !!!\n");
//			memcpy(send->head,"TA",2);
		send->cmd = htonl(TCP_RECV_CMD_TO_GETDATA);
		send->len = htonl(16);

		temp = (int)(my_sensor.temperature_air+0.5);
		hum = (int)(my_sensor.humidity_air+0.5);
		soilhum = (int)(my_sensor.humidity_soil+0.5);
		soiltemp = (int)(my_sensor.temperature_soil+0.5);

		send->temper = htonl(temp);
		send->hum = htonl(hum);
		send->soilHum = htonl(soilhum);
		send->soiltemper = htonl(soiltemp);

		printf("send air temper %d\n",temp);
		printf("send air hum %d\n",hum);
		printf("send air soilHum %d\n",soilhum);
		printf("send air soiltemper %d\n",soiltemp);

		tcp_write(tpcb,sendData,sizeof(struct tcpDataSend),0);
		tcp_output(tpcb);
	}else{
		printf("!!! No Cmd !!!\n");
	}
}



static err_t tcp_service_recv(void *arg, struct tcp_pcb *tpcb,
                             struct pbuf *p, err_t err){
	struct tcpd_service_state *tcpd_s = (struct tcpd_service_state *)arg;
	LWIP_DEBUGF(TCPD_DEBUG | LWIP_DBG_TRACE, ("tcpd_service_recv: pcb=%p pbuf=%p err=%s\n", (void*)tpcb,
	(void*)p, lwip_strerr(err)));


	if ((err != ERR_OK) || (p == NULL) || (tcpd_s == NULL)) {
		/* error or closed by other side? */
		if (p != NULL) {
			/* Inform TCP that we have taken the data. */
			tcp_recved(tpcb, p->tot_len);
			pbuf_free(p);
		}
		if (tcpd_s == NULL) {
			/* this should not happen, only to be robust */
			LWIP_DEBUGF(TCPD_DEBUG, ("Error, http_recv: hs is NULL, close\n"));
			tcpd_service_close_conn(tpcb, tcpd_s);
		}


		return ERR_OK;
	}

	{
		/* Inform TCP that we have taken the data. */
		tcp_recved(tpcb, p->tot_len);
        char buf[512] = {0};
		printf("p %s len %d\r\n",(char *)p->payload,p->tot_len);
		memcpy(buf,p->payload,p->tot_len);
		dataProcessingRecvFuntion(tpcb,buf);

		if (tcpd_s == NULL){
			D("tcpd_s == NULL \r\n");
		}else{
			D("tcpd_s != NULL \r\n");
		}
//		tcpd_service_close_conn(tpcb, tcpd_s);
		pbuf_free(p);
	}

	return ERR_OK;
}

err_t tcpd_service_accept(void *arg, struct tcp_pcb *pcb, err_t err) {

	struct tcpd_service_state *tcpd_s;
	LWIP_UNUSED_ARG(err);
	LWIP_UNUSED_ARG(arg);
	LWIP_DEBUGF(TCPD_DEBUG, ("tcpd_service_accept %p / %p\n", (void*)pcb, arg));

	if ((err != ERR_OK) || (pcb == NULL)) {
		return ERR_VAL;
	}
	/* Set priority */
	tcp_setprio(pcb, TCPD_TCP_PRIO);

	tcpd_s = (struct tcpd_service_state *)mem_malloc(sizeof(struct tcpd_service_state));
	if (tcpd_s == NULL) {
		LWIP_DEBUGF(TCPD_DEBUG, ("http_accept: Out of memory, RST\n"));
		return ERR_MEM;
	}
	memset(tcpd_s, 0, sizeof(struct tcpd_service_state));
	tcpd_s->pcb = pcb;

	tcp_arg(pcb, tcpd_s);
	/* Set up the various callback functions */
	tcp_recv(pcb, tcp_service_recv);
	tcp_err(pcb, tcp_service_err);
	tcp_poll(pcb, tcp_service_poll, TCPD_POLL_INTERVAL);
	tcp_sent(pcb, tcp_service_sent);

	return ERR_OK;
}

void tcpd_service_init() {
	struct tcp_pcb *pcb;
	err_t err;
	LWIP_DEBUGF(TCPD_DEBUG, ("tcpd_service_init\n"));

	pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	LWIP_ASSERT("tcpd_service_init: tcp_new failed", pcb != NULL);
	tcp_setprio(pcb, TCPD_TCP_PRIO);

	err = tcp_bind(pcb, IP_ANY_TYPE, TCPD_SERVER_PORT);
	LWIP_UNUSED_ARG(err); /* in case of LWIP_NOASSERT */
	LWIP_ASSERT("tcpd_service_init: tcp_bind failed", err == ERR_OK);
	pcb = tcp_listen(pcb);
	LWIP_ASSERT("tcpd_service_init: tcp_listen failed", pcb != NULL);
	tcp_accept(pcb, tcpd_service_accept);
}

