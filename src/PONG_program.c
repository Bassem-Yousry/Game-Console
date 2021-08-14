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


#include "PONG_interface.h"
#include "PONG_private.h"
#include "PONG_config.h"

#define Null (void *) 0
	
#define Null (void *) 0

static volatile  u8 PP_Score  = 0;
static volatile  u8 PP_Score2 = 0;
static volatile  u8 PP_HMove = 1;			//1 ->increase  x postion
static volatile  s8 PP_VMove = 1;			//1 ->increase  y postion
struct PONG{
	u8 x;
	u8 y;
};

volatile struct PONG PP_global_u8Ball = {7,3};
volatile struct PONG PP_global_u8Paddle = {0,10};
volatile struct PONG PP_global_u8Paddle2 = {15,10};



static void PP_Print_Ball(u16 Color){
	u8 x = PP_global_u8Ball.x , y = PP_global_u8Ball.y;
	HTFT_voidDrawCircle(x*8,y*8,Color);

}

static void PP_ScoreUpdate(){
	u8 ScoreNum;
	s8 SCORE[5];
	HTFT_voidDrawRect (11,  45, 130 ,147,0);
	HTFT_voidDrawRect (75,  110, 130 ,147,0);
	ScoreNum=sprintf(SCORE,"%i",PP_Score);
	HTFT_voidDrawText (SCORE,  35+ScoreNum*5, 130 ,2,0Xfffe);
	ScoreNum=sprintf(SCORE,"%i",PP_Score2);
	HTFT_voidDrawText (SCORE,  100+ScoreNum*5, 130 ,2,0Xfffe);
}

static void PP_Screen_Update(){
	if(PP_global_u8Ball.x==5||PP_global_u8Ball.x==9){
		for(int i=0;i<20;i++)
		{
			HTFT_voidDrawRect(63,63,i*8,i*8+8,0xffff*((1+i)%2));
			HTFT_voidDrawRect(63,63,i*8,i*8+8,0xffff*((1+i)%2));
		}
	}
}



static void PP_Update_Ball(){
	PP_Print_Ball(0);
	u8  Ball_y=PP_global_u8Ball.y ,
		Ball_x=PP_global_u8Ball.x,
		Paddle_y =PP_global_u8Paddle.y,
		Paddle2_y =PP_global_u8Paddle2.y;
	if (PP_HMove){										//moving leftward
		if( Ball_x  == 15 )
		{
			PP_Score++;
			PP_HMove=0;
			PP_global_u8Ball.x--;
			HTFT_voidDrawRect (  64,  120, 0, 159, 0);
			PP_ScoreUpdate();
		}
		else if ( Ball_x+1==15  )
			{
			if(Ball_y>=Paddle2_y-2 &&Ball_y<Paddle2_y ){PP_HMove=0;PP_global_u8Ball.x--;	PP_VMove=-1;}
			else if(Ball_y==Paddle2_y){PP_HMove=0;PP_global_u8Ball.x--;PP_VMove=0;}
			else if(Ball_y>Paddle2_y &&Ball_y<=Paddle2_y+2){PP_HMove=0;PP_global_u8Ball.x--;PP_VMove=1;}
			else{PP_global_u8Ball.x++;}

			}
		else
		{
			PP_global_u8Ball.x++;
		}
	}
	else{												//moving rightward
		if( Ball_x  == 0 ) {
				PP_Score2++;
				HTFT_voidDrawRect (  8,  62, 0, 159, 0);
				PP_ScoreUpdate();
				PP_HMove=1;
				PP_global_u8Ball.x++;
		}
		else if ( Ball_x-1==0)
		{
			if(Ball_y>=Paddle_y-2 &&Ball_y<Paddle_y ){PP_HMove=1;PP_global_u8Ball.x++;	PP_VMove=-1;}
			else if(Ball_y==Paddle_y){PP_HMove=1;PP_global_u8Ball.x++;PP_VMove=0;}
			else if(Ball_y>Paddle_y &&Ball_y<=Paddle_y+2){PP_HMove=1;PP_global_u8Ball.x++;PP_VMove=1;}
			else{PP_global_u8Ball.x--;}

		}else
		{
			PP_global_u8Ball.x--;
		}

	}
	if (PP_VMove==1){										//moving upward
			if( Ball_y  == 19  )
			{
				PP_VMove=-1;
				PP_global_u8Ball.y--;

			}
			else
			{
				PP_global_u8Ball.y++;
			}

		}
		else if (PP_VMove==-1){												//moving downward
			if( Ball_y  == 0 ) {
						PP_VMove=1;
						PP_global_u8Ball.y++;
			}
			else
			{
				PP_global_u8Ball.y--;
			}

		}
	PP_Print_Ball(0xfff);
}

