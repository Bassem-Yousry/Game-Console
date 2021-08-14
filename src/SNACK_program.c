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

#include "SNACK_interface.h"
#include "SNACK_private.h"
#include "SNACK_config.h"
#define Null (void *) 0
u8 TS_MoveDir=1;	//	0->left , 1->up , 2->right, 3-> down
u8 TS_Score = 0;
struct Snack{
	u8 x;
	u8 y;
	struct Snack * next ;
};
struct Snack * TS_Head =Null;
struct Snack * TS_Apple=Null;

 static void TS_DrawApple(){

	 HTFT_voidDrawCircle (  TS_Apple->x, TS_Apple->y, 0xF104);

 }
static void TS_ScreenUpdate(){
			 HTFT_voidDrawRect (  10,  110, 10, 120, 0x2EA5);
}
static void TS_Lose(){
	 HTFT_voidDrawRect (  0,127, 0,160, 0);
	 MSTK_voidStopInterval();
	 HTFT_voidDrawText ("U LOST", 110,60 , 2,0xfe0);
	 TIM3_voidStopInterval();

}
static void TS_DeleteList(struct Snack ** Head){
	struct Snack * node =*Head ;
	struct Snack * deleteNode ;
	while(node!=Null){
		deleteNode=node;
		node=node->next;
		free(deleteNode);
	}
}
static void TS_AddNode(){
	struct Snack * node;
	node = TS_Head;
	while(node->next != Null){
		node=node->next;
	}
	struct Snack *New =Null;
	New = (struct Snack*)malloc(sizeof(struct Snack)) ;
	New->next=Null;
	New->x=128;
	node->next = New;

}
static void TS_UpdateSnake(){
	u8 stack[40][2];
	u8 flag =0;
	u8 count =0;
	struct Snack * node;
	node = TS_Head;
	while(node != Null){
		if(flag){
			stack[count][0] = node->x;
			stack[count][1] = node->y;
			HTFT_voidDrawRect (  node->x,  node->x +10, node->y, node->y+10, 0x2EA5);
			node->x=stack[count-1][0];
			node->y=stack[count-1][1];
			count++;
			node=node->next;
		}
		else{
			flag =1;
			HTFT_voidDrawRect (  node->x,  node->x +10, node->y, node->y+10, 0x2EA5);
			stack[count][0] = node->x;
			stack[count][1] = node->y;
			count++;
			node=node->next;
	}}

}

static void TS_PrintSnake(u16 Color){
	u16 SnackFrame[10]={508, 1022, 2047, 1911, 1651, 1911, 1022, 508, 248, 112};
	struct Snack * node;
	node = TS_Head;
	HTFT_voidDraw_u16Frame(SnackFrame, TS_Head->x, TS_Head->y,11,10,1,Color);
	//HTFT_voidDrawRect (  TS_Head->x+2,  TS_Head->x +8, TS_Head->y, TS_Head->y+10, Color);
	while(node->next != Null){
		node=node->next;
		HTFT_voidDrawRect (  node->x+2, node->x + 8, node->y+2, node->y+8, Color);
		if(TS_Head->x==node->x && TS_Head->y==node->y){TS_Lose();}
	}

}


static void TS_MoveUpdate(){
	if (TS_MoveDir==0){
		TS_Head->x +=5;
		if(TS_Head->x >100){
			TS_Head->x =10;
			}
		}
	else if (TS_MoveDir==1){
		TS_Head->y +=5;
		if(TS_Head->y > 110){
			TS_Head->y=10;
		}
	}
	else if (TS_MoveDir==2){
			TS_Head->x -=5;
			if(TS_Head->x < 10){
				TS_Head->x=100;
			}
		}
	else if (TS_MoveDir==3){
			TS_Head->y -=5;
			if(TS_Head->y < 10){
				TS_Head->y=110;
			}
		}
}
static void TS_Start(){
	TS_UpdateSnake();
	TS_MoveUpdate();
	TS_DrawApple();
	TS_PrintSnake(0x099D);
	if(TS_Head->x==TS_Apple->x && TS_Head->y==TS_Apple->y){
		TS_UpdateScore();
		TS_AddNode();
		TS_Apple->y =TS_Head->x;
		TS_Apple->x+=30;
		if(TS_Apple->x > 110){TS_Apple ->x=30;}
	}
}
static void TS_UpdateScore(){
	TS_Score++;
	s8 score[20];
	sprintf(score,"SCORE : %i",TS_Score);
	HTFT_voidDrawRect (  60, 120,144,160, 0x01C0);
	HTFT_voidDrawText (score,  120, 145 ,1,0Xffff);
	TIM3_voidSetIntervalPeriodic(1000000-TS_Score*20000,TS_Start);

}
static void PLAY_SNACK(){

u8 Key;
Key= HIR_voidReturnKey();
switch (Key){
case 14 :
	TIM4_voidStopInterval();
	TIM3_voidStopInterval();
	GameConsoleStart();
	break;
case 13:
	if(TS_MoveDir!=2){TS_MoveDir=0;}

	break;

case 10:
	if(TS_MoveDir!=3){TS_MoveDir=1;}

	break;
case 12:
	if(TS_MoveDir!=0){TS_MoveDir=2;}

	break;
case 11:
	if(TS_MoveDir!=1){TS_MoveDir=3;}

	break;

case 25:SNACK_voidStart();
		 //Turn_Num=0;
		 break;	// power Button

}
}





void SNACK_voidStart(){
	HTFT_voidFillColor ( 0x01C0);
	HTFT_voidDrawRect (  0,  127, 0, 140, 0x0280);
	HTFT_voidDrawText ("SCORE : 0",  120, 145 ,1,0Xffff);
	TS_DeleteList(&TS_Head);
	TS_DeleteList(&TS_Apple);
	TS_ScreenUpdate();
	TIM3_voidSetIntervalPeriodic(1000000,TS_Start);
    TS_Head = (struct Snack*)malloc(sizeof(struct Snack));
    TS_Head->next =Null;
    TS_Apple = (struct Snack*)malloc(sizeof(struct Snack));
    TS_Apple->next=Null;
    TS_Head->x=10;
	TS_Head->y= 20;
	TS_Apple->x=10;
	TS_Apple->y=20;
	HIR_voidSetCallBack(PLAY_SNACK);

}
