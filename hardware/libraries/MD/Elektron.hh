#ifndef ELEKTRON_H__
#define ELEKTRON_H__

#include <inttypes.h>

uint8_t sysex_to_data_elektron(uint8_t *sysex, uint8_t *data, uint8_t len);
uint8_t data_to_sysex_elektron(uint8_t *data, uint8_t *sysex, uint8_t len);

#endif /* ELEKTRON_H__ */
