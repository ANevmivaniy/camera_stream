
#ifndef VIDEO_STREAM_I2C_CAMERA_INIT_H
#define VIDEO_STREAM_I2C_CAMERA_INIT_H
#include <ov7670configs/configs.h>
#include <stdint.h>

const uint8_t i2cCameraAddress = 0x21;

void init();
void setRegister(const struct RegisterData* data);
struct RegisterData* getRegister(uint8_t addr);

#endif //VIDEO_STREAM_I2C_CAMERA_INIT_H
