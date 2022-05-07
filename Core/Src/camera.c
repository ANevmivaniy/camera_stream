/*
 * camera.c
 *
 *  Created on: Apr 17, 2022
 *      Author: alnem
 */

#include "../Inc/camera.h"
#include <stdbool.h>

#define CDC_DATA_HS_MAX_PACKET_SIZE                 512U  /* Endpoint IN & OUT Packet size */

extern USBD_HandleTypeDef hUsbDeviceFS;
extern enum Resolution resolution;

uint8_t verticalPadding = 0;
uint8_t lineBuffer [lineLength * 2]; // Two bytes per pixel
uint8_t * lineBufferProcessByte;
bool lineBufferProcessingByteFormatted;
bool lineBufferProcessParityFirstByte;
uint16_t frameCounter = 0;
uint16_t processedByteCountDuringCameraRead = 0;
uint8_t horizontalBytePaddingLeft = 0;
uint8_t horizontalBytePaddingRight = 0;

bool flag = true;
bool isEndLine = false;

typedef struct
{
  uint32_t data[CDC_DATA_HS_MAX_PACKET_SIZE / 4U];      /* Force 32bits alignment */
  uint8_t  CmdOpCode;
  uint8_t  CmdLength;
  uint8_t  *RxBuffer;
  uint8_t  *TxBuffer;
  uint32_t RxLength;
  uint32_t TxLength;

  __IO uint32_t TxState;
  __IO uint32_t RxState;
} USBD_CDC_HandleTypeDef;


void waitForPreviousUartByteToBeSent() {
  while(!isUSBReady()); //wait for byte to transmit
}


bool isUSBReady() {
	USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*) hUsbDeviceFS.pClassData;

	return hcdc->TxState == 0;
}

uint8_t formatPixelByteGrayscale(uint8_t pixelByte) {
  return pixelByte | 0b00000001; // make pixel color always slightly above 0 since zero is end of line marker
}

void startNewFrame(uint8_t pixelFormat) {
  uint8_t buffer = formatPixelByteGrayscale(*lineBufferProcessByte);

  waitForPreviousUartByteToBeSent();
  buffer = 0x00;
  CDC_Transmit_FS(&buffer, 1);
  waitForPreviousUartByteToBeSent();
  buffer = COMMAND_NEW_FRAME;
  CDC_Transmit_FS(&buffer, 1);


  // frame width
  waitForPreviousUartByteToBeSent();
  buffer = (lineLength >> 8) & 0xFF;
  CDC_Transmit_FS(&buffer, 1);
  waitForPreviousUartByteToBeSent();
  buffer = lineLength & 0xFF;
  CDC_Transmit_FS(&buffer, 1);

  // frame height
  waitForPreviousUartByteToBeSent();
  buffer = (lineCount >> 8) & 0xFF;
  CDC_Transmit_FS(&buffer, 1);
  waitForPreviousUartByteToBeSent();
  buffer = lineCount & 0xFF;
  CDC_Transmit_FS(&buffer, 1);

  // pixel format
  waitForPreviousUartByteToBeSent();
  buffer = (pixelFormat);
  CDC_Transmit_FS(&buffer, 1);
}

void waitForVsync() {
	while(!OV7670_VSYNC);
}

void waitForPixelClockRisingEdge() {
  waitForPixelClockLow();
  waitForPixelClockHigh();
}

void waitForPixelClockLow() {
  while(OV7670_PIXEL_CLOCK);
}

void waitForPixelClockHigh() {
  while(!OV7670_PIXEL_CLOCK);
}

void readPixelByte(uint8_t* byte) {
	//OV7670_READ_PIXEL_BYTE(byte);
  if (isEndLine) {
	if (flag) {
		*byte = 0b00000000;
	} else {
		*byte = 0b00011111;
	}

	flag = !flag;
  } else {
	  OV7670_READ_PIXEL_BYTE(byte);
  }
}

// RRRRRGGG
uint8_t formatRgbPixelByteH(uint8_t pixelByteH) {
  // Make sure that
  // A: pixel color always slightly above 0 since zero is end of line marker
  // B: odd number of bits for H byte under H_BYTE_PARITY_CHECK and H_BYTE_PARITY_INVERT to enable error correction
  if (pixelByteH & H_BYTE_PARITY_CHECK) {

	return pixelByteH & (~H_BYTE_PARITY_INVERT);
  } else {

	return pixelByteH | H_BYTE_PARITY_INVERT;
  }
}

// GGGBBBBB
uint8_t formatRgbPixelByteL(uint8_t pixelByteL) {
	  // Make sure that
	  // A: pixel color always slightly above 0 since zero is end of line marker
	  // B: even number of bits for L byte under L_BYTE_PARITY_CHECK and L_BYTE_PARITY_INVERT to enable error correction
	  if (pixelByteL & L_BYTE_PARITY_CHECK) {
	    return pixelByteL | L_BYTE_PARITY_INVERT | L_BYTE_PREVENT_ZERO;
	  } else {
	    return (pixelByteL & (~L_BYTE_PARITY_INVERT)) | L_BYTE_PREVENT_ZERO;
	  }
}

