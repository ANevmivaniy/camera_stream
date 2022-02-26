//
// Created by alnem on 12.02.2022.
//
#include <stdbool.h>
#include "setup_camera.h"
#include "registers.h"
#include <stm32f4xx.h>
#include "ov7670configs/configs.h"
#include "ov7670configs/default_configs.h"

struct StatusStruct setup(I2C_HandleTypeDef * hi2c1, enum Resolution resolution, enum PixelFormat pixelFormat) {
    setI2cHandleTypeDef(hi2c1);
    struct StatusStruct status;
    status.status = resetSettings();
    status.value = false;
    if (status.status == HAL_OK) {
        //setRegisters(regsDefault);
        status.value = true;
    }

    return status;
}
