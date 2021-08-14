/********************************************************************************/
/* Author : Bassem																*/
/* Date   : 21 Aug 2020															*/
/* V01																			*/
/********************************************************************************/
#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "DIO_interface.h"
#include "STK_interface.h"
#include "NVIC_interface.h"
#include "EXTI_interface.h"
#include "AFIO_interface.h"


#include "HIR_interface.h"
#include "HIR_config.h"
#include "HIR_private.h"

static volatile u8   u8StartFlag       = 0;
static volatile u32  u32FrameData[60] = {0};
static volatile u8   u8EdgeCounter     = 0;
static volatile u8   u8Data            = 0;
static volatile u8   u8RepeatCode      = 39;

void(*HIR_Play)(void);


static void HIR_voidPlay(void)
{
	HIR_Play();
	
}
static void HIR_voidGetData(void)
{
		u8 i;
	 	u8Data = 0;

		if ( (u32FrameData[0] >= 100000) && (u32FrameData[0] <= 140000) )
		{
			for (i=0;i<8;i++)
			{
				if (  (u32FrameData[17+i] >= 19000) && (u32FrameData[17+i] <=23000) )
				{
					SET_BIT(u8Data,i);
				}

				else
				{
					CLR_BIT(u8Data,i);
				}
			}

			HIR_voidPlay();
		}

		else
		{
			/* Invalid Frame */
		}

}

static void HIR_voidTakeAction(void)
{

	u8StartFlag     = 0;
	u32FrameData[0] = 0;
	u8EdgeCounter   = 0;
	u8RepeatCode	= 39;
	for(u8 i =38 ; i<50;i++)
	{
		u32FrameData[i] = 0;
	}
}

static void HIR_voidGetFrame(void)
{
	if (u8StartFlag == 0)
	{
		/* Start Timer */
		MSTK_voidSetIntervalSingle(1000000,HIR_voidTakeAction);
		u8StartFlag = 1;
	}

	else
	{
		if(u8EdgeCounter>=60){return;}
		u32FrameData[u8EdgeCounter] = MSTK_u32GetElapsedTime();
		MSTK_voidSetIntervalSingle(1000000,HIR_voidTakeAction);
		u8EdgeCounter++;
		if(u8EdgeCounter==35){HIR_voidGetData();}
		else if(u8EdgeCounter==u8RepeatCode && u8Data)
		{
			HIR_voidPlay();
			u8RepeatCode+=2;
		}

	}
}




void HIR_voidInit(void){
	MSTK_voidInit();

#if	PORT_SELECTION == PortA
DIO_voidSetPinDirection (GPIOA, OUTPUT_LINE,0b0100);
#elif PORT_SELECTION == PortB
DIO_voidSetPinDirection (GPIOB,OUTPUT_LINE,0b0100);
#elif PORT_SELECTION == PortC
DIO_voidSetPinDirection (GPIOC,OUTPUT_LINE,0b0100);
#endif

if (OUTPUT_LINE <=4){
	MNVIC_voidEnableInterrupt(OUTPUT_LINE +6);
	MEXTI_voidSetCallBack(OUTPUT_LINE , HIR_voidGetFrame);

}
else if (OUTPUT_LINE <=9){
	MNVIC_voidEnableInterrupt(23);
	MEXTI_voidSetCallBack(5 , HIR_voidGetFrame);

}
else if (OUTPUT_LINE <=15){
	MNVIC_voidEnableInterrupt(40);
	MEXTI_voidSetCallBack(10 , HIR_voidGetFrame);

}

MEXTI_voidEnableEXTI(OUTPUT_LINE);
MEXTI_voidSetSignalLatch(OUTPUT_LINE , FALLING);	

MAFIO_voidSetEXTIConfiguration(OUTPUT_LINE ,PORT_SELECTION);
}

void HIR_voidSetCallBack(void(*ptr)(void)){
	
	HIR_Play=ptr;	
	
}

u8 HIR_voidCheckRC(){
	return u8RepeatCode;
}

u8 HIR_voidReturnKey(){
	
	return u8Data;
}