void processNextRgbPixelByte() {
  // Format pixel bytes and send out in different cycles.
  // There is not enough time to do both on faster frame rates.
  if (!lineBufferProcessingByteFormatted) {
    if (lineBufferProcessParityFirstByte) {
      *lineBufferProcessByte = formatRgbPixelByteH(*lineBufferProcessByte);
    } else {
      *lineBufferProcessByte = formatRgbPixelByteL(*lineBufferProcessByte);
    }
    lineBufferProcessingByteFormatted = true;
    lineBufferProcessParityFirstByte = !lineBufferProcessParityFirstByte;

  } else if (isUSBReady()) {
      uint8_t buffer = formatPixelByteGrayscale(*lineBufferProcessByte);
      CDC_Transmit_FS(&buffer, 1);
    lineBufferProcessByte++;
    lineBufferProcessingByteFormatted = false;
  }
}

void endOfLine()   {
  uint8_t buffer = formatPixelByteGrayscale(*lineBufferProcessByte);

  waitForPreviousUartByteToBeSent();
  buffer = 0x00;
  CDC_Transmit_FS(&buffer, 1);

  waitForPreviousUartByteToBeSent();
  buffer = COMMAND_END_OF_LINE;
  CDC_Transmit_FS(&buffer, 1);
}


void ignoreVerticalPadding() {
  for (uint8_t i = 0; i < verticalPadding; i++) {
    for (uint16_t x = 0; x < resolution * 2 + horizontalBytePaddingLeft + horizontalBytePaddingRight - 1; x++) {
      waitForPixelClockRisingEdge();
    }
    // After the last pixel byte of a line there is a very small pixel clock pulse.
    // To avoid accidentally counting this small pulse we measure the length of the
    // last pulse and then wait the same time to add a byte wide delay at the
    // end of the line.
    volatile uint16_t pixelTime = 0;
    while(OV7670_PIXEL_CLOCK) pixelTime++;
    while(!OV7670_PIXEL_CLOCK) pixelTime++;
    while(pixelTime) pixelTime--;
  }
}

void ignoreHorizontalPaddingLeft() {
  for (uint8_t x = 0; x < horizontalBytePaddingLeft; x++) {
    waitForPixelClockRisingEdge();
  }
}

void processRgbFrame() {
  uint16_t y = 0;
  waitForVsync();
  //ignoreVerticalPadding();
  do {
    lineBufferProcessByte = &lineBuffer[0];
    lineBufferProcessingByteFormatted = false;
    lineBufferProcessParityFirstByte = true; // Always start with high byte

    //ignoreHorizontalPaddingLeft();

    for (uint16_t x = 0; x < lineLength * 2; x++) {
      waitForPixelClockRisingEdge();
      if (x < (lineLength - 160) * 2) {
    	 readPixelByte(&lineBuffer[x]);
      } else {
    	  isEndLine = true;
    	  readPixelByte(&lineBuffer[x]);
    	  isEndLine = false;
      }

      if (startSendingWhileReading) {
        processNextRgbPixelByte();
      }
    }

    // Debug info to get some feedback how mutch data was processed during line read.
    processedByteCountDuringCameraRead = lineBufferProcessByte - (&lineBuffer[0]);

    // send rest of the line
    while (lineBufferProcessByte < (&lineBuffer[lineLength* 2])) {
      processNextRgbPixelByte();
    }

    endOfLine();

    y++;
  } while (y < lineCount);
}

void debugPrint(const char * string) {
    uint8_t buffer = formatPixelByteGrayscale(*lineBufferProcessByte);
    waitForPreviousUartByteToBeSent();
    buffer = 0x00;
    CDC_Transmit_FS(&buffer, 1);

    waitForPreviousUartByteToBeSent();
    buffer = COMMAND_DEBUG_DATA;
    CDC_Transmit_FS(&buffer, 1);

    waitForPreviousUartByteToBeSent();
    buffer = strlen(string);
    CDC_Transmit_FS(&buffer, 1);

    for (uint16_t i=0; i < strlen(string); i++) {
    	waitForPreviousUartByteToBeSent();
        buffer = string[i];
        CDC_Transmit_FS(&buffer, 1);
    }
}

void processFrame() {
	processedByteCountDuringCameraRead = 0;
	startNewFrame(uartPixelFormat);
//	 //__disable_irq();
//
	processRgbFrame();

//	//__enable_irq();
	frameCounter++;
	char str[20];
	snprintf(str, 20, "Frame %d %d", frameCounter, processedByteCountDuringCameraRead);
	debugPrint(str);
}


