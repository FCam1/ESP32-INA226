#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ina219.h>
#include <string.h>
#include <esp_log.h>
#include "driver/uart.h"
#include <math.h>

//export IDF_PATH=~/esp/esp-idf
//export PATH="$IDF_PATH/tools:$PATH"
//idf.py menuconfig


#define I2C_PORT 0
#define I2C_ADDR INA219_ADDR_GND_GND
#define SDA_GPIO 21
#define SCL_GPIO 22

const static char *TAG = "INA219_example";

void task(void *pvParameters)
{
   uart_config_t uart_config = {
        .baud_rate = 921600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_0, &uart_config);

    ina219_t dev;
    memset(&dev, 0, sizeof(ina219_t));

    ESP_ERROR_CHECK(ina219_init_desc(&dev, I2C_ADDR, I2C_PORT, SDA_GPIO, SCL_GPIO));
    ESP_LOGI(TAG, "Initializing INA219");
    ESP_ERROR_CHECK(ina219_init(&dev));

    ESP_LOGI(TAG, "Configuring INA219");
    //ESP_ERROR_CHECK(ina219_reset(&dev));
    //ESP_ERROR_CHECK(ina219_configure(&dev, INA219_BUS_RANGE_32V, INA219_GAIN_0_125,
      //     INA219_RES_12BIT_1S, INA219_RES_12BIT_1S, INA219_MODE_CONT_SHUNT_BUS));

    ESP_LOGI(TAG, "Calibrating INA219");
    ESP_ERROR_CHECK(ina219_calibrate(&dev, 40.0, 0.002)); // max current,  Ohm shunt resistance

    float bus_voltage, shunt_voltage, current, power;

    ESP_LOGI(TAG, "Starting the loop");
    while (1)
    {
        //long t = esp_timer_get_time();

        //ESP_ERROR_CHECK(ina219_get_bus_voltage(&dev, &bus_voltage));
        //ESP_ERROR_CHECK(ina219_get_current(&dev, &current));
        //ESP_ERROR_CHECK(ina219_get_power(&dev, &power));

        ESP_ERROR_CHECK(ina219_getVCP(&dev, &bus_voltage,&current, &power));

        //t = esp_timer_get_time() - t;    

        /* Using float in printf() requires non-default configuration in
         * sdkconfig. See sdkconfig.defaults.esp32 and
         * sdkconfig.defaults.esp8266  */
        //ESP_ERROR_CHECK(ina219_get_shunt_voltage(&dev, &shunt_voltage));
        //printf("VBUS: %.04f mV, VSHUNT: %.04f mV, IBUS: %4.04f mA , PBUS: %.04f mW, time: %ld\n",
        //        bus_voltage, shunt_voltage *1, current * 1 , power * 1000, t); 

        /*Raw : */
        //printf("%ld  ,%f,%f,%f \n",t , bus_voltage,current,power);

        /*Metric : */
        printf("%f %f %f %f\n",esp_timer_get_time()/1000000.0,bus_voltage/1000.0,current,power);


        //vTaskDelay(100 / portTICK_PERIOD_MS);

    }
}

void app_main()
{
    ESP_ERROR_CHECK(i2cdev_init());
    xTaskCreate(task, "test", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL);
}