/********************************************************************************/
/* Author : Bassem																*/
/* Date   : 27 Sep 2020															*/
/* V01																			*/
/********************************************************************************/
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


#include "TREX_interface.h"
#include "TREX_private.h"
#include "TREX_config.h"

#define Null (void *) 0

static volatile u8 TRex_Leg=0;
static volatile u8 TRex_LegSpeed=0;
static volatile u8 TRex_Jump =0;
static volatile u8 TRex_CloudTime=0;
static volatile u8 TRex_Speed=0;
struct Dinosaur{
	s8 x;
	s8 y;
	u8 Type;
};
struct Dinosaur  TRex ={3,4,0};
struct Dinosaur  Cactus ={3,80,1};
struct Dinosaur  Bird ={30,110,1};
struct Dinosaur  Cloud ={50,70,1};


static void PLAY_TRex(){

u8 Key;
Key= HIR_voidReturnKey();
switch (Key){
case 14:
	TIM4_voidStopInterval();
	TIM3_voidStopInterval();
	GameConsoleStart();
	break;
case 25:
		TRex_Jump=1;
		 break;	// power Button

}

}

static void TRex_CheckObstacle(){
	s8 x,y;
	u8 Flag=0;
	 x=TRex.x ;
	 y = TRex.y ;
	 if(Bird.y<=y+9 && Bird.y+8 >=y){
	 	if( x >=Bird.x && x < Bird.x+8 )
	 	 {
			Flag=1;
	 	 }
	 }
	if(Cactus.y<=y+9 && Cactus.y+6 >=y){
		 if( x >=Cactus.x && x < Cactus.x+ (Cactus.Type*6) )
		 {
			Flag=1;
		 }
	}
	if(Flag){
			asm("cpsid i" : : : "memory");
			TIM4_voidStopInterval();
			TIM3_voidStopInterval();
			HTFT_voidDrawTRex(x*2,y*2,TRex_Leg,2,0xffff);
			HTFT_voidDrawCactus(2*Cactus.x,2*Cactus.y,Cactus.Type,0xffff);
			asm("cpsie i" : : : "memory");
	}
}

static void TRex_ObstacleMove()
{

	HTFT_voidDrawBird(2*Bird.x,2*Bird.y,Bird.Type,0);
	Bird.y-=2;
	Bird.Type=(Bird.Type+1)%2;
	HTFT_voidDrawBird(2*Bird.x,2*Bird.y,Bird.Type,0xffff);

	HTFT_voidDrawCactus(2*Cactus.x,2*Cactus.y,Cactus.Type,0);
	Cactus.y-=2;
	HTFT_voidDrawCactus(2*Cactus.x,2*Cactus.y,Cactus.Type,0xffff);
	TRex_CheckObstacle();
	if(Cactus.y<=-12)
	{
		Cactus.Type=(Cactus.Type)%2+1;
		Cactus.y=80;
	}
	if(Bird.y<=-10 && Cactus.y<70)
	{
		Bird.y=Cactus.y+60;
		TRex_Speed++;
		TIM4_voidSetIntervalPeriodic  ( 450000 -TRex_Speed*8000, TRex_ObstacleMove );

	}
	if(Cloud.y<=-20)
	{
		Cloud.x+=3;
		if(Cloud.x>60){Cloud.x-=20;}
		Cloud.y=80;
	}
	if(!TRex_CloudTime){
		HTFT_voidDrawCloud(2*Cloud.x,2*Cloud.y,0);
		Cloud.y--;
		HTFT_voidDrawCloud(2*Cloud.x,2*Cloud.y,0xffff);
	}
TRex_CloudTime=(TRex_CloudTime+1)%2;

}


static void TRex_Update()
{
	int x,y;
	 x=TRex.x ;
	 y = TRex.y ;

	if(TRex_Jump)
	{
		//__disable_irq();
		asm("cpsid i" : : : "memory");
		HTFT_voidDrawRect (  x*2, (x)*2 +34 , 8,40 , 0);
		asm("cpsie i" : : : "memory");
		x+=4;
		TRex.x+=4;
		if (TRex.x>=40)
		{
			TRex_Jump=0;
		}
	}
	else if(TRex.x!=3)
	{
		asm("cpsid i" : : : "memory");
		HTFT_voidDrawRect (  x*2, (x)*2 +34 , 8,40 , 0);
		asm("cpsie i" : : : "memory");
		x-=4;
		TRex.x-=4;
	}

	if(TRex.x!=3){HTFT_voidDrawTRex(TRex.x*2,TRex.y*2,TRex_Leg,2,0xffcc);}

	if(!TRex_LegSpeed)
	{
	 x =TRex.x;
	 y =TRex.y;
	TRex_Leg=(TRex_Leg+1)%2;
	HTFT_voidDrawRect (  x*2, (4+x)*2 , 0,30 , 0);
	HTFT_voidDrawTRex(x*2,y*2,TRex_Leg,2,0xffcc);
	}
	TRex_CheckObstacle();

	TRex_LegSpeed=(TRex_LegSpeed+1)%4;

}

void TREX_voidStart(){
	HTFT_voidFillColor ( 0);
	TRex.x=3;
	Cactus.y=80;
	TRex_Leg=0;
	Bird.y=110;
	TRex_LegSpeed=0;
	TRex_Jump =0;
	TRex_CloudTime=0;
	TRex_Speed=0;
	TIM3_voidSetIntervalPeriodic  ( 120000, TRex_Update );
	TIM4_voidSetIntervalPeriodic  ( 450000, TRex_ObstacleMove );
	HIR_voidSetCallBack(PLAY_TRex);
}
