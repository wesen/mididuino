#ifndef ELEKTRON_H__
#define ELEKTRON_H__

#include <inttypes.h>

uint16_t sysex_to_data_elektron(uint8_t *sysex, uint8_t *data, uint16_t len);
uint16_t data_to_sysex_elektron(uint8_t *data, uint8_t *sysex, uint16_t len);

#endif /* ELEKTRON_H__ */
