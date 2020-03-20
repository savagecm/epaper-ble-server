// 640*384/8*2
#define BUFFER_SIZE 61440

class epdBuffer
{
public:
    epdBuffer()
    {
        bufferIndex = 0;
        epaper1pMode = true;
    }

    static epdBuffer *getInstance()
    {
        static epdBuffer *ins = new epdBuffer();
        return ins;
    }

    bool getEpaperMode()
    {
        return epaper1pMode;
    }

    void setMode(bool is1p)
    {
        epaper1pMode = is1p;
    }

    bool isFull()
    {
        return epaper1pMode ? (bufferIndex >= (BUFFER_SIZE / 2)) : (bufferIndex >= BUFFER_SIZE);
    }

    bool isEmpty()
    {
        return bufferIndex <= 0;
    }

    void clearBuf()
    {
        bufferIndex = 0;
    }

    int getBufferIndex()
    {
        return bufferIndex;
    }

    bool buffSet(char data)
    {
        if (!isFull())
        {
            bufferArray[bufferIndex] = data;
            bufferIndex++;
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
            ret = bufferArray[bufferIndex];
            bufferIndex--;
            return ret;
        }
    }

private:
    int bufferIndex;
    char bufferArray[BUFFER_SIZE];
    bool epaper1pMode;
};
