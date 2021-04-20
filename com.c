/**********************************************
 * name: T5L protocol v1.2
 * Copyright(c) MTF 2016~2030. www.mtf123.club
 * All Rights Reserved
 * ******************************************/

#include "stdio.h"

#define SUM_OPEN 1 //open frame verify function

static u8 Head[] = {0XAA}; //head define
static u8 End[] = {0XCC, 0X33, 0XC3, 0X3C}; //end define

/*rec uart data and judge frame(need use in uart rec int)
data: frame data(not include head and end byte)
num: data length*/
u8 frameFlag = 0; //has one frame
u8 frameStart = 0, frameOverTime = 0;
/*read data over time error, need use in timer irq*/
void readDataTimer(void)
{
    static u8 cnt = 0;
    if(frameStart)
    {
        if (++cnt>=10)
        {
            cnt = 0;
            frameOverTime = 1;
        }
    }
    else
    {
        frameOverTime = 0;
        cnt = 0;
    }
}
void readData(u8 *data, int *num)
{
    static u8 step = 0;
    static int i = 0;
    static u16 k = 0, sum = 0;
    u8 rec = 0;
    
    if(frameFlag) //a frame without handle
        return;
    if (frameOverTime) //frame rec over time error
    {
        frameOverTime = 0; //initialize
        i = 0;
        step = 0;
        *num -= 6;
        frameStart = 0;
    }
    if(uart_rec(rec)) //uart has data
    {
        switch (step)
        {
        case 0:
            if (rec == 0xAA) //check head
            {
                *num = 0;
                frameStart = 1;
                step++;
                i = 0;
            }
            break;
        case 1:
            *num = *num+1; //data
            data[*num-1] = rec;
            break;
        default:
            step = 0;
            break;
        }

        if(frameStart) //check end
        {
            if(i==0&&rec == End[0])
            {
                    i++;
            }
            else if(i==1)
            {
                if (rec == End[1])
                    i++;
                else
                    i = 0;
            }
            else if(i==2)
            {
                if (rec == End[2])
                    i++;
                else
                    i = 0;
            }
            else if(i==3)
            {
                if (rec == End[3])
                    i++;
                else
                    i = 0;
            }
        }
        if(i==4)
        {
            if (SUM_OPEN) //enable sum verify
            {
                for (k = 0, sum = 0; k < *num - 6; k++)
                    sum += data[k];
                if (sum == (((u16)data[*num - 6] << 8) + data[*num - 5]))
                {
                    i = 0;
                    step = 0;
                    *num -= 6;
                    frameStart = 0;
                    frameFlag = 1; //has one frame
                }
                else
                {
                    i = 0;
                    step = 0;
                    *num -= 6;
                    frameStart = 0; //error
                }
            }
            else
            {
                i = 0;
                step = 0;
                *num -= 4;
                frameStart = 0;
                frameFlag = 1; //has one frame
            }
        }
    }
}

/*send one frame to display
data: user data (not include head&end byte)
num: data length*/
void sendData(u8 *data, int *num)
{
    u8 i = 0;
    u16 sum = 0;

    //head
    for (i = 0; i<sizeof(Head); i++)
        uart_putc(Head[i]);

    //BODY
    for (i = 0; i<*num; i++)
        uart_putc(data[i]);

    if (SUM_OPEN) //enable sum verify
    {
        for (i = 0; i < *num; i++)
            sum += data[i];
        uart_putc((u8)(sum>>8));
        uart_putc((u8)sum);
    }

    //end
    for (i = 0; i < sizeof(End); i++)
        uart_putc(End[i]);
}

/**************demo**************/
#define buffer_size 32
u8 frameData[buffer_size], total = 0;
void Uart_INT_irq(void) //uart int
{
    readData(&frameData, &total);
}

void main(void) //demo
{
    u8 data = 0, i = 1; 
    sendData(&data, &i); //Inquire command
    while (1)
    { 
        if(frameFlag)
        {
            frameFlag = 0;
            if(frameData[0]==0) //Inquire command
                printf("function = %#X, length = %d\r\n", 
                        frameData[2], total);
        }
    }
}
