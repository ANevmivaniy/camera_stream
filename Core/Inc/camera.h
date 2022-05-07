/*
 * camera.h
 *
 *  Created on: Apr 17, 2022
 *      Author: alnem
 */

#ifndef INC_CAMERA_H_
#define INC_CAMERA_H_

#include <stdbool.h>
#include <stdint.h>
#include <stm32f4xx.h>
#include "stm32f4xx_hal_gpio.h"
#include <usb_device.h>

// vsync - PB7
#ifndef OV7670_VSYNC
#define OV7670_VSYNC ((*GPIOB).IDR & 0x0080)
#endif

// pixel clock - PA6
#ifndef OV7670_PIXEL_CLOCK
#define OV7670_PIXEL_CLOCK ((*GPIOA).IDR & 0x0040)
#endif

// href - PA4
#ifndef OV7670_HREF
#define OV7670_HREF ((*GPIOA).IDR & 0x0010)
#endif

#ifndef OV7670_READ_PIXEL_BYTE
//#define OV7670_READ_PIXEL_BYTE(b) b = ((uint8_t*)(&(*GPIOB).IDR))[1]
#define OV7670_READ_PIXEL_BYTE(b) *b = ((uint8_t*)((((*GPIOC).IDR & 0b0000001111000000) >> 6) | ((*GPIOE).IDR & 0x0010) | (((*GPIOB).IDR & 0x0040) >> 1) | (((*GPIOE).IDR & 0b0000000001100000) << 1)))
#endif

static const uint8_t VERSION = 0x00;
static const uint8_t COMMAND_NEW_FRAME = 0x01 | VERSION;
static const uint8_t COMMAND_END_OF_LINE = 0x02;
static const uint8_t COMMAND_DEBUG_DATA = 0x03;

static const uint16_t COLOR_GREEN = 0x07E0;
static const uint16_t COLOR_RED = 0xF800;

// Lower three bits 0b00000111.
// Upper bits reserved for future attributes.
static const uint16_t UART_PIXEL_BYTE_PARITY_CHECK = 0x80;
static const uint16_t UART_PIXEL_FORMAT_RGB565 = 0x01 | UART_PIXEL_BYTE_PARITY_CHECK; // Signal to ArduImageCapture that parity check is enabled
static const uint16_t UART_PIXEL_FORMAT_GRAYSCALE = 0x02;


// Pixel byte parity check:
// Pixel Byte H: odd number of bits under H_BYTE_PARITY_CHECK and H_BYTE_PARITY_INVERT
// Pixel Byte L: even number of bits under L_BYTE_PARITY_CHECK and L_BYTE_PARITY_INVERT
//                                          H:RRRRRGGG
static const uint8_t H_BYTE_PARITY_CHECK =  0b00100000;
static const uint8_t H_BYTE_PARITY_INVERT = 0b00001000;
//                                          L:GGGBBBBB
static const uint8_t L_BYTE_PARITY_CHECK =  0b00001000;
static const uint8_t L_BYTE_PARITY_INVERT = 0b00100000;
// Since the parity for L byte can be zero we must ensure that the total byet value is above zero.
// Increasing the lowest bet of blue color is OK for that.
static const uint8_t L_BYTE_PREVENT_ZERO  = 0b00000001;

//static const uint16_t lineLength = 320;
//static const uint16_t lineCount = 240;
static const uint32_t baud  = 1000000;
static const bool startSendingWhileReading = false;
static const uint8_t uartPixelFormat = UART_PIXEL_FORMAT_RGB565;

#define lineLength 320
#define lineCount 240


//   bool init(I2C_HandleTypeDef * hi2c1);
//   bool setRegister(uint8_t addr, uint8_t val);
//   uint8_t readRegister(uint8_t addr);
//   void setRegisterBitsOR(uint8_t addr, uint8_t bits);
//   void setRegisterBitsAND(uint8_t addr, uint8_t bits);
//   void setManualContrastCenter(uint8_t center);
//   void setContrast(uint8_t contrast);
//   void setBrightness(uint8_t birghtness);
//   void reversePixelBits();
//   void showColorBars(bool transparent);
//
//   inline void waitForVsync(void) __attribute__((always_inline));
//   inline void waitForPixelClockRisingEdge(void) __attribute__((always_inline));
//   inline void waitForPixelClockLow(void) __attribute__((always_inline));
//   inline void waitForPixelClockHigh(void) __attribute__((always_inline));
//   inline void readPixelByte(uint8_t & byte) __attribute__((always_inline));
//
//   virtual void ignoreVerticalPadding();
//   void ignoreHorizontalPaddingLeft();

//void readFrame();
void startNewFrame(uint8_t pixelFormat);
uint8_t formatPixelByteGrayscale(uint8_t pixelByte);
void waitForPreviousUartByteToBeSent();
bool isUSBReady();
void processRgbFrame();
void processFrame();
void waitForVsync();
void waitForPixelClockRisingEdge();
void waitForPixelClockLow();
void waitForPixelClockHigh();
void readPixelByte(uint8_t* byte);
void processNextRgbPixelByte();
uint8_t formatRgbPixelByteH(uint8_t pixelByteH);
uint8_t formatRgbPixelByteL(uint8_t pixelByteL);
void endOfLine();



#endif /* INC_CAMERA_H_ */
