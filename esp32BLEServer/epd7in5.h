
#include "buffer.h"

#define PIN_SPI_SCK 13
#define PIN_SPI_DIN 14
#define PIN_SPI_CS 15
#define PIN_SPI_BUSY 25 //19
#define PIN_SPI_RST 26  //21
#define PIN_SPI_DC 27   //22

/* Pin level definition ------------------------------------------------------*/
#define LOW 0
#define HIGH 1

#define GPIO_PIN_SET 1
#define GPIO_PIN_RESET 0

#define UBYTE uint8_t
#define UWORD uint16_t
#define UDOUBLE uint32_t

class epd7in5
{
  public:
    static epd7in5 *getInstance()
    {
        static epd7in5 *ins = new epd7in5();
        return ins;
    }

    bool init()
    {
        EPD_initSPI();
        EPD_Init_7in5();
    }

    bool writeData(char data)
    {
      if (epdBuffer::getInstance()->isEmpty())
        {
            init();
        }
        if (!epdBuffer::getInstance()->isFull())
        {
            epdBuffer::getInstance()->buffSet(data);
        }
        if (epdBuffer::getInstance()->isFull())
        {
            flushBuf();
        }
        return true;
    }
    
    void clearBuffer()
    {
        epdBuffer::getInstance()->clearBuf();
    }

    int getBufferSize()
    {
        return epdBuffer::getInstance()->getBufferIndex();
    }

    void EPD_initSPI()
    {
        pinMode(PIN_SPI_BUSY, INPUT);
        pinMode(PIN_SPI_RST, OUTPUT);
        pinMode(PIN_SPI_DC, OUTPUT);
        pinMode(PIN_SPI_SCK, OUTPUT);
        pinMode(PIN_SPI_DIN, OUTPUT);
        pinMode(PIN_SPI_CS, OUTPUT);
        digitalWrite(PIN_SPI_CS, HIGH);
        digitalWrite(PIN_SPI_SCK, LOW);
    }

    /* The procedure of sending a byte to e-Paper by SPI -------------------------*/
    void EpdSpiTransferCallback(byte data)
    {
        //SPI.beginTransaction(spi_settings);
        digitalWrite(PIN_SPI_CS, GPIO_PIN_RESET);
        for (int i = 0; i < 8; i++)
        {
            if ((data & 0x80) == 0)
                digitalWrite(PIN_SPI_DIN, GPIO_PIN_RESET);
            else
                digitalWrite(PIN_SPI_DIN, GPIO_PIN_SET);
            data <<= 1;
            digitalWrite(PIN_SPI_SCK, GPIO_PIN_SET);
            digitalWrite(PIN_SPI_SCK, GPIO_PIN_RESET);
        }
        digitalWrite(PIN_SPI_CS, GPIO_PIN_SET);
    }

    /* Sending a byte as a command -----------------------------------------------*/
    void EPD_SendCommand(byte command)
    {
        digitalWrite(PIN_SPI_DC, LOW);
        EpdSpiTransferCallback(command);
    }

    /* Sending a byte as a data --------------------------------------------------*/
    void EPD_SendData(byte data)
    {
        digitalWrite(PIN_SPI_DC, HIGH);
        EpdSpiTransferCallback(data);
    }

    /* Waiting the e-Paper is ready for further instructions ---------------------*/
    void EPD_WaitUntilIdle()
    {
        //0: busy, 1: idle
        while (digitalRead(PIN_SPI_BUSY) == 0)
            delay(100);
    }

    /* Send a one-argument command -----------------------------------------------*/
    void EPD_Send_1(byte c, byte v1)
    {
        EPD_SendCommand(c);
        EPD_SendData(v1);
    }

    /* Send a two-arguments command ----------------------------------------------*/
    void EPD_Send_2(byte c, byte v1, byte v2)
    {
        EPD_SendCommand(c);
        EPD_SendData(v1);
        EPD_SendData(v2);
    }

    /* Send a three-arguments command --------------------------------------------*/
    void EPD_Send_3(byte c, byte v1, byte v2, byte v3)
    {
        EPD_SendCommand(c);
        EPD_SendData(v1);
        EPD_SendData(v2);
        EPD_SendData(v3);
    }

