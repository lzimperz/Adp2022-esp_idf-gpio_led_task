/* GPIO Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "DHT22.h"

/**
 * Brief:
 * This test code shows how to configure gpio and how to use gpio interrupt.
 *
 * GPIO status:
 * GPIO18: output
 * GPIO19: output
 * GPIO4:  input, pulled up, interrupt from rising edge and falling edge
 * GPIO5:  input, pulled up, interrupt from rising edge.
 *
 * Test:
 * Connect GPIO18 with GPIO4
 * Connect GPIO19 with GPIO5
 * Generate pulses on GPIO18/19, that triggers interrupt on GPIO4/5
 *
 */

#define GPIO_OUTPUT_IO_0    2
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0))
#define ESP_INTR_FLAG_DEFAULT 0
#define ON 1
#define OFF 0
#define LED_CHANGE_DELAY_MS    250

static void led_toggle_task(void* arg)
{
    static uint8_t led_state = OFF;

    while(true) {
        if (led_state == OFF) {
            led_state = ON;
            gpio_set_level(GPIO_OUTPUT_IO_0, ON);
        }
        else {
                led_state = OFF;
                gpio_set_level(GPIO_OUTPUT_IO_0, OFF);
        }

        vTaskDelay(LED_CHANGE_DELAY_MS / portTICK_PERIOD_MS);
        printf("Toggle LED\n");
    }
}


static void counter_task(void* arg)
{
    int cnt = 0;
    while(true) {
        printf("Ejemplo TASKS AdP_2022 - Counts: %d\n", cnt++);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void DHT_task(void *pvParameter)
{
	setDHTgpio( 23 );
	printf( "Starting DHT Task\n\n");

	while(1) {
	
		printf("=== Reading DHT ===\n" );
		int ret = readDHT();
		
		errorHandler(ret);

		printf( "Hum %.1f\n", getHumidity() );
		printf( "Tmp %.1f\n", getTemperature() );
		
		// -- wait at least 2 sec before reading again ------------
		// The interval of whole process must be beyond 2 seconds !! 
		vTaskDelay( 3000 / portTICK_RATE_MS );
	}
}

void app_main(void)
{
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);


    //start toggle led task
    xTaskCreate(led_toggle_task, "led_toggle_task", 2048, NULL, 10, NULL);

    //start counter task
    xTaskCreate(counter_task, "counter_task", 2048, NULL, 10, NULL);

    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

	xTaskCreate( &DHT_task, "DHT_task", 2048, NULL, 5, NULL );

    while(1) {
        vTaskDelay(1000 / portTICK_RATE_MS);
        // idle
    }
}
