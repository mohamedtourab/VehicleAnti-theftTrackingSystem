#include "I2C_Driver.h"
#include "I2C_Manager.h"


int main(void)
{
        States WriteFlag=I2C_IDLE,ReadFlag=I2C_IDLE;
        uint8_t Data_output=0,WriteDone=0,ReadDone=0;
        I2C_Init();
        while(1)
        {
                if((WriteFlag!=I2C_GENERATE_STOP)&&(WriteDone==0))
                {
                        WriteFlag=I2C_Write(0xA0/*Slave Address*/,0x0A/*Address of memory location*/,0x52/*data to be stored*/);
                }
                else if((WriteFlag==I2C_GENERATE_STOP)&&(WriteDone==0))
                {
                        WriteFlag = I2C_Write(0xA0/*Slave Address*/,0x0A/*Address of memory location*/,0x52/*data to be stored*/);
                        if(WriteFlag==I2C_IDLE)
                        {
                                WriteDone=1;
                        }
                        else
                        {/*MISRA*/}
                }
                if((WriteDone==1)&&(ReadDone==0)&&(ReadFlag!=I2C_GENERATE_STOP))
                {                        
                                ReadFlag=I2C_Read(0xA0/*SlaveAddress*/,0x0A/*LocationAddress*/,&Data_output/*pointer to variable to store data read in*/);
                }
                else if((WriteDone==1)&&(ReadDone==0)&&(ReadFlag==I2C_GENERATE_STOP))
                {
                        ReadFlag=I2C_Read(0xA0/*SlaveAddress*/,0x0A/*LocationAddress*/,&Data_output/*pointer to variable to store data read in*/);
                        if(ReadFlag==I2C_IDLE)
                        {
                                ReadDone=1;
                        }
                        else
                        {/*MISRA*/}
                }
        }
}