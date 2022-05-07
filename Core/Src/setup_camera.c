//
// Created by alnem on 12.02.2022.
//
#include <ov7670configs/qvga.h>
#include <stdbool.h>
#include "setup_camera.h"
#include "registers.h"
#include <stm32f4xx.h>
#include "ov7670configs/configs.h"
#include "ov7670configs/default_configs.h"
#include "ov7670configs/rgb565.h"

enum Resolution resolution;

struct StatusStruct setup(I2C_HandleTypeDef * hi2c1, enum Resolution resolution, enum PixelFormat pixelFormat) {
    setI2cHandleTypeDef(hi2c1);
    struct StatusStruct status;
    status.status = resetSettings();
    status.value = false;
    if (status.status == HAL_OK) {
        setRegisters(regsDefault);
        status.value = true;

        switch (pixelFormat) {
              default:
              case PIXEL_RGB565:
                setRegisters(regsRGB565);
                break;
            }

        switch (resolution) {
             case RESOLUTION_QVGA_320x240:
               setRegisters(regsQVGA);
//               verticalPadding = QVGA_VERTICAL_PADDING;
//               horizontalBytePaddingLeft = 1;
//               horizontalBytePaddingRight = 1;
               break;
           }
    }

    return status;
}


struct RegisterData* getSetupInfo() {
    return getRegisters(regsDefault);
}

struct RegisterData* getSetupInfo2() {
    return getRegisters2(REG_SIZE);
}
