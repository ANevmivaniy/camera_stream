//
// Created by alnem on 12.02.2022.
//

#ifndef TEST_VIRTUAL_COMPORT_SETUP_CAMERA_H
#define TEST_VIRTUAL_COMPORT_SETUP_CAMERA_H
#define REG_SIZE 100

#include <stm32f4xx.h>
#include "ov7670configs/configs.h"
#include <stdbool.h>

struct StatusStruct setup(I2C_HandleTypeDef * hi2c1, enum Resolution resolution, enum PixelFormat pixelFormat);
struct RegisterData* getSetupInfo();
struct RegisterData* getSetupInfo2();

#endif //TEST_VIRTUAL_COMPORT_SETUP_CAMERA_H