    /* Send a four-arguments command ---------------------------------------------*/
    void EPD_Send_4(byte c, byte v1, byte v2, byte v3, byte v4)
    {
        EPD_SendCommand(c);
        EPD_SendData(v1);
        EPD_SendData(v2);
        EPD_SendData(v3);
        EPD_SendData(v4);
    }

    /* Send a five-arguments command ---------------------------------------------*/
    void EPD_Send_5(byte c, byte v1, byte v2, byte v3, byte v4, byte v5)
    {
        EPD_SendCommand(c);
        EPD_SendData(v1);
        EPD_SendData(v2);
        EPD_SendData(v3);
        EPD_SendData(v4);
        EPD_SendData(v5);
    }

    /* This function is used to 'wake up" the e-Paper from the deep sleep mode ---*/
    void EPD_Reset()
    {
        digitalWrite(PIN_SPI_RST, LOW);
        delay(200);
        digitalWrite(PIN_SPI_RST, HIGH);
        delay(200);
    }

    // should send data to epaper and clear the buff
    void flushBuf()
    {
        // Enumerate all of image data bytes
        while (!epdBuffer::getInstance()->isEmpty())
        {
            // Get current byte from obtained image data
            char value = epdBuffer::getInstance()->buffGet();
            if (epdBuffer::getInstance()->getEpaperMode())
            {
                // Processing of 4 1-bit pixels to 4 4-bit pixels:
                // black(value 0) to bits 0000, white(value 1) to bits 0011
                EPD_SendData((byte)((value & 0x80) ? 0x30 : 0x00) + ((value & 0x40) ? 0x03 : 0x00));
                EPD_SendData((byte)((value & 0x20) ? 0x30 : 0x00) + ((value & 0x10) ? 0x03 : 0x00));
                EPD_SendData((byte)((value & 0x08) ? 0x30 : 0x00) + ((value & 0x04) ? 0x03 : 0x00));
                EPD_SendData((byte)((value & 0x02) ? 0x30 : 0x00) + ((value & 0x01) ? 0x03 : 0x00));
            }
            else
            {
                int A = (value)&3;
                if (A == 3)
                    A = 4;
                if (A == 1)
                    A = 3;
                int B = (value >> 2) & 3;
                if (B == 3)
                    B = 4;
                if (B == 1)
                    B = 3;
                int C = (value >> 4) & 3;
                if (C == 3)
                    C = 4;
                if (C == 1)
                    C = 3;
                int D = (value >> 6) & 3;
                if (D == 3)
                    D = 4;
                if (D == 1)
                    D = 3;
                // Write the word into e-Paper's memory
                EPD_SendData((A << 4) + B);
                EPD_SendData((C << 4) + D);
            }
        }
        // Refresh
        EPD_SendCommand(0x12); // DISPLAY_REFRESH
        delay(100);
        EPD_WaitUntilIdle();
        // Sleep
        EPD_SendCommand(0x02); // POWER_OFF
        EPD_WaitUntilIdle();
        EPD_Send_1(0x07, 0xA5); // DEEP_SLEEP
        epdBuffer::getInstance()->clearBuf();
    }

    int EPD_Init_7in5()
    {
        EPD_Reset();
        EPD_Send_2(0x01, 0x37, 0x00);       //POWER_SETTING
        EPD_Send_2(0x00, 0xCF, 0x08);       //PANEL_SETTING
        EPD_Send_3(0x06, 0xC7, 0xCC, 0x28); //BOOSTER_SOFT_START
        EPD_SendCommand(0x4);               //POWER_ON
        EPD_WaitUntilIdle();
        EPD_Send_1(0x30, 0x3C);                   //PLL_CONTROL
        EPD_Send_1(0x41, 0x00);                   //TEMPERATURE_CALIBRATION
        EPD_Send_1(0x50, 0x77);                   //VCOM_AND_DATA_INTERVAL_SETTING
        EPD_Send_1(0x60, 0x22);                   //TCON_SETTING
        EPD_Send_4(0x61, 0x02, 0x80, 0x01, 0x80); //TCON_RESOLUTION
        EPD_Send_1(0x82, 0x1E);                   //VCM_DC_SETTING: decide by LUT file
        EPD_Send_1(0xE5, 0x03);                   //FLASH MODE
        EPD_SendCommand(0x10); //DATA_START_TRANSMISSION_1
        delay(2);
        return 0;
    }
};
