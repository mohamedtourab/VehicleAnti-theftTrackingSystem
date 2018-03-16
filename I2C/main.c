#include "I2C_Driver.h"
#include "I2C_Manager.h"

int main(void)
{
       // States WriteState=I2C_GENERATE_START,ReadState=I2C_GENERATE_START,OldWriteState=I2C_GENERATE_START,OldReadState=I2C_GENERATE_START;
       // volatile uint8_t Data_output=0,WriteDone=0,ReadDone=0,AllDone=0,WriteError=0;
        ReturnStruct WriteReturn;
        ReturnStruct ReadReturn;
        uint8_t DataOutput=0;

        GPIO_Init();
				I2C_Init();
        GPIO_Write(2,1);//Green
        do
        {
                WriteReturn = I2C_Write(0xA0,0x07,0x52,0);
        }while(WriteReturn.CurrentState != I2C_WRITE_DONE);
        GPIO_Write(3,1);//Orange
        do
        {
						ReadReturn = I2C_Read(0xA0,0x07,&DataOutput,0);
        }while(ReadReturn.CurrentState != I2C_READ_DONE);
				if(DataOutput==0x52)
				{
					GPIO_Write(4,1);//Red
				}
				else if(DataOutput==0xA1)
				{
					GPIO_Write(5,1);//Blue
				}
       
       
        // while(1)
        // {
        //        while(write)
        // }






































//         while(1)
//         {
//                 if((WriteState!=I2C_GENERATE_STOP && OldWriteState!=I2C_ERROR_CHECK)&&(WriteDone==0))
//                 {
//                         OldWriteState = WriteState;
//                         WriteState=I2C_Write(0xA0/*Slave Address*/,0x0A/*Address of memory location*/,0x52/*data to be stored*/);
//                 }
//                 else if((WriteState==I2C_GENERATE_STOP && OldWriteState!=I2C_ERROR_CHECK)&&(WriteDone==0))
//                 {
//                         OldWriteState = WriteState;
//                         WriteState = I2C_Write(0xA0/*Slave Address*/,0x0A/*Address of memory location*/,0x52/*data to be stored*/);
//                         if(WriteState==I2C_GENERATE_START)
//                         {
//                                 WriteDone=1;
//                         }
//                         else
//                         {/*MISRA*/}
//                 } 
//                 else if(WriteState==I2C_GENERATE_STOP && OldWriteState==I2C_ERROR_CHECK)
//                 {
//                         WriteState = I2C_GENERATE_START;
//                         OldWriteState = I2C_GENERATE_START;       
//                         WriteDone=0;
//                         ReadDone=0;
//                         AllDone=0;
//                 }               
//                 if((WriteDone==1)&&(ReadDone==0)&&(ReadState!=I2C_GENERATE_STOP && OldReadState!=I2C_ERROR_CHECK))
//                 {                        
//                         OldReadState = ReadState;
//                         ReadState=I2C_Read(0xA0/*SlaveAddress*/,0x0A/*LocationAddress*/,&Data_output/*pointer to variable to store data read in*/);
//                 }
//                 else if((WriteDone==1)&&(ReadDone==0)&&(ReadState==I2C_GENERATE_STOP && OldReadState!=I2C_ERROR_CHECK))
//                 {
//                         OldReadState = ReadState;
//                         ReadState=I2C_Read(0xA0/*SlaveAddress*/,0x0A/*LocationAddress*/,&Data_output/*pointer to variable to store data read in*/);
//                         if(ReadState==I2C_GENERATE_START)
//                         {
//                                 ReadDone=1;
//                         }
//                         else
//                         {/*MISRA*/}
//                 }
//                 else if(ReadState==I2C_GENERATE_STOP && OldReadState==I2C_ERROR_CHECK)
//                 {
//                         ReadState = I2C_GENERATE_START;
//                         OldReadState = I2C_GENERATE_START;       
//                         ReadDone=0;
//                         AllDone=0; 
//                 }
//                 else if((WriteDone==1)&&(ReadDone==1))
//                 {
//                         AllDone = 1;
//                 }
//                 else
//                 {/*MISRA*/}
//         }
 }