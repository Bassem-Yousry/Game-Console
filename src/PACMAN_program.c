/********************************************************************************/
/* Author : Bassem																*/
/* Date   : 27 Sep 2020															*/
/* V01																			*/
/********************************************************************************/
#include <stdio.h>
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

#include "PACMAN_interface.h"
#include "PACMAN_private.h"
#include "PACMAN_config.h"
#define Null (void *) 0

struct Pac{
	u8 x;
	u8 y;
	u8 Move_P;	//move position 0 -> move up , 1-> move left , 2->move right , 3 -> move down
	u16 Color;
};
static volatile  u8 global_u8Level=1;

static volatile  struct Pac PacMan = {15,0,2,0xCFE0};
static volatile  u8 PacMouth =0;

volatile struct Pac Ghost [4] ={
		{7,10,0,0xF800},
		{7,9,2,0xE05B},
		{9 ,10,3,0x0E9C},
		{12 ,8,1,0x07A0}
};
volatile u16 Obstacle [20] ={0, 32606, 1096, 54763, 4096,
		30068, 1284, 48598, 16, 53255, 53271, 53271, 56695,
		4368, 22486, 20480, 1518, 30190, 30190, 1024};


volatile u16 Dots [20] ={};

static void PM_ClearGhosts(){
	u8 Ghost_x , Ghost_y;
	for(int i =0;i<4;i++){
		Ghost_x=Ghost[i].x;
		Ghost_y=Ghost[i].y;
		if( ! GET_BIT(Dots[Ghost_y],Ghost_x) )	//no dot here
		{
			HTFT_voidDrawRect(Ghost_x*8,Ghost_x*8+7,Ghost_y*8,Ghost_y*8+7,0);
			continue;
		}
		HTFT_voidDrawRect(Ghost_x*8, Ghost_x*8+7, Ghost_y*8, Ghost_y*8+7,0);
		HTFT_voidDrawRect(Ghost_x*8+3, Ghost_x*8+4, Ghost_y*8+3, Ghost_y*8+4,0xBE08);
	}

}

static void PM_PrintGhosts(){
	for(int i =0;i<4;i++){
		HTFT_voidDrawGhost(Ghost[i].x*8,Ghost[i].y*8,Ghost[i].Color);
	}
}

u8 PM_ChasePac(u8 Pac_x,u8 Pac_y,u8 Ghost_x,u8 Ghost_y,u8 MoveDir){
	//MoveDir = 0 ->upwards , 1->leftwards , 2->rightwards , 3->downwards
	u8 MovePosition=10;   // -> no move
	if (MoveDir){
		if(Ghost_x>0 && !GET_BIT( Obstacle[Ghost_y],Ghost_x-1))
		{
			if(Ghost_x > Pac_x){ return 2; }
			MovePosition =2;
		}

		if (Ghost_x<15 && !GET_BIT( Obstacle[Ghost_y],Ghost_x+1)){return 1;}
		else if (MovePosition !=10){return 2;}
		else{return 10;}
	}
	else
	{
		if(Ghost_y>0 && !GET_BIT( Obstacle[Ghost_y-1],Ghost_x)){
			if(Ghost_y >Pac_y){ return 3;}
			MovePosition=3;
		}
		if (Ghost_y<19 && !GET_BIT( Obstacle[Ghost_y+1],Ghost_x)){return 0;}
		else if (MovePosition !=10){return 3;}
		else{return 10;}
	}
}

