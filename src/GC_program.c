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

s8 GameList [10][30]={
		{"                XO"},
		{"            PONG"},
		{"            T REX"},
		{"         PAC MAN"},
		{"         TETRIS"},
		{"    THE SNACK"},
		{"    ICY TOWER"},
		{"EGG  COLLECT"},
		{"SPACE INVAD"},
		{"BRICK BREAK"}
};

u8 x=56;
u8 Switch=1;
u8 GameNum ='1';


void GameConsoleSwitch(){
	u8 Key;
	Key= HIR_voidReturnKey();
	switch (Key){
	case 13:
		HTFT_voidFillColor ( 0x0817);
		if(GameNum > '1'){
		Switch=0;
		TIM3_voidSetIntervalPeriodic(100000,GameConsoleUpdate);
		}
		break;

	case 12:
		HTFT_voidFillColor ( 0x0817);
		if(GameNum -1 < '9'){
		Switch=1;
		TIM3_voidSetIntervalPeriodic(100000,GameConsoleUpdate);
		}
		break;

	case 25:
		HTFT_voidFillColor ( 0x0817);
		StartGame();
			 //Turn_Num=0;
			 break;	// power Button
	}
}

void StartGame()
{

	switch(GameNum){
	case '1':
		XO_voidStart();
		break;
	case '2':
		PONG_voidStart();
		break;
	case '3':
		TREX_voidStart();
		break;
	case '4':
		PACMAN_voidStart();
		break;
	case '5':
		 HTETRIS_voidTetrisInit();
		break;
	case '6':
		SNACK_voidStart();
		break;
	case '7':
		ICYTOWER_voidStart();
		break;
	case '8':
		EC_voidStart();
		break;
	case '9':
		SPACEINVADERS_voidStart();
		break;
	case ':':
		BB_voidStart();
		break;

	}
}

void GameConsoleUpdate()
{
	u8 Size = GameNum-49;
	Size/=7;
	if(GameNum==':')
		{HTFT_voidDrawText("10",100,80,5,0x0817);}
	else{HTFT_voidDrawChar(GameNum,x,80,5,0x0817);}

	HTFT_voidDrawText(GameList[GameNum-49],127,20,2,0x0817);

	if(Switch){
		x+=10;
		if(x>128)
			{
			TIM3_voidStopInterval();
			x=56;
			GameNum++;
			HTFT_voidDrawText(GameList[GameNum-49],127,20,2,0xffff);
			}
	}
	else{
		x-=10;
		if(x==242)
			{
				TIM3_voidStopInterval();
				x=56;
				GameNum--;
				HTFT_voidDrawText(GameList[GameNum-49],127,20,2,0xffff);

			}
	}
	if(GameNum==':'){HTFT_voidDrawText("10",100,80,5,0xffff);}
	else{HTFT_voidDrawChar(GameNum,x,80,5,0xffff);}

	if(GameNum >'1') {HTFT_voidDrawChar('<',113,67,3,0xCE20);}
	else{HTFT_voidDrawChar('<',113,67,3,0x0817);}
	if(GameNum<='9') {HTFT_voidDrawChar('>',0,67,3,0xCE20);}
	else{HTFT_voidDrawChar('>',0,67,3,0x0817);}

}

void GameConsoleStart(){
	HTFT_voidFillColor ( 0x0817);
	HTFT_voidDrawChar('>',0,67,3,0xCE20);
	HTFT_voidDrawChar(GameNum,56,80,5,0xffff);
	HIR_voidSetCallBack(GameConsoleSwitch);
	HTFT_voidDrawText(GameList[GameNum-49],127,20,2,0xffff);

}
