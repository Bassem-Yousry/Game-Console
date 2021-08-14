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


#include "EGG-COLLECTOR_interface.h"
#include "EGG-COLLECTOR_private.h"
#include "EGG-COLLECTOR_config.h"

#define Null (void *) 0

u8 EC_Lose=0;
u8 EC_Score=255;
u8 Egg_X=1;
u8 Egg_Type=0;
u8 EggFrames [2][10]=
{
	{60, 126, 255, 255, 255, 255, 255, 126, 60, 24}, // Egg
	{60, 126, 255, 255, 255, 126, 60 , 16 , 16, 8}	 //Bomb
};
struct EggCol{
	u8 x;
	u8 y;
	u8 GoldBomb;     //0-> Normal Egg , 1->Golden Egg , 2->Bomb
	struct EggCol * Next;
};
static  struct EggCol * EggList = Null;
static  struct EggCol  Bowl ;


static void EC_DeleteList(struct EggCol ** Head){
	struct EggCol * node =*Head ;
	struct EggCol * deleteNode ;
	while(node!=Null){
		deleteNode=node;
		node=node->Next;
		free(deleteNode);
	}
}

static void EC_AddEgg(u8 Egg_x,u8 Egg_y){

	struct EggCol * Egg = Null;
	Egg = (struct EggCol*)malloc(sizeof(struct EggCol)) ;
	Egg->x=Egg_x;
	Egg->y=Egg_y;
	Egg->GoldBomb=0;
	Egg->Next=EggList;
	EggList=Egg;

}

static void EC_PrintBowl(){
	u8 Bowl_x =Bowl.x ;
	for (int i=0;i<16;i++)
	{
		if(i==Bowl_x)
		{
			HTFT_voidDrawRect (  Bowl_x*8, Bowl_x*8+31, 0,2,0xffff);
			HTFT_voidDrawRect (  Bowl_x*8, Bowl_x*8+2, 0,10,0xffff);
			HTFT_voidDrawRect (  Bowl_x*8+29, Bowl_x*8+31, 0,10,0xffff);
			HTFT_voidDrawRect (  Bowl_x*8+3, Bowl_x*8+28, 3,10,0);
			i+=3;
			continue;
		}
		HTFT_voidDrawRect (  i*8, i*8+7, 0,10,0);
	}
}

static void EC_UpdateEgg(){
	if (EggList==Null){return;}
	struct EggCol * LastEgg, *PreLastEgg=Null;
	EggList->y--;

	LastEgg=EggList;
	while(LastEgg->Next != Null){
		PreLastEgg=LastEgg;
		LastEgg=LastEgg->Next;
		LastEgg->y--;
	}
	if (LastEgg->y==0)
	{
		if(PreLastEgg ==Null)
		{
			free(LastEgg);
			EggList=Null;
			return;
		}
		free(LastEgg);
		PreLastEgg->Next=Null;
	}
}

static void EC_PrintEggs(){
	u8 Bowl_x =Bowl.x ;
	u8 Egg_x,Egg_y;
	struct EggCol * Egg;
		Egg=EggList;
		while(Egg != Null){
			 Egg_x=Egg->x; Egg_y=Egg->y;
			if(Egg->GoldBomb==1)
			{
				HTFT_voidDraw_u8Frame(EggFrames[0],Egg_x*4, Egg_y*4,8,10,1, 0xFEA0);
				//HTFT_voidDrawEgg ( Egg->x*4, Egg->y*4, 0xFEA0,0);
			}
			else if(Egg->GoldBomb==2)
			{
				HTFT_voidDraw_u8Frame(EggFrames[1],Egg_x*4, Egg_y*4,8,10,1, 0xc000);
				//HTFT_voidDrawEgg ( Egg->x*4, Egg->y*4, 0xC000,1);
			}
			else{HTFT_voidDraw_u8Frame(EggFrames[0],Egg_x*4, Egg_y*4,8,10,1, 0xFFFF);}
			if(Egg_y<2 && ( Egg_x/2 >= Bowl_x && Egg_x/2 <= Bowl_x+3) )
			{
				if(Egg->GoldBomb==1){EC_Score+=9;}
				else if(Egg->GoldBomb==2){
					EC_Lose=1;
					EC_GameOver();
					return;
					}
				EC_UpdateScore();
			}
			Egg = Egg->Next;
		}
}

static void EC_GameOver(){
	TIM3_voidStopInterval();
	TIM4_voidStopInterval();
	HTFT_voidFillColor ( 0);
	if(EC_Lose){
	HTFT_voidDrawText("U LOST",110,80,3,0xfff);
	}
	EC_DeleteList(&EggList);

}

static void EC_ClearEgg(){
	struct EggCol * Egg;
		Egg=EggList;
		while(Egg != Null){
			HTFT_voidDraw_u8Frame(EggFrames[0],Egg->x*4, Egg->y*4,8,10,1, 0);
			if(Egg->y < 3 ){
				EC_PrintBowl();
			}
			Egg = Egg->Next;
		}
}


static void EC_Start(){
Bowl.y=0;
Bowl.x=7;
HTFT_voidFillColor ( 0);
HTFT_voidDrawRect (  0,  63, 0, 160, 0);
HTFT_voidDrawRect (  64,  127, 0, 160, 0);
MSTK_voidSetBusyWait(500000);
EC_PrintBowl();

}
static void EC_Update(){
	EC_ClearEgg();
	EC_UpdateEgg();
	EC_PrintEggs();
	Egg_X=(Egg_X+7)%29;
	Egg_Type++;
}
static void EC_Add(){
	EC_AddEgg(Egg_X,35);
	if(Egg_Type >210){
		EggList->GoldBomb=2;
	}
	if(Egg_Type >235){
		EggList->GoldBomb=1;
	}
}
static void EC_UpdateScore(){
	u8 Speed;
	HTFT_voidDrawRect (0,  127, 149 ,159,0);
	EC_Score++;
	s8 SCORE[20];
	sprintf(SCORE,"SCORE : %i",EC_Score);
	Speed = EC_Score/10; // increase speed and difficulty every 10 points
	if(Speed){
	TIM4_voidSetIntervalPeriodic  ( 700000-Speed*32000, EC_Update );
	TIM3_voidSetIntervalPeriodic  ( 11000000-Speed*505050, EC_Add );
	}
	HTFT_voidDrawText (SCORE,  124, 150 ,1,0xffff);
}

static void PLAY_EGG_COLLECTOR(){
u8 Bowl_x=Bowl.x;
u8 Key;
Key= HIR_voidReturnKey();
switch (Key){

case 12:			//move rightwards
	if( Bowl_x > 0 )
	{
		Bowl.x--;
	}
	EC_PrintBowl();
	break;

case 13:			//move leftwards

	if(Bowl_x <12  )
	{
		Bowl.x++;
	}
	EC_PrintBowl();
	break;

case 14:
	EC_GameOver();
	TIM4_voidStopInterval();
	TIM3_voidStopInterval();
	GameConsoleStart();
	break;
		 break;	// power Button

}

}



void EC_voidStart(){
	EggList = Null;
	EC_Lose=0;
	EC_Score=255;
	Egg_X=1;
	Egg_Type=0;
	EC_Start();
	EC_UpdateScore();
	TIM4_voidSetIntervalPeriodic  ( 700000, EC_Update );
	TIM3_voidSetIntervalPeriodic  ( 11000000, EC_Add );
	HIR_voidSetCallBack(PLAY_EGG_COLLECTOR);

}