static void PP_Print_Paddle(){
	u8  Paddle_y =PP_global_u8Paddle.y-2;
	for (int i =0;i<20;i++){
		if(i==Paddle_y ){
			for(int j =0 ; j<=4;j++)
			{
				HTFT_voidDrawRect (  6,  7, (i+j)*8, (i+j)*8+7, 0xffff);
			}
			i+=4;
			continue;
		}
		HTFT_voidDrawRect (  6,  7, i*8, i*8+8, 0);
	}

}
static void PP_Print_Paddle2(){
	u8  Paddle_y2 =PP_global_u8Paddle2.y-2;
	for (int i =0;i<20;i++){
		if(i==Paddle_y2 ){
			for(int j =0 ; j<=4;j++)
			{
				HTFT_voidDrawRect (  120,  121, (i+j)*8, (i+j)*8+7, 0xffff);
			}
			i+=4;
			continue;
		}
		HTFT_voidDrawRect (  120,  121, i*8, i*8+8, 0);
	}

}

static void PP_Start(){
	PP_Screen_Update();
	if(PP_global_u8Ball.x>=3 &&PP_global_u8Ball.y >=16){PP_ScoreUpdate();}
	PP_Update_Ball();
}

static void PLAY_PONG(){

u8 Key;
Key= HIR_voidReturnKey();
switch (Key){
case 14:
	TIM4_voidStopInterval();
	TIM3_voidStopInterval();
	GameConsoleStart();
	break;
case 10:
	if(PP_global_u8Paddle.y<17){PP_global_u8Paddle.y++;}

	break;

case 11:
	if(PP_global_u8Paddle.y>2){PP_global_u8Paddle.y--;}

	break;

case 25: PONG_voidStart();
		 //Turn_Num=0;
		 break;	// power Button

}
PP_Print_Paddle();
}

static void PP_UpdatePaddle2(){
	u8 Paddle2_y = PP_global_u8Paddle2.y , Ball_y=PP_global_u8Ball.y;

	if(Paddle2_y+2 <Ball_y && Paddle2_y<17){
		PP_global_u8Paddle2.y++;
	}
	else if(Paddle2_y-2 >Ball_y && Paddle2_y>2){
		PP_global_u8Paddle2.y--;
	}
	else{
		PP_global_u8Paddle2.y+=PP_VMove;
	}
	PP_Print_Paddle2();
}


void PONG_voidStart(){
	PP_global_u8Ball.x=7;
	PP_global_u8Ball.y=3;
	PP_Score=0;
	PP_Score2=0;
	PP_global_u8Paddle.y=10;
	PP_global_u8Paddle2.y=10;
	HTFT_voidFillColor ( 0);
	HTFT_voidDrawRect (  0,  63, 0, 160, 0);
	for(int i=0;i<20;i++)
	{
		HTFT_voidDrawRect(63,63,i*8,i*8+8,0xffff);
	}
	HTFT_voidDrawRect (  64,  127, 0, 160, 0);
	PP_ScoreUpdate();
	PP_Print_Paddle2();
	PP_Print_Paddle();
	HIR_voidSetCallBack(PLAY_PONG);
	TIM4_voidSetIntervalPeriodic  ( 500000, PP_Start );
	TIM3_voidSetIntervalPeriodic  ( 2000000, PP_UpdatePaddle2 );

}
