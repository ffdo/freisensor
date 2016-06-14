#include "espressif/esp_common.h"

#include <FreeRTOS.h>
#include <task.h>
#include "freisensor/sensors.h"
#include "freisensor/coap.h"
#include <json/cJSON.h>

static xTaskHandle nodeTaskHandle;
static node_t *currentNode;
static char *reportEndpoint;

static freisensor_finished finished_callback;

char* sensor_type_to_string(sensor_type_t type) {
  switch(type){
    case TEMPERATURE:
      return "Temperature";
      break;
    default:
      return "Unknown";
      break;
  }
}

static void start_all_conversions() {
  printf("Starting conversion on %d sensors\n", currentNode->sensor_count);
  for(int i=0; i < currentNode->sensor_count; i++){
    printf("Starting conversion on sensor %d\n", i);
    sensor_module_t *sensor = currentNode->sensors[i];
    if(!sensor) {
      printf("Sensor %d is NULL\n", i);
      continue;
    } else {
      sensor->start_conversion();
    }
  }
}

static void wait_for_conversions() {
  printf("Waiting on %d sensors to finish conversion\n", currentNode->sensor_count);
  bool all_finished = false;

  while(!all_finished) {
    for(int i=0; i < currentNode->sensor_count; i++){
      sensor_module_t *sensor = currentNode->sensors[i];
      if(!sensor->conversion_finished()) {
        all_finished = false;
        continue;
      }
    }
    all_finished = true;
  }
  printf("All conversions finished\n");
}

static void send_sensor_values() {
  printf("Creating json report\n");
  cJSON *root;
  root = cJSON_CreateObject();

  for(int i=0; i < currentNode->sensor_count; i++){
    sensor_module_t *sensor = currentNode->sensors[i];
    cJSON_AddNumberToObject(root, sensor_type_to_string(sensor->type), sensor->get_latest_value());
  }
  char *report = cJSON_Print(root);
  printf("JSON report: %s", report);
  int ret = send_message(reportEndpoint, report);
  printf("Returncode from send_message %d\n", ret);
  if(ret < 0){
    printf("[ERROR] Can't send coap request\n");
  }
  free(report);
  cJSON_Delete(root);
}

static void measure_and_report_task(void *pxParameter) {

  while(1) {
    start_all_conversions();
    wait_for_conversions();
    send_sensor_values();
    printf("Measure loop finished, calling callback\n");
    finished_callback();
    //taskYIELD();
  }
}

void init(node_t *node, char *endpoint, freisensor_finished callback) {
  currentNode = node;
  reportEndpoint = endpoint;
  finished_callback = callback;
  printf("Initialising Freisensor version %s\n", FREISENSOR_VERSION);

  xTaskCreate(measure_and_report_task, (signed char *)"measure_and_report", 768, NULL, 8, &nodeTaskHandle);
}

node_t* new_node(char *token) {
  node_t *node = (node_t *)malloc(sizeof(node_t));
  node->sensor_count = 0;
  return node;
}

int add_sensor(node_t *node, sensor_module_t *sensor) {
  int ret = sensor->init();
  if(ret != 0) {
    return ret;
  }
  if(node->sensor_count == 0){
    node->sensors = (sensor_module_t **)malloc(sizeof(sensor_module_t *));
  } else {
    node->sensors = (sensor_module_t **)realloc(node->sensors, (node->sensor_count + 1) * sizeof(sensor_module_t *));
  }
  
  node->sensors[node->sensor_count] = sensor;
  node->sensor_count++;

  return 0;
}