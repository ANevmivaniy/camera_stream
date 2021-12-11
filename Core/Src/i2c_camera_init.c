//
// Created by alnem on 13.11.2021.
//
#include <i2c_camera_init.h>
#include <stm32f4xx_hal_i2c.h>

void init() {


}

void setRegister(const struct RegisterData* data) {

    HAL_I2C_Master_Transmit();
}
