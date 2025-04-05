#include "ft6206.h"
#include "pico/stdlib.h"
#include <string.h>

void ft6206_init(FT6206 *dev, i2c_inst_t *i2c, uint8_t addr) {
    dev->i2c = i2c;
    dev->addr = addr;
    dev->touches = 0;
    memset(dev->touchX, 0, sizeof(dev->touchX));
    memset(dev->touchY, 0, sizeof(dev->touchY));
    memset(dev->touchID, 0, sizeof(dev->touchID));
}

bool ft6206_begin(FT6206 *dev, uint8_t threshold) {
    uint8_t vend = ft6206_read_register8(dev, FT62XX_REG_VENDID);
    if (vend != FT62XX_VENDID) return false;

    uint8_t chip = ft6206_read_register8(dev, FT62XX_REG_CHIPID);
    if (chip != FT6206_CHIPID && chip != FT6236_CHIPID &&
        chip != FT6236U_CHIPID && chip != FT6336U_CHIPID)
        return false;

    if (threshold != 0)
        ft6206_write_register8(dev, FT62XX_REG_THRESHHOLD, threshold);

    return true;
}

uint8_t ft6206_touched(FT6206 *dev) {
    uint8_t n = ft6206_read_register8(dev, FT62XX_REG_NUMTOUCHES);
    return (n > 2) ? 0 : n;
}

TS_Point ft6206_get_point(FT6206 *dev, uint8_t n) {
    ft6206_read_data(dev);
    TS_Point p;
    if (dev->touches == 0 || n > 1) {
        p.x = p.y = p.z = 0;
    } else {
        p.x = dev->touchX[n];
        p.y = dev->touchY[n];
        p.z = 1;
    }
    return p;
}

void ft6206_read_data(FT6206 *dev) {
    uint8_t data[16] = {0};
    uint8_t reg = 0;

    i2c_write_blocking(dev->i2c, dev->addr, &reg, 1, true);
    i2c_read_blocking(dev->i2c, dev->addr, data, 16, false);

    dev->touches = data[0x02];
    if (dev->touches > 2 || dev->touches == 0) {
        dev->touches = 0;
        return;
    }

    for (uint8_t i = 0; i < dev->touches; i++) {
        dev->touchX[i] = ((data[0x03 + i * 6] & 0x0F) << 8) | data[0x04 + i * 6];
        dev->touchY[i] = ((data[0x05 + i * 6] & 0x0F) << 8) | data[0x06 + i * 6];
        dev->touchID[i] = data[0x05 + i * 6] >> 4;
    }
}

uint8_t ft6206_read_register8(FT6206 *dev, uint8_t reg) {
    uint8_t val = 0;
    i2c_write_blocking(dev->i2c, dev->addr, &reg, 1, true);
    i2c_read_blocking(dev->i2c, dev->addr, &val, 1, false);
    return val;
}

void ft6206_write_register8(FT6206 *dev, uint8_t reg, uint8_t val) {
    uint8_t data[2] = {reg, val};
    i2c_write_blocking(dev->i2c, dev->addr, data, 2, false);
}