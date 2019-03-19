/* i2c - Example

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "driver/i2c.h"
#include "i2c_main.h"
#include "tcpd_service.h"

/**
 * TEST CODE BRIEF
 *
 * This example will show you how to use I2C module by running two tasks on i2c bus:
 *
 * - read external i2c sensor, here we use a BH1750 light sensor(GY-30 module) for instance.
 * - Use one I2C port(master mode) to read or write the other I2C port(slave mode) on one ESP32 chip.
 *
 * Pin assignment:
 *
 * - slave :
 *    GPIO25 is assigned as the data signal of i2c slave port
 *    GPIO26 is assigned as the clock signal of i2c slave port
 * - master:
 *    GPIO18 is assigned as the data signal of i2c master port
 *    GPIO19 is assigned as the clock signal of i2c master port
 *
 * Connection:
 *
 * - connect GPIO18 with GPIO25
 * - connect GPIO19 with GPIO26
 * - connect sda/scl of sensor with GPIO18/GPIO19
 * - no need to add external pull-up resistors, driver will enable internal pull-up resistors.
 *
 * Test items:
 *
 * - read the sensor data, if connected.
 * - i2c master(ESP32) will write data to i2c slave(ESP32).
 * - i2c master(ESP32) will read data from i2c slave(ESP32).
 */


SemaphoreHandle_t print_mux = NULL;



my_sensor_t my_sensor;


