#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "connect_wifi.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "cJSON.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"
// https://wokwi.com/projects/305566932847821378
#define LED_PIN 2
// skapa en funktion som körs på annan TASK
// 2. Fixa en WIFI-koppling

#define FIRMWARE_VERSION	0.7
#define UPDATE_JSON_URL		"https://github.com/aspcodenet/project-name2/raw/main/bin/firmware.json"



void updateTask(){
    // Fråga github finns ny version?
    // 1. fetch https:// firmware.json from Github 
    // 2. läsa i firmware.json finns ny version?
    // om ja 
    // ladda ner
    // installera på annan partition
    // reboot
    esp_restart();
}

// receive buffer
char rcv_buffer[200];

// esp_http_client event handler
esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
    
	switch(evt->event_id) {
        case HTTP_EVENT_REDIRECT:
        case HTTP_EVENT_ERROR:
            break;
        case HTTP_EVENT_ON_CONNECTED:
            break;
        case HTTP_EVENT_HEADER_SENT:
            break;
        case HTTP_EVENT_ON_HEADER:
            break;
        case HTTP_EVENT_ON_DATA:
            if (!esp_http_client_is_chunked_response(evt->client)) {
				strncpy(rcv_buffer, (char*)evt->data, evt->data_len);
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            break;
        case HTTP_EVENT_DISCONNECTED:
            break;
    }
    return ESP_OK;
}





// Stefans API nyckel RQZXY61HSJURBPZL
void sendTask(void *pvParameter) {
    int cnt = 12; // MÄTVÄRDEN FRÅN SENSORER
    int cnt2 = 13;// MÄTVÄRDEN FRÅN SENSORER
    while(1) {
        char buf[255];
        sprintf(buf, "https://api.thingspeak.com/update?api_key=RQZXY61HSJURBPZL&field1=%d&field2=%d",cnt,cnt2);    

        cnt++;
        cnt2++;

        // configure the esp_http_client
		esp_http_client_config_t config = {
            .url = buf,
            .event_handler = _http_event_handler,
            .keep_alive_enable = true,        
            .timeout_ms = 30000,
		};
		esp_http_client_handle_t client = esp_http_client_init(&config);
	
		// downloading the json file
		esp_err_t err = esp_http_client_perform(client);        

        vTaskDelay(10000 / portTICK_PERIOD_MS);        
    }
}


void app_main(void)
{
	esp_err_t ret = nvs_flash_init();
    ESP_LOGI("CH", "%d ret", ret);
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}

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
