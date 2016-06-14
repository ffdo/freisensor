#ifndef SENSORS_H_
#define SENSORS_H_

#include <FreeRTOS.h>

#define FREISENSOR_VERSION "0.0.1"

typedef double (*get_latest_sensor_value) (void);
typedef void (*start_conversion) (void);
typedef bool (*conversion_finished) (void);
typedef int (*sensor_init)(void);

typedef void (*freisensor_finished)(void);

enum sensor_type {
  TEMPERATURE,
  HUMIDITY,
  ATMOSPHERIC_PRESSURE,
  WIND_SPEED,
  LUMEN,
  DECIBEL,
  PPM
};
typedef enum sensor_type sensor_type_t;

struct sensor_module {
  get_latest_sensor_value get_latest_value;
  start_conversion start_conversion;
  conversion_finished conversion_finished;
  sensor_init init;
  sensor_type_t type;
};
typedef struct sensor_module sensor_module_t;

struct node {
  char *token;
  sensor_module_t **sensors;
  uint8_t sensor_count;
};
typedef struct node node_t;

char* sensor_type_to_string(sensor_type_t type);

node_t* new_node(char *token);

int add_sensor(node_t *node, sensor_module_t *sensor);

void init(node_t *node, char *endpoint, freisensor_finished callback);

#endif /* SENSORS_H_ */