/********************************************************************************/
/* Author : Bassem																*/
/* Date   : 27 Oct 2020															*/
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

#include "ICYTOWER_interface.h"
#include "ICYTOWER_private.h"
#include "ICYTOWER_config.h"

#define Null (void *) 0


static volatile u8 IcyTower_JumpHeight;
static volatile u8 IcyTower_Jump =0;
static volatile u8 IcyTower_Score=0;
static volatile u8 IcyTower_ScoreInc =0;
static volatile u8 IcyTower_Falling =0;
static volatile u8 IcyTower_Lose =0;


static u16 IcyTower [16] = {15996, 7800, 1632, 15996, 10212, 16380,
					3120, 15996, 32766, 55707, 59799, 61431,
					62415,31806, 16380, 8184};
 struct IcyTower{
	u8 x;
	u8 y;
	u8 Length;
};
static struct IcyTower  Character={12,2,0};
static struct IcyTower  Floor [10];



static void IcyTower_UpdateFloor(){
	u8 x , y , Length;
	for(int i =0 ; i<10;i++)
	{
		x=Floor[i].x;
		y=Floor[i].y;
		Length=Floor[i].Length;
		if(y<37){HTFT_voidDrawRect(x*4-4,(x+Length)*4,y*4,y*4+8,0x0800);}
		if(y!=0)
		{
			Floor[i].y--;
		}
		else if(i==0){Floor[i].y=255;}
		else{
			Floor[i].y=90;
		}
	}
}

static void IcyTower_DrawFloor(){
	u8 x , y , Length;
	for(int i =0 ; i<10;i++)
	{
		x=Floor[i].x;
		y=Floor[i].y;
		Length=Floor[i].Length;
		if(y<37 ){HTFT_voidDrawFrameRect(x*4,(x+Length)*4,y*4,y*4+3,0x18A1,0x0020);}
	}

}

static void IcyTower_GameOver(){
	TIM3_voidStopInterval();
	HTFT_voidFillColor ( 0);
	if(IcyTower_Lose){
		HTFT_voidDrawText("U LOST",110,80,3,0xfff);
		IcyTower_UpdateScore();
	}
}

static u8 IcyTower_FallDown(){
	if (IcyTower_Jump){return 0;}
	u8 Floor_x , Floor_y,Character_x=Character.x,Character_y=Character.y , Length;

	for(int i =0 ; i<10;i++)
	{
		Floor_x=Floor[i].x;
		Floor_y=Floor[i].y;
		Length=Floor[i].Length;
		if(Floor_y>=37){continue;}
		if(Floor_y+2==Character_y){Character.y--;return 0;}
		if(Floor_y+1== Character_y && Character_x+2>=Floor_x && Character_x<Floor_x+Length  ){ return 0;}
	}
	if(!IcyTower_Jump){Character.y-=2;}
	return 1;
}

static void IcyTower_UpdateCharacter(){
	u8 x=Character.x,y=Character.y;
	if( y>252 )
	{
		IcyTower_Lose=1;
		IcyTower_GameOver();
		return ;
	}
	HTFT_voidDrawRect(x*4-4,(x)*4+20,y*4,y*4+16,0x0800);
	Character.y--;
	IcyTower_Falling=IcyTower_FallDown();
	if( IcyTower_Jump)
	{
		if(Character.y>=IcyTower_JumpHeight)
		{
			IcyTower_Jump=0;
		}
		else{Character.y+=3;}
	}
	y=Character.y;
	HTFT_voidDraw_u16Frame(IcyTower,x*4,y*4,16,16,1,0xF9C0);
}

static void IcyTower_Update(){

	IcyTower_UpdateFloor();
	IcyTower_DrawFloor();
	IcyTower_UpdateCharacter();
	if(IcyTower_Lose)
	{
		TIM3_voidStopInterval();
		return;
	}
	IcyTower_ScoreInc=(IcyTower_ScoreInc+1)%3; // update Score every 3 cycles
	if(! IcyTower_ScoreInc ){
		IcyTower_Score++;
		IcyTower_UpdateScore();
	}
}

static void IcyTower_UpdateScore(){
	u8 IcyTower_Speed;
	HTFT_voidDrawRect (0,  127, 151 ,159,0xF800);
	s8 SCORE[20];
	sprintf(SCORE,"SCORE : %i",IcyTower_Score);
	IcyTower_Speed = IcyTower_Score/100; // increase speed and difficulty every 100 points
	if(IcyTower_Speed && !IcyTower_Lose){
	TIM3_voidSetIntervalPeriodic  ( 1200000 -IcyTower_Speed*200000, IcyTower_Update );
	}
	HTFT_voidDrawText (SCORE,  124, 152 ,1,0xD5A0);
}


static void PLAY_ICYTOWER(){

u8 Key;
u8 x =Character.x, y =Character.y;
Key= HIR_voidReturnKey();
HTFT_voidDrawRect(x*4-3,x*4+18,y*4,y*4+18,0x0800);

switch (Key){
case 14:
	IcyTower_GameOver();
	TIM4_voidStopInterval();
	TIM3_voidStopInterval();
	GameConsoleStart();
	return;
	break;
case 12:
	if(x>0)
	{
		Character.x--;
	}
	break;
case 13 :
	if(x<31)
		{
			Character.x++;
		}
	break;
case 25:
		if(!IcyTower_Score){TIM3_voidSetIntervalPeriodic  ( 1200000, IcyTower_Update );}
		if(IcyTower_Jump || IcyTower_Falling){return;}
		IcyTower_Jump=1;
		IcyTower_JumpHeight=Character.y+8;
		break;

}
HTFT_voidDraw_u16Frame(IcyTower,x*4,y*4,16,16,1,0xF9C0);
}



void ICYTOWER_voidStart(){
	 struct IcyTower  Floor_Start [10]={
			{0,1,31},
			{8,11,16},
			{23,21,8},
			{15,31,10},
			{0,41,15},
			{18,51,8},
			{2,61,20},
			{6,71,9},
			{20,81,8},
			{12,91,18}
	};
	for(u8 i=0;i<10;i++){
		Floor[i]=Floor_Start[i];
	}
	Character.x=12;
	Character.y=2;
	IcyTower_JumpHeight=0;
	IcyTower_Jump =0;
	IcyTower_Score=0;
	IcyTower_ScoreInc =0;
	IcyTower_Falling =0;
	IcyTower_Lose =0;

		/* TFT Init */
	HTFT_voidFillColor ( 0x0800);

	IcyTower_DrawFloor();
	HTFT_voidDraw_u16Frame(IcyTower,Character.x*4,Character.y*4,16,16,1,0xF9C0);

	IcyTower_UpdateScore();
	
	/* IR Initialization */
	HIR_voidSetCallBack(PLAY_ICYTOWER);

}
