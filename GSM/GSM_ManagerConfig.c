/**************************************************************************************
*File name: GSM_ManagerConfig.c
*Auther :edited by the user
*Date: 6/3/2018
*Description:
*       This file contains:
*               - A structurs that containes the Groups to manage the GSM 
*Microcontroller: STM32F407VG
***************************************************************************************/ 


const GSM_ManageConfigType GSM_ManageConfigParam =
{

        /*GSM power key Group ID & it's pin mask*/
        2,PIN_MASK_2,

        /*GSM ready to send Group ID*/
        3,PIN_MASK_13,
        /*pin mask of the RTS in the selected GPIO group*/

        /*GSM State Group ID*/
        3,PIN_MASK_14,
        /*pin mask of the DTR in the selected GPIO group*/
        
        /*GSM Ring Group ID*/
        4,PIN_MASK_0,
        /*pin mask of the Ring in the selected GPIO group*/
};