
#include <cstdint>
#include <ov7670configs/configs.h>

// First few lines are garbage.
// For some reason increasing vstart will not remove the first line, and causes synchronization problems.
// It is easier read all lines from the beginning and ignore the garbage lines in the code.
const uint8_t VGA_VERTICAL_PADDING = 10;
const uint16_t vstart = 0;
const uint16_t vstop = 480 + VGA_VERTICAL_PADDING;

// 480 + 1 pixel (2 bytes) for padding.
const uint16_t hstart = 157;
const uint16_t hstop = 14;

RegisterData regsVGA [] = {
    {REG_VSTART,vstart >> 2},
    {REG_VSTOP,vstop >> 2},
    {REG_VREF,(vstart & 0b11) | ((vstop & 0b11) << 2)},
    {REG_HSTART,hstart >> 3},
    {REG_HSTOP,hstop >> 3},
    {REG_HREF,0b11000000 | (hstart & 0b111) | ((hstop & 0b111) << 3)}, // Note sure the 0b11000000 is necessary.

    {0xff, 0xff},		/* END MARKER */
};
