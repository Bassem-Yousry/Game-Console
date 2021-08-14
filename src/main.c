#include <stdio.h>
#include <stdlib.h>
#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include"GPTIM_interface.h"
#include "DIO_interface.h"
#include "STK_interface.h"
#include "SPI_interface.h"
#include "RCC_interface.h"
#include "EXTI_interface.h"
#include "NVIC_interface.h"
#include "TFT_interface.h"
#include "HIR_interface.h"
#include "GC_interface.h"


#include "BB_interface.h"
#include "EGG-COLLECTOR_interface.h"
#include "HTETRIS_interface.h"
#include "ICYTOWER_interface.h"
#include "PACMAN_interface.h"
#include "PONG_interface.h"
#include "SNACK_interface.h"
#include "SPACEINVADERS_interface.h"
#include "TREX_interface.h"
#include "XO_interface.h"


#define Null (void *) 0

void main(void)
 {

	/* RCC Initialization */
	RCC_voidInitSysClock();
	RCC_voidEnableClock(RCC_APB2,2);   /* GPIOA */
	RCC_voidEnableClock(RCC_APB2,12);  /* SPI1  */
	RCC_voidEnableClock(RCC_APB1,1);  /* TIM3  */
	RCC_voidEnableClock(RCC_APB1,2);  /* TIM4  */
	/* Timer Initialization */
	MNVIC_voidEnableInterrupt(29);
	MNVIC_voidSetPriority(29,4<<1);

	MNVIC_voidEnableInterrupt(30);
	MNVIC_voidSetPriority(30,3<<1);

	TIM3_voidInit();
	TIM4_voidInit();

	/* Pin Initializations */
	DIO_voidSetPinDirection(GPIOA,3,0b1000);
	DIO_voidSetPinValue(GPIOA,3,1);//input pull up
	DIO_voidSetPinDirection(GPIOA,4,0b1000);
	DIO_voidSetPinValue(GPIOA,4,1);
	DIO_voidSetPinDirection(GPIOA,8,0b1000);
	DIO_voidSetPinValue(GPIOA,8,1);

	DIO_voidSetPinDirection(GPIOA,1,0b0010);  /* A0   */
	DIO_voidSetPinDirection(GPIOA,2,0b0010);  /* RST  */
	DIO_voidSetPinDirection(GPIOA,5,0b1001);  /* CLK  */
	DIO_voidSetPinDirection(GPIOA,7,0b1001);  /* MOSI */

	/* Init STK */
	MSTK_voidInit();

	/* SPI Init */
	MSPI1_voidInit();


	/* TFT Init */
	HTFT_voidInitialize();

	HIR_voidInit();
	GameConsoleStart();
	//TIM3_voidSetIntervalPeriodic(100000,GameConsoleUpdate);
	while(1){



	}

}


