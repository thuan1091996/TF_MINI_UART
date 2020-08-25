//Include Libraries
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/sysctl.c"
#include "driverlib/gpio.h"
#include "driverlib/gpio.c"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
// Define variables
#define RX2_PIN             GPIO_PIN_6
#define TX2_PIN             GPIO_PIN_7
char DATA[9],Byte1,Byte2;
char CORRECT_DATA[90];
uint8_t count_9byte,count_200byte=0;
uint16_t DISTANCE,Distance_L,Distance_H,sum,Distance_DATA[1000];
uint32_t clock;
int main(void)
{
    //Setting system clock at 50MHz
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    //Enable Peripheral for PORT D where contain UART2 Pins
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    //Settings and Configure for UART 2
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
    GPIOPinConfigure(GPIO_PD6_U2RX); GPIOPinConfigure(GPIO_PD7_U2TX);
    GPIOPinTypeUART(GPIO_PORTD_BASE, RX2_PIN|TX2_PIN);
    UARTConfigSetExpClk(UART2_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    clock=SysCtlClockGet();
    //Read data
    while(1)
    {
        while(UARTCharsAvail(UART2_BASE))
        {
            count_9byte=0;
            Byte1=UARTCharGet(UART2_BASE);
            if (Byte1==0x59)
            {
                DATA[count_9byte]=Byte1; //correct byte 1 is 0x59
                count_9byte++;
                char Byte2=UARTCharGet(UART2_BASE);
                if (Byte2==0x59) //correct byte 2 is 0x59
                {
                    DATA[count_9byte]=Byte2;
                    count_9byte++;
                    int8_t bytedem=0;
                    for(bytedem;bytedem<=6;bytedem++) //read the next 7 bytes (byte 3-9) store to DATA[2-8]
                    {
                        DATA[count_9byte]=UARTCharGet(UART2_BASE);
                        count_9byte++;
                    }
                    sum=0;
                    uint8_t i=0;
                    for(i;i<=7;i++) //find sum of the first 8 bytes
                    {
                        sum=sum+DATA[i];
                    }
                    i=0;
                    sum=sum&0x00FF; //8 bit low of sum 8 bytes
                    if (sum==DATA[8]) // correct 9 byte
                    {
                        Distance_L=DATA[2]& 0x00FF;
                        Distance_H=DATA[3]<<8;
                        DISTANCE=Distance_H+Distance_L;
                        Distance_DATA[count_200byte]=DISTANCE;
                        SysCtlDelay(clock/(100*3)); //delay 100ms
                        count_200byte++;
                    } // close if sum correct
                } // close if byte 2 correct
            }// close if byte 1 correct
        } //close while data available
    }  //close while 1
} // close main
