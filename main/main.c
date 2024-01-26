#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "connect_wifi.h"

// https://wokwi.com/projects/305566932847821378
#define LED_PIN 2
// skapa en funktion som körs på annan TASK
// 2. Fixa en WIFI-koppling
void check_update_task(void *pvParameter) {
    while(1) {
        char buf[255];
        sprintf(buf, "%s?token=%d",UPDATE_JSON_URL,cnt);    
    }
}


void app_main(void)
{
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    connect_wifi();

    xTaskCreate(sendTask, "sendToThingSpeak", 8192, NULL, 5, NULL);

    while(1){
        gpio_set_level(LED_PIN,1 );
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(LED_PIN,0 );
        vTaskDelay(1000 / portTICK_PERIOD_MS);        
    }
}
