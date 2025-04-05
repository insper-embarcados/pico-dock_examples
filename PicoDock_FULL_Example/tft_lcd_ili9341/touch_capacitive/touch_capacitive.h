#ifndef FT6206_H
#define FT6206_H

#include <stdint.h>
#include <stdbool.h>
#include "hardware/i2c.h"

// Endereço padrão do FT6206
#define FT6206_DEFAULT_ADDR       0x38
#define FT6206_DEFAULT_THRESHOLD  128

// Registradores
#define FT62XX_REG_VENDID         0xA8
#define FT62XX_REG_CHIPID         0xA3
#define FT62XX_REG_FIRMVERS       0xA6
#define FT62XX_REG_POINTRATE      0x88
#define FT62XX_REG_THRESHHOLD     0x80
#define FT62XX_REG_NUMTOUCHES     0x02

#define FT6206_CHIPID             0x06
#define FT6236_CHIPID             0x36
#define FT6236U_CHIPID            0x64
#define FT6336U_CHIPID            0x64
#define FT62XX_VENDID             0x11

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} TS_Point;

typedef struct {
    i2c_inst_t *i2c;
    uint8_t addr;
    uint8_t touches;
    uint16_t touchX[2];
    uint16_t touchY[2];
    uint8_t touchID[2];
} FT6206;

void ft6206_init(FT6206 *dev, i2c_inst_t *i2c, uint8_t addr);
bool ft6206_begin(FT6206 *dev, uint8_t threshold);
uint8_t ft6206_touched(FT6206 *dev);
TS_Point ft6206_get_point(FT6206 *dev, uint8_t n);
void ft6206_read_data(FT6206 *dev);

uint8_t ft6206_read_register8(FT6206 *dev, uint8_t reg);
void ft6206_write_register8(FT6206 *dev, uint8_t reg, uint8_t val);

#endif