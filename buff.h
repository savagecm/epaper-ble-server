
#include"epd.h"
#define BUFF_SIZE 61400
int bufIndex;
char bufArr[BUFF_SIZE];
bool epaper1pMode;
bool buffSet(char data)
{
    if (!isFull())
    {
        bufArr[bufIndex] = data;
        bufIndex++;
    }
    else
    {
        return false;
    }
}

char buffGet()
{
    char ret = 0;
    if (isEmpty())
    {
        return ret;
    }
    else
    {
        ret = bufArr[bufIndex];
        bufIndex--;
        return ret;
    }
}
void setMode(bool is1p)
{
    epaper1pMode = is1p;
}

bool isFull()
{
    return epaper1pMode ? (bufIndex >= (BUFF_SIZE / 2)) : (bufIndex >= BUFF_SIZE);
}
bool isEmpty()
{
    return bufIndex <= 0;
}

void clearBuf()
{
    bufIndex = 0;
}
// should send data to epaper and clear the buff
void flushBuf()
{
    // Enumerate all of image data bytes
    while (!isEmpty())
    {
        // Get current byte from obtained image data
        char value = buffGet();
        if (epaper1pMode)
        {
            // Processing of 4 1-bit pixels to 4 4-bit pixels:
            // black(value 0) to bits 0000, white(value 1) to bits 0011
            EPD_SendData((byte)((valueA & 0x80) ? 0x30 : 0x00) + ((valueA & 0x40) ? 0x03 : 0x00));
            EPD_SendData((byte)((valueA & 0x20) ? 0x30 : 0x00) + ((valueA & 0x10) ? 0x03 : 0x00));
            EPD_SendData((byte)((valueA & 0x08) ? 0x30 : 0x00) + ((valueA & 0x04) ? 0x03 : 0x00));
            EPD_SendData((byte)((valueA & 0x02) ? 0x30 : 0x00) + ((valueA & 0x01) ? 0x03 : 0x00));
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
    clearBuf();
}
