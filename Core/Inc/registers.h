//
// Created by alnem on 13.11.2021.
//

#ifndef VIDEO_STREAM_CAMERAOV7670REGISTERS_H
#define VIDEO_STREAM_CAMERAOV7670REGISTERS_H
#include "ov7670configs/configs.h"

void setI2cHandleTypeDef(I2C_HandleTypeDef * hi2c1);
HAL_StatusTypeDef resetSettings();
void setRegisters(const struct RegisterData *registerData);
HAL_StatusTypeDef setRegister(uint8_t addr, uint8_t val);
uint8_t readRegister(uint8_t addr);
void setRegisterBitsOR(uint8_t addr, uint8_t bits);
void setRegisterBitsAND(uint8_t addr, uint8_t bits);
void setDisablePixelClockDuringBlankLines();
void setDisableHREFDuringBlankLines();
void setHREFReverse();
void setInternalClockPreScaler(int preScaler);
void setPLLMultiplier(uint8_t multiplier);
void setManualContrastCenter(uint8_t center);
void setContrast(uint8_t contrast);
void setBrightness(uint8_t birghtness);
void reversePixelBits();
void setShowColorBar(bool transparent);

#endif //VIDEO_STREAM_CAMERAOV7670REGISTERS_H