static void PM_UpdateGhosts(){
	u8 Ghost_x , Ghost_y , Ghost_MoveP;
	u8 Pac_x=PacMan.x , Pac_y=PacMan.y;
	u8 Direction;
	for(int i =0;i<4;i++){
		Ghost_x = Ghost[i].x;
		Ghost_y=Ghost[i].y;
		Ghost_MoveP = Ghost[i].Move_P;
		if (Ghost_x==9 && Ghost_y==8 && i!=2)
		{
			Ghost_MoveP=i;
		}
		else if (Ghost_x==14 && Ghost_y==6 && Ghost_y<Pac_y)
		{
			Ghost_MoveP=0;
		}
		else if (Ghost_x==7 && Ghost_y==11 && Ghost_y<Pac_y)
		{
			Ghost_MoveP=0;
		}
		else if (Ghost_x==11 && Ghost_y==16 && i==0)
				{
					Ghost_MoveP=3;
				}
		else if (Ghost_x==13 && Ghost_y==8 && Ghost_y<Pac_y)
		{
				Ghost_MoveP=0;
		}
		while(1)
		{
			if (Ghost_MoveP==0)		// move upwards
			{
				if (Ghost_y<19 && !GET_BIT( Obstacle[Ghost_y+1],Ghost_x) )
				{
					Ghost[i].y++;
					Ghost[i].Move_P = Ghost_MoveP;		//PM_Update new move direction
					break;
				}else
				{
					Direction = PM_ChasePac(Pac_x,Pac_y,Ghost_x,Ghost_y,1);
					if (Direction!=10){ Ghost_MoveP =Direction;}
					else{Ghost_MoveP=3;}
				}
			}
			else if( Ghost_MoveP==2)		// move rightwards
			{
				u8 x =Ghost_x-1;
				if (Ghost_x > 0 && !GET_BIT( Obstacle[Ghost_y],x) )
				{
					Ghost[i].x--;
					Ghost[i].Move_P = Ghost_MoveP;		//PM_Update new move direction
					break;
				}else
				{
					Direction = PM_ChasePac(Pac_x,Pac_y,Ghost_x,Ghost_y,0);
					if (Direction!=10){ Ghost_MoveP =Direction;}
					else{Ghost_MoveP=1;}

				}
			}
			else if(Ghost_MoveP==1)		//move leftwards
			{
				u8 x =Ghost_x+1;
				if(Ghost_x==13 &&Ghost_y==8 ){Ghost_MoveP=0; continue;}
				if (Ghost_x <15 && !GET_BIT( Obstacle[Ghost_y],x) )
				{
					Ghost[i].x++;
					Ghost[i].Move_P = Ghost_MoveP;		//PM_Update new move direction
					break;
				}else
				{
					Direction = PM_ChasePac(Pac_x,Pac_y,Ghost_x,Ghost_y,0);
					if (Direction!=10){ Ghost_MoveP =Direction;}
					else{Ghost_MoveP=2;}

				}
			}
			else if ( Ghost_MoveP==3)	//move downwards
			{
				if (Ghost_y>0 && !GET_BIT( Obstacle[Ghost_y-1],Ghost_x) )
				{
					Ghost[i].y--;
					Ghost[i].Move_P = Ghost_MoveP;		//PM_Update new move direction
					break;
				}else
				{
					Direction = PM_ChasePac(Pac_x,Pac_y,Ghost_x,Ghost_y,1);
					if (Direction!=10){ Ghost_MoveP =Direction;}
					else{Ghost_MoveP=0;}

				}
			}
		}
	}
}

static void PM_PrintDots(){
	for(int i =0 ;i<20;i++){
		for(int j =0 ;j<16;j++){
			if(GET_BIT(Dots[i],j)){
				HTFT_voidDrawRect ( j*8+3,  j*8+4, 3+(i)*8, (i)*8+4,0xBE08);
			}
		}
	}

}
static void PM_PrintObstacles(){
	for(int i =0 ;i<20;i++){
		for(int j =0 ;j<16;j++){
			if(GET_BIT(Obstacle[i],j)){
				HTFT_voidDrawRect ( j*8,  j*8+7, (i)*8, (i)*8+7,0x015F);
			}else{
				HTFT_voidDrawRect ( j*8,  j*8+7, (i)*8, (i)*8+7, 0);
			}
		}
	}
}
static void PM_CheckGhostContact()
{
	u8 Pac_x =PacMan.x , Pac_y=PacMan.y;
	for(int i=0;i<4;i++){
		if(Ghost[i].x==Pac_x && Ghost[i].y==Pac_y)
		{
			TIM4_voidStopInterval();
			HTFT_voidDrawRect (  0, 127, 0,159, 0x6808);
			HTFT_voidDrawText ("U LOST",  125, 50 ,4,0Xfffe);
		}
		else if(i!=3 && Ghost[i].x==Ghost[i+1].x && Ghost[i].y==Ghost[i+1].y)
		{
			Ghost[i+1].Move_P=(Ghost[i+1].Move_P+1)%4;
		}

	}
}

static void PM_Update(){
	PacMouth=(PacMouth+1)%2;
	HTFT_voidDrawPac(PacMan.x*8,PacMan.y*8,PacMan.Color,PacMan.Move_P,PacMouth);
	PM_ClearGhosts();
	PM_UpdateGhosts();
	PM_PrintGhosts();
	PM_CheckGhostContact();
	PM_CheckWin();
}

static void PM_Start(){
	 u16 Copy_u16Dots [20] ={32767, 32929, 64439, 10772, 61439,35467,64251,
							 16393, 57359, 8200, 8200, 8200, 8200, 61167,
							 43049, 45055, 64017, 35345, 35345, 64511};
	 for(int i=0;i<20;i++)
	 {
		 Dots[i]=Copy_u16Dots[i];
	 }
HTFT_voidFillColor ( 0);
HTFT_voidDrawRect (  0,  63, 0, 160, 0);
HTFT_voidDrawRect (  64,  127, 0, 160, 0);
MSTK_voidSetBusyWait(500000);
PM_PrintObstacles();
PM_PrintDots();
PM_PrintGhosts();
HTFT_voidDrawPac(PacMan.x*8,PacMan.y*8,PacMan.Color,2,PacMouth);

}


