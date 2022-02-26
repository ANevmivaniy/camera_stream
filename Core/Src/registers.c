
#include <stdint.h>
#include <stdbool.h>
#include <stm32f4xx.h>
#include "../Inc/registers.h"

extern struct RegisterData* regsDefault;
extern struct RegisterData* regsVGA;
const uint8_t i2cAddress = 0x21;

uint8_t writeBuf[2];
uint8_t readBuf;
I2C_HandleTypeDef * baseHi2c1;

void setI2cHandleTypeDef(I2C_HandleTypeDef * hi2c1) {
    baseHi2c1 = hi2c1;
}

HAL_StatusTypeDef resetSettings() {
  HAL_StatusTypeDef status = setRegister(REG_COM7, COM7_RESET);
  //bool isSuccessful = setRegister(REG_COM7, COM7_RESET);
  HAL_Delay(500);
  return status;
}

void setRegisters(const struct RegisterData *programMemPointer) {
  while (1) {
    struct RegisterData regData;
    regData.addr = programMemPointer->addr;
    regData.val = programMemPointer->val;
    if (regData.addr == 0xFF) {
      break;
    } else {
      setRegister(regData.addr, regData.val);
      programMemPointer++;
    }
  }
}

HAL_StatusTypeDef setRegister(uint8_t addr, uint8_t val) {
    *(writeBuf + 0) = addr;
    *(writeBuf + 1) = val;
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(
            baseHi2c1, i2cAddress, writeBuf, 2 * sizeof (uint8_t), 1000);

  return status;
}

uint8_t readRegister(uint8_t addr) {
    HAL_I2C_Mem_Read(
            baseHi2c1,
            i2cAddress,
            (uint16_t) addr,
            sizeof (uint8_t),
            &readBuf,
            sizeof (uint8_t),
            1000
            );

    return readBuf;
}

void setRegisterBitsOR(uint8_t addr, uint8_t bits) {
  uint8_t val = readRegister(addr);
  setRegister(addr, val | bits);
}

void setRegisterBitsAND(uint8_t addr, uint8_t bits) {
  uint8_t val = readRegister(addr);
  setRegister(addr, val & bits);
}


void setDisablePixelClockDuringBlankLines() {
  setRegisterBitsOR(REG_COM10, COM10_PCLK_HB);
}

void setDisableHREFDuringBlankLines() {
  setRegisterBitsOR(REG_COM6, COM6_HREF_HB);
}

void setHREFReverse() {
  setRegisterBitsOR(REG_COM10, COM10_HREF_REV);
}


void setInternalClockPreScaler(int preScaler) {
  setRegister(REG_CLKRC, 0x80 | preScaler); // f = input / (val + 1)
}


void setPLLMultiplier(uint8_t multiplier) {
  uint8_t mask = 0b11000000;
  uint8_t currentValue = readRegister(DBLV);
  setRegister(DBLV, (currentValue & ~mask) | (multiplier << 6));
}


void setManualContrastCenter(uint8_t contrastCenter) {
  setRegisterBitsAND(MTXS, 0x7F); // disable auto contrast
  setRegister(REG_CONTRAST_CENTER, contrastCenter);
}


void setContrast(uint8_t contrast) {
  // default 0x40
  setRegister(REG_CONTRAS, contrast);
}


void setBrightness(uint8_t birghtness) {
  setRegister(REG_BRIGHT, birghtness);
}


void reversePixelBits() {
  setRegisterBitsOR(REG_COM3, COM3_SWAP);
}


void setShowColorBar(bool transparent) {
  if (transparent) {
    setRegisterBitsOR(REG_COM7, COM7_COLOR_BAR);
  } else {
    setRegisterBitsOR(REG_COM17, COM17_CBAR);
  }
}



