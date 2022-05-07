
#include <stdint.h>
#include "configs.h"

const uint8_t QVGA_VERTICAL_PADDING = 5;
const uint16_t vstart = 0;
const uint16_t vstop = 240 + QVGA_VERTICAL_PADDING;

// 240 + 1 pixel (2 bytes) for padding.
const uint16_t hstart = 176;
const uint16_t hstop = 34;

struct RegisterData regsQVGA [] = {
    {REG_VSTART,vstart >> 1},
    {REG_VSTOP,vstop >> 1},
    {REG_VREF,((vstart & 0b1) << 1) | ((vstop & 0b1) << 3)},
    {REG_HSTART,hstart >> 3},
    {REG_HSTOP,hstop >> 3},
    {REG_HREF,0b00000000 | (hstart & 0b111) | ((hstop & 0b111) << 3)},

    {REG_COM3, COM3_DCWEN}, // enable downsamp/crop/window
    {REG_COM14, 0x19},        // divide by 2
    {SCALING_DCWCTR, 0x11},   // downsample by 2
    {SCALING_PCLK_DIV, 0xf1}, // divide by 2

    {0xff, 0xff},	/* END MARKER */
};
