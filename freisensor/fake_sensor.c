#include "freisensor/fake_sensor.h"
#include "espressif/esp_common.h"

void do_nothing() {
  printf("The fake sensor does nothing\n");
}

int fake_init() {
  printf("Fake sensor init\n");
  return 0;
}

bool conversion_is_always_finished() {
  printf("Fake conversion is finished\n");
  return true;
}

double get_fake_value() {
  printf("Reporting fake value\n");
  return 42.0;
}

sensor_module_t *get_fake_sensor(){
  sensor_module_t *sensor = (sensor_module_t *)malloc(sizeof(sensor_module_t));
  sensor->type = TEMPERATURE;
  sensor->get_latest_value = get_fake_value;
  sensor->start_conversion = do_nothing;
  sensor->init = fake_init;
  sensor->conversion_finished = conversion_is_always_finished;
  return sensor;
}