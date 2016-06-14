#include <espressif/esp_common.h>
#include <espressif/esp_system.h>
#include <espressif/esp_sta.h>
#include <espressif/esp_wifi.h>
#include <esp/uart.h>

#include "lwip/inet.h"

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "freisensor/sensors.h"
#include "freisensor/fake_sensor.h"

#include "node.h"
#include "ssid_config.h"

xSemaphoreHandle wifi_alive;

void measure_loop_finished() {
  printf("Entering hibernation\n");
  vTaskDelay( 5000 / portTICK_RATE_MS );
  //sdk_system_deep_sleep(SLEEP_MS);
}

static void wifi_task(void *pxParameter) {
  struct sdk_station_config config = {
    .ssid = WIFI_SSID,
    .password = WIFI_PASS,
  };
  sdk_wifi_set_opmode(STATION_MODE);
  sdk_wifi_station_set_config(&config);

  printf("Connecting to %s\n", WIFI_SSID);

  uint8_t retries = 30;
  uint8_t status  = 0;
  printf("Waiting for connection...\n");
  while(1) {
  while ((status != STATION_GOT_IP) && (retries)){
      status = sdk_wifi_station_get_connect_status();

      bool unrecoverable_fail = false;
      switch(status){
        case STATION_CONNECTING:
          printf("Connecting...\n");
          break;
        case STATION_IDLE:
          printf("Idle\n");
          break;
        case STATION_WRONG_PASSWORD:
          printf("WiFi: wrong password\n\r");
          unrecoverable_fail = true;
          break;
        case STATION_NO_AP_FOUND:
          printf("WiFi: AP not found\n\r");
          unrecoverable_fail = true;
          break;
        case STATION_CONNECT_FAIL:
          printf("WiFi: connection failed\r\n");
          unrecoverable_fail = true;
          break;
        default:
          printf("Unknown connection status: %d\n", status);
          break;
      }
      retries--;
      printf("Tries left %d\n", retries);
      if(unrecoverable_fail) {
        printf("Encountered unrecoverable error, stopping connection\n");
        break;
      }
      vTaskDelay( 3000 / portTICK_RATE_MS );
    }
    if (status == STATION_GOT_IP) {
      printf("Connected to network %s\n", WIFI_SSID);
      
      node_t *our_node = new_node(TOKEN);
      add_sensor(our_node, get_fake_sensor());
      init(our_node, COAP_ENDPOINT, measure_loop_finished);

      xSemaphoreGive( wifi_alive );
      taskYIELD();
    }
    while ((status = sdk_wifi_station_get_connect_status()) == STATION_GOT_IP) {
      xSemaphoreGive( wifi_alive );
      taskYIELD();
    }
    printf("WiFi: disconnected\n\r");
    sdk_wifi_station_disconnect();
    vTaskDelay( 1000 / portTICK_RATE_MS );
  }
}

void user_init(void) {
  //sdk_system_update_cpu_freq(SYS_CPU_80MHZ);
  uart_set_baud(0, 115200);
  printf("SDK version:%s\n", sdk_system_get_sdk_version());

  vSemaphoreCreateBinary(wifi_alive);

  xTaskCreate(wifi_task, (signed char *)"wifi_task", 768, NULL, 1, NULL);

}