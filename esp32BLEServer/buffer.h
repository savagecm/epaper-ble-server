#define BUFFER_SIZE 61400

class epdBuffer
{
public:
    static epdBuffer *getInstance()
    {
        static epdBuffer *ins = new epdBuffer();
        return ins;
    }
    
    epdBuffer()
    {
        bufferIndex = 0;
        epaper1pMode = true;
    }
   bool getEpaperMode()
   {
    return epaper1pMode;
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
    void setMode(bool is1p)
    {
        epaper1pMode = is1p;
    }

private:
    int bufferIndex;
    char bufferArray[BUFFER_SIZE];
    bool epaper1pMode;
};