static void PM_CheckWin(){
	for(int i=0;i<20;i++){
		if (Dots[i] !=0)
		{
			return;
		}
	}
	TIM4_voidStopInterval();
	HTFT_voidDrawRect (  0, 127, 0,159, 0x6808);
	global_u8Level++;
	if (global_u8Level==4){
		HTFT_voidDrawText ("U WON",  117, 60 ,4,0Xfffe);
		return;
	}
	s8 LEVEL[10];
	sprintf(LEVEL,"LEVEL %i",global_u8Level);
	HTFT_voidDrawText (LEVEL,  117, 60 ,3,0Xfefe);
	MSTK_voidSetBusyWait(5000000);
	PM_PrintObstacles();
	PM_PrintDots();
	PM_Start();
	TIM4_voidSetIntervalPeriodic  ( 3000000 -800000*global_u8Level , PM_Update );

}



static void PLAY_PM(){
u8 Pac_x=PacMan.x, Pac_y=PacMan.y;
u8 Key;
u8 x;
Key= HIR_voidReturnKey();
switch (Key){
case 14:
	TIM4_voidStopInterval();
	TIM3_voidStopInterval();
	GameConsoleStart();
	break;
case 12:			//move rightwards
	 x =Pac_x-1;
	if( Pac_x > 0 && !GET_BIT(Obstacle[Pac_y],x) )
	{
		CLR_BIT(Dots[Pac_y], Pac_x-1);
		HTFT_voidDrawRect(PacMan.x*8,PacMan.x*8+7,PacMan.y*8,PacMan.y*8+7,0); // clear old position
		PacMan.x--;
		HTFT_voidDrawPac(PacMan.x*8,PacMan.y*8,PacMan.Color,2,PacMouth); //draw new position
		PacMan.Move_P=2;
	}

	break;

case 13:			//move leftwards
	 x =Pac_x+1;

	if(Pac_x<15 && !GET_BIT(Obstacle[Pac_y] ,x) )
	{
		CLR_BIT(Dots[Pac_y], Pac_x+1);
		HTFT_voidDrawRect(PacMan.x*8,PacMan.x*8+7,PacMan.y*8,PacMan.y*8+7,0); // clear old position
		PacMan.x++;
		HTFT_voidDrawPac(PacMan.x*8,PacMan.y*8,PacMan.Color,1,PacMouth); //draw new position
		PacMan.Move_P=1;
	}

	break;


case 10:		//move upwards
	if(Pac_y<19 && !GET_BIT(Obstacle[Pac_y+1] ,Pac_x) ){
		CLR_BIT(Dots[Pac_y+1], Pac_x);
		HTFT_voidDrawRect(PacMan.x*8,PacMan.x*8+7,PacMan.y*8,PacMan.y*8+7,0); // clear old position
		PacMan.y++;
		HTFT_voidDrawPac(PacMan.x*8,PacMan.y*8,PacMan.Color,0,PacMouth); //draw new position
		PacMan.Move_P=0;
	}

	break;

case 11:		//move downwards
	if(Pac_y>0 && !GET_BIT(Obstacle[Pac_y-1],Pac_x) ){
		CLR_BIT(Dots[Pac_y-1], Pac_x);
		HTFT_voidDrawRect(PacMan.x*8,PacMan.x*8+7,PacMan.y*8,PacMan.y*8+7,0); // clear old position
		PacMan.y--;
		HTFT_voidDrawPac(PacMan.x*8,PacMan.y*8,PacMan.Color,3,PacMouth); //draw new position
		PacMan.Move_P=3;
	}

	break;

case 25: //RESET();
		 //Turn_Num=0;
		 break;	// power Button

}
PM_CheckGhostContact();

}


void PACMAN_voidStart(){
	global_u8Level=1;

PacMan.x=15;
PacMan.y=0;
PacMouth =0;

struct Pac GhostInit [4] ={
		{7,10,0,0xF800},
		{7,9,2,0xE05B},
		{9 ,10,3,0x0E9C},
		{12 ,8,1,0x07A0}
};
for(u8 i =0;i<4;i++){Ghost[i]=GhostInit[i];}
	PM_Start();
	TIM4_voidSetIntervalPeriodic  ( 3000000, PM_Update );
	HIR_voidSetCallBack(PLAY_PM);

}
