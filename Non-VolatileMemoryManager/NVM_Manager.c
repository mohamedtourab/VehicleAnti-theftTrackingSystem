#include "NVM_Manager.h"

#define NVM_TIME_OUT 	(1000U)

#define NVM_ERROR_ID 	(0U)

static uint8_t  GlobalConfigStructure_ID = 0;
static uint8_t* GlobalDataPointer = 0;
static uint8_t  BusyFlag = 0;
static uint8_t  WriteRequest = 0;
static uint8_t  ReadRequest = 0;
static uint8_t  NVM_InitFLag = 0;
static uint8_t  I2C_WriteDone = 0;
static uint8_t  I2C_ReadDone = 0;
static uint64_t NVM_TimeOut = (NVM_TIME_OUT/NVM_CYCLIC_TIME);

void I2C_TxDone(void)
{
    I2C_WriteDone = 1;
}

void I2C_RxDone(void)
{
    I2C_ReadDone = 1;
}

NVM_CheckType NVM_Init(void)
{
    NVM_CheckType Retval = NVM_OK;
    const NVM_ConfigType* ConfigPtr;
    uint8_t LoopIndex = 0;

    for(LoopIndex = 0; (LoopIndex < NO_OF_NVM_USED) && (Retval == NVM_OK); LoopIndex++)
    {
        ConfigPtr = &NVM_ConfigParam[LoopIndex];
        if( (((ConfigPtr->NVM_LocationAddress) + (ConfigPtr->NVM_NoOfBytes)) < MEMORY_SIZE) && (I2C_InitFlag == 1) )
        {
            Retval = NVM_OK;
        }
        else
        {
            Retval = NVM_NOK;
        }

    }

    if(Retval == NVM_OK)
    {
        NVM_InitFLag = 1;
    }
    else
    {
        NVM_InitFLag = 0;
    }

    return Retval;
}

NVM_CheckType NVM_Write(uint8_t ConfigStructure_ID, uint8_t* DataPointer)
{
    NVM_CheckType RetVal;
	
    if(BusyFlag == 1)
    {
        RetVal = NVM_BUSY;
    }
    else
    {
        RetVal = NVM_NOT_BUSY;
        GlobalConfigStructure_ID = ConfigStructure_ID;
        GlobalDataPointer = DataPointer;
        WriteRequest = 1;
        BusyFlag = 1;
    }

    return RetVal;
}


NVM_CheckType NVM_Read(uint8_t ConfigStructure_ID, uint8_t* DataPointer)
{
    NVM_CheckType RetVal;

    if(BusyFlag == 1)
    {
        RetVal = NVM_BUSY;
    }
    else
    {
        RetVal = NVM_NOT_BUSY;
        GlobalConfigStructure_ID = ConfigStructure_ID;
        GlobalDataPointer = DataPointer;
        ReadRequest = 1;
        BusyFlag = 1;
        
    }

    return RetVal;
}

void NVM_Manager(void)
{
    const NVM_ConfigType* ConfigPtr = &NVM_ConfigParam[GlobalConfigStructure_ID];
    const uint8_t* const LocationAddress = &(ConfigPtr->NVM_LocationAddress);
    static uint8_t NVM_ReadOperationWriteFlag = 0;
    static uint64_t ErrorCounter = 0;
    static NVM_States ManagerState = NVM_UN_INIT;
    switch(ManagerState)
    {
        case NVM_UN_INIT:
        {
            if(NVM_InitFLag == 1)
            {
                ManagerState = NVM_IDLE;
            }
            else
            {
                ManagerState = NVM_UN_INIT;
            }
        }
        break;

        case NVM_IDLE:
        {
            if(WriteRequest == 1)
            {
                ManagerState = NVM_WRITE;
            }
            else if(ReadRequest == 1)
            {
                ManagerState = NVM_READ;
            }
            else
            {
                ManagerState = NVM_IDLE;
            }
        }
        break;

        case NVM_WRITE:
        {
            Concatenator(GlobalConfigStructure_ID, GlobalDataPointer);
            I2C_RequestWrite(ConfigPtr->NVM_SlaveAddress,GlobalDataPointer,((ConfigPtr->NVM_NoOfBytes)+1));
            ManagerState = NVM_WAIT;
        }
        break;

        case NVM_READ:
        {
            /*NVM_ReadOperationWriteFlag this flag handle the read sequence if we executed the write part in the read sequence 
            this flag will be set so we will start the second part in the read sequence which starts from Repeated Start*/
            if(NVM_ReadOperationWriteFlag == 0)
            {
                I2C_RequestWrite(ConfigPtr->NVM_SlaveAddress,(uint8_t*)LocationAddress,1);
            }
            else
            {
                I2C_RequestRead(ConfigPtr->NVM_SlaveAddress,GlobalDataPointer,ConfigPtr->NVM_NoOfBytes);
            }
            ManagerState = NVM_WAIT;
        } 
        break;

        case NVM_WAIT:
        {
            ErrorCounter++;
            if( ErrorCounter > NVM_TimeOut)
            {
                ManagerState = NVM_ERROR;
            }
            else
            {
                if( (I2C_WriteDone == 1) && (WriteRequest == 1) )
                {
                    ManagerState = NVM_IDLE;
                    BusyFlag = 0;
                    GlobalDataPointer = 0;
                    I2C_WriteDone = 0;
                    WriteRequest = 0;
                    ErrorCounter = 0;
                    I2C_GenerateStop(0);
                    (*(ConfigPtr->NVM_WriteDoneCallBackFunction))();
                }
                else if( (I2C_WriteDone == 1) && (ReadRequest == 1) )
                {
                    I2C_WriteDone = 0;
                    ManagerState = NVM_READ;
                    NVM_ReadOperationWriteFlag = 1;
                }
                else if( (I2C_ReadDone == 1) && (ReadRequest == 1) )
                {
                    ManagerState = NVM_IDLE;
                    BusyFlag = 0;
                    GlobalDataPointer = 0;
                    I2C_ReadDone = 0;
                    ReadRequest = 0;
                    NVM_ReadOperationWriteFlag = 0;
                    ErrorCounter = 0;
                    I2C_GenerateStop(0);
                    (*(ConfigPtr->NVM_ReadDoneCallBackFunction))();
                }
                else
                {
                    ManagerState = NVM_WAIT;
                }
            }
        }
        break;

        case NVM_ERROR:
        {
            ManagerState = NVM_IDLE;
            BusyFlag = 0;
            GlobalDataPointer = 0;
            I2C_WriteDone = 0;
            I2C_ReadDone = 0;
            WriteRequest = 0;
            ReadRequest = 0;
            ErrorCounter = 0;
            NVM_ReadOperationWriteFlag = 0;
            I2C_GenerateStop(0);
            (*(ConfigPtr->NVM_ErrorCallBackFunction))(NVM_ERROR_ID);
        }
        break;
    }
}


static void Concatenator(uint8_t GlobalConfigStructure_ID, uint8_t* OriginalArray)
{
    const NVM_ConfigType* ConfigPtr = &NVM_ConfigParam[GlobalConfigStructure_ID];
    uint8_t Counter;

    for(Counter = ((ConfigPtr->NVM_NoOfBytes)); Counter > 0 ; Counter--)
    {
        OriginalArray[Counter] = OriginalArray[Counter-1];
    }
    OriginalArray[0] = (ConfigPtr->NVM_LocationAddress);
}
