

#define BUFF_SIZE 61400
int bufIndex;
char bufArr[BUFF_SIZE];
bool epaper1pMode;
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