static esp_err_t i2c_read_sht30_test(i2c_port_t i2c_num, float* data_t,	float* data_h) {
	esp_err_t ret;
	i2c_cmd_handle_t cmd;
	uint8_t retry_times = 0;
	uint8_t temp[6] = {0};
	uint16_t read_temp = 0;
	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ( SHT30_SENSOR_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, 0x24, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	if (ret != ESP_OK) {
		return ret;
	}
	retry_times = 0;
	do {
		retry_times++;
		if (retry_times > 5) {
			printf("read sht30 error\r\n");
			return ESP_FAIL;
		}
		vTaskDelay(20 / portTICK_RATE_MS);

		cmd = i2c_cmd_link_create();
		i2c_master_start(cmd);
		i2c_master_write_byte(cmd, ( SHT30_SENSOR_ADDR << 1) | READ_BIT, ACK_CHECK_EN);
		i2c_master_read_byte(cmd, &temp[0], ACK_VAL);
		i2c_master_read_byte(cmd, &temp[1], ACK_VAL);
		i2c_master_read_byte(cmd, &temp[2], ACK_VAL);
		i2c_master_read_byte(cmd, &temp[3], ACK_VAL);
		i2c_master_read_byte(cmd, &temp[4], ACK_VAL);
		i2c_master_read_byte(cmd, &temp[5], NACK_VAL);
		i2c_master_stop(cmd);
		ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
		i2c_cmd_link_delete(cmd);
	} while (ret != ESP_OK);
	read_temp = ((uint16_t)temp[0] << 8) + temp[1];
	*data_t = read_temp * 175.0 / 65535 - 45;
	read_temp = ((uint16_t)temp[3] << 8) + temp[4];
	*data_h = (100.0 * read_temp/65535);
	return ret;
}

static esp_err_t i2c_read_sht20_test(i2c_port_t i2c_num, float* data_t,	float* data_h) {
	esp_err_t ret;
	i2c_cmd_handle_t cmd;
	uint8_t data_m;
	uint8_t data_l;
	uint8_t retry_times = 0;
	uint16_t temp;
	//tri humidity
	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ( SHT20_SENSOR_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, SHT20_CMD_TRI_H_N, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	if (ret != ESP_OK) {
		return ret;
	}
	retry_times = 0;
	do {
		retry_times++;
		if (retry_times > 3) {
			printf("read humidity error\r\n");
			return ESP_FAIL;
		}
		vTaskDelay(30 / portTICK_RATE_MS);

		cmd = i2c_cmd_link_create();
		i2c_master_start(cmd);
		i2c_master_write_byte(cmd, ( SHT20_SENSOR_ADDR << 1) | READ_BIT, ACK_CHECK_EN);
		i2c_master_read_byte(cmd, &data_m, ACK_VAL);
		i2c_master_read_byte(cmd, &data_l, NACK_VAL);
		i2c_master_stop(cmd);
		ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
		i2c_cmd_link_delete(cmd);
	} while (ret != ESP_OK);
	temp = ((uint16_t) data_m << 8) + (data_l & 0xfc);
	*data_h = (temp * 125) / 65536.0 - 6;
	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ( SHT20_SENSOR_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, SHT20_CMD_TRI_T_N, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	if (ret != ESP_OK) {
		return ret;
	}
	retry_times = 0;
	do {
		retry_times++;
		if (retry_times > 3) {
			printf("read temperature error\r\n");
			return ESP_FAIL;
		}
		vTaskDelay(30 / portTICK_RATE_MS);
		cmd = i2c_cmd_link_create();
		i2c_master_start(cmd);
		i2c_master_write_byte(cmd, ( SHT20_SENSOR_ADDR << 1) | READ_BIT, ACK_CHECK_EN);
		i2c_master_read_byte(cmd, &data_m, ACK_VAL);
		i2c_master_read_byte(cmd, &data_l, NACK_VAL);
		i2c_master_stop(cmd);
		ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
		i2c_cmd_link_delete(cmd);
	} while (ret != ESP_OK);
	temp = ((uint16_t) data_m << 8) + (data_l & 0xfc);
	*data_t = (temp * 175.72) / 65536.0 - 46.85;
	return ret;
}


/**
 * @brief i2c master initialization
 */
void i2c_example_master_init()
{
    int i2c_master_port = I2C_EXAMPLE_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_EXAMPLE_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_EXAMPLE_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_EXAMPLE_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode,
                       I2C_EXAMPLE_MASTER_RX_BUF_DISABLE,
                       I2C_EXAMPLE_MASTER_TX_BUF_DISABLE, 0);
}

void i2c_test_task(void* arg)
{
    int ret;
    uint32_t task_idx = (uint32_t) arg;
//    uint8_t sensor_data_h, sensor_data_l;
    while (1) {
        printf("read_count: %d\n", my_sensor.read_count);
        my_sensor.read_count++;
        ret = i2c_read_sht20_test(I2C_EXAMPLE_MASTER_NUM, &my_sensor.temperature_soil, &my_sensor.humidity_soil);
        if(ret == ESP_ERR_TIMEOUT) {
            printf("sht20I2C timeout\n");
        } else if(ret == ESP_OK) {
            printf("*******************\n");
            printf("TASK[%d]  MASTER READ SENSOR( SHT20 )\n", task_idx);
            printf("*******************\n");
            printf("humidity_soil: %f\n", my_sensor.humidity_soil);
            printf("temperature_soil: %f\n\r\n", my_sensor.temperature_soil);
        } else {
            printf("%s: No ack, sensor not connected...skip...\n", esp_err_to_name(ret));
        }

        ret = i2c_read_sht30_test(I2C_EXAMPLE_MASTER_NUM, &my_sensor.temperature_air, &my_sensor.humidity_air);
		if(ret == ESP_ERR_TIMEOUT) {
			printf("sht30I2C timeout\n");
		} else if(ret == ESP_OK) {
			printf("*******************\n");
			printf("TASK[%d]  MASTER READ SENSOR( SHT30 )\n", task_idx);
			printf("*******************\n");
			printf("humidity_air: %f\n", my_sensor.humidity_air);
			printf("temperature_air: %f\n\r\n", my_sensor.temperature_air);
		} else {
			printf("%s: No ack, sensor not connected...skip...\n", esp_err_to_name(ret));
		}

        vTaskDelay((setTimeData*1000) / portTICK_RATE_MS);
    }
}

