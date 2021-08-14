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


#include "BB_interface.h"
#include "BB_private.h"
#include "BB_config.h"

#define Null (void *) 0


static volatile  u8 BB_Level  = 1;
static volatile  u8 BB_HMove = 1;			//1 ->increase  x postion
static volatile  u8 BB_VMove = 1;			//1 ->increase  y postion
struct Brick{
	u8 x;
	u8 y;
};

volatile struct Brick global_u8Ball = {14,6};
volatile struct Brick global_u8Paddle = {3,2};
static volatile  u16 BB_Area [17]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xff,0xff};
static volatile  u8 BB_Lives =3;

static void BB_Print_Ball(u16 Color){
	u8 x = global_u8Ball.x , y = global_u8Ball.y;
	HTFT_voidDrawCircle(x*4,y*4,Color);

}

static void BB_UpdateLive(){
	if(BB_Lives==0)
	{
		TIM4_voidStopInterval();
		HTFT_voidDrawRect (  0, 127, 0,159, 0x1880);
		HTFT_voidDrawText ("U LOST",  125, 60 ,4,0Xfffe);
		return;
	}
	BB_Lives--;
	HTFT_voidDrawRect (  0, 127, 140,159, 0x1082);
	for(int i=0;i<=BB_Lives;i++)
	{
		HTFT_voidDrawHeart( 4+(16*i) , 143,0xf800);
	}
	s8 SCORE[20];
	sprintf(SCORE,"LEVEL %i",BB_Level);
	HTFT_voidDrawText (SCORE,  120, 145 ,1,0Xfffe);

	global_u8Ball.x=(global_u8Paddle.x+1)*2;
	global_u8Ball.y=6;
}
static void BB_Screen_Update(){

	for(int y =14;y<17;y++){
		for(int x =0 ; x<8;x++){
			if( !GET_BIT(BB_Area[y],x) )    //empty block
			{
				HTFT_voidDrawRect (  x*16,  x*16+16, y*8, y*8+8, 0);
			}else{
				HTFT_voidDrawFrameRect (  x*16,  x*16+16, y*8, y*8+8, 0xF000 ,0xffff);
			}
		}
	}


}

static void BB_CheckWin(){
	for (int i =14; i<17;i++)
	{
			if (BB_Area[i] !=0){return;}
	}
	TIM4_voidStopInterval();
	HTFT_voidDrawRect (  0, 127, 0,159, 0x6808);
	HTFT_voidDrawText ("U WON",  115, 50 ,4,0Xfffe);

}

static void BB_UpdateBall(){
	BB_Print_Ball(0);
	u8  Ball_y=global_u8Ball.y/2 ,
		Ball_x=global_u8Ball.x/2,
		Paddle_x =global_u8Paddle.x,
		Paddle_y =global_u8Paddle.y;

	if (BB_VMove){										//moving upward
			if( Ball_y  == 16 || GET_BIT(BB_Area[Ball_y+1],Ball_x/2) )
			{
				BB_VMove=0;
				global_u8Ball.y--;
				if(Ball_y!=16 && GET_BIT(BB_Area[Ball_y+1],Ball_x/2 ) ){
					CLR_BIT( BB_Area[Ball_y+1], Ball_x/2) ;
					BB_Screen_Update();
				}
			}else
			{
				global_u8Ball.y++;
			}

		}
		else{												//moving downward
			if( Ball_y  == 0 ) {
						BB_VMove=1;
						BB_UpdateLive();
						return;
					}
			else if ( GET_BIT(BB_Area[Ball_y-1],Ball_x/2) )
			{
				BB_VMove=1;
				global_u8Ball.y++;
				CLR_BIT(BB_Area[Ball_y-1],Ball_x/2);
				BB_Screen_Update();
			}
			else if (Ball_y ==Paddle_y ){
				if(Ball_x/2==Paddle_x-1){BB_VMove=1;BB_HMove=0;}
				else if (Ball_x/2==Paddle_x){BB_VMove=1;}
				else if (Ball_x/2==Paddle_x+1){BB_VMove=1;BB_HMove=1;}
				else {global_u8Ball.y--;}

			}
			else
			{
				global_u8Ball.y--;
			}

		}
	if (BB_HMove){										//moving leftward
		if( Ball_x  == 15 || GET_BIT(BB_Area[Ball_y],(Ball_x+1)/2 ) )
		{
			BB_HMove=0;
			global_u8Ball.x--;
			if(GET_BIT(BB_Area[Ball_y],(Ball_x+1)/2 ) ){
				CLR_BIT( BB_Area[Ball_y], (Ball_x+1)/2 ) ;
				BB_Screen_Update();
			}
		}else
		{
			global_u8Ball.x++;
		}

	}
	else{												//moving rightward
		if( Ball_x  == 0 ) {
					BB_HMove=1;
					global_u8Ball.x++;
				}
		else if ( GET_BIT(BB_Area[Ball_y],(Ball_x-1)/2 ) )
		{
			BB_HMove=1;
			global_u8Ball.x++;
			CLR_BIT(BB_Area[Ball_y],(Ball_x-1)/2);
			BB_Screen_Update();
		}else
		{
			global_u8Ball.x--;
		}

	}
	BB_Print_Ball(0xFDC0);
}

static void BB_PrintPaddle(){
	u8 Paddle_x =global_u8Paddle.x -1, Paddle_y =global_u8Paddle.y;
	for (int i =0;i<8;i++){
		if(i==Paddle_x ){
			HTFT_voidDrawRect (  (i)*16+5,  (i)*16+16, Paddle_y*8-4, Paddle_y*8+4, 4);
			HTFT_voidDrawRect (  (i+1)*16,  (i+1)*16+16, Paddle_y*8-4, Paddle_y*8+4, 4);
			HTFT_voidDrawRect (  (i+2)*16,  (i+2)*16+11, Paddle_y*8-4, Paddle_y*8+4, 4);
			i+=2;
			continue;
		}
		HTFT_voidDrawRect (  (i)*16,  (i)*16+16, Paddle_y*8-4, Paddle_y*8+4, 0);

	}

}

static void BB_Start(){

	BB_PrintPaddle();
	BB_UpdateBall();
	if (global_u8Ball.y>=26)
	{
		BB_CheckWin();
	}

}

static void BB_RESET(){
	global_u8Ball.x=14;
	global_u8Ball.y=6;
	global_u8Paddle.x=3;
	BB_Lives=3;
	BB_Area[15]=0xff;
	BB_Area[16]=0xff;
	HTFT_voidFillColor ( 0);
	BB_UpdateLive();
	BB_Screen_Update();
	TIM4_voidSetIntervalPeriodic  ( 230000, BB_Start );


}
static void PLAY_BB(){
u8 Key;
Key= HIR_voidReturnKey();
switch (Key){

case 14:
	TIM4_voidStopInterval();
	TIM3_voidStopInterval();
	GameConsoleStart();

	break;
case 12:
	if(global_u8Paddle.x>1){global_u8Paddle.x--;}
	BB_PrintPaddle();

	break;

case 13:
	if(global_u8Paddle.x<6){global_u8Paddle.x++;}
	BB_PrintPaddle();

	break;

case 25: BB_RESET();
		 //Turn_Num=0;
		 break;	// power Button

}

}


void BB_voidStart(){
	BB_Level  = 1;
    BB_HMove = 1;	
    BB_VMove = 1;
	HIR_voidSetCallBack(PLAY_BB);	
	BB_RESET();

}
