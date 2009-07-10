#ifndef ELEKTRON_H__
#define ELEKTRON_H__

#include <inttypes.h>

typedef struct machine_name_s {
  char name[11];
  uint8_t id;
} machine_name_t;

typedef struct model_param_name_s {
  char name[4];
  uint8_t id;
} model_param_name_t;

typedef struct model_to_param_names_s {
  uint8_t model;
  model_param_name_t *names;
} model_to_param_names_t;

uint16_t sysex_to_data_elektron(uint8_t *sysex, uint8_t *data, uint16_t len);
uint16_t data_to_sysex_elektron(uint8_t *data, uint8_t *sysex, uint16_t len);


#endif /* ELEKTRON_H__ */
