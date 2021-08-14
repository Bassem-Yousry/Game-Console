/********************************************************************************/
/* Author : Bassem																*/
/* Date   : 27 Sep 2020															*/
/* V01																			*/
/********************************************************************************/
#include <stdio.h>
#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "DIO_interface.h"
#include "STK_interface.h"
#include "SPI_interface.h"
#include "RCC_interface.h"
#include "EXTI_interface.h"
#include "NVIC_interface.h"
#include "TFT_interface.h"
#include "HIR_interface.h"
#include "GPTIM_interface.h"
#include "GC_interface.h"


#include "XO_interface.h"
#include "XO_private.h"
#include "XO_config.h"


static volatile u8 XO_global_u8Shape_Num=0;
static volatile u8 XO_global_u8Winner=0;
static volatile u8 XO_global_u8Turn_Num=0;
static volatile u8 XO_Area [3][3]={{0}};

static void XO_Screen_Update(){
u8 character_num;
u8 character ;
u16 color;
	for(int y =0;y<3;y++){
		for(int x =0 ; x<3;x++){
			character_num=XO_Area[y][x];
			if( !character_num )    //empty block
			{
				HTFT_voidDrawFrameRect (  x*42,  x*42+42, y*53, y*53+53, 0,0xfff);
			}
			else{
				character = 'O';color=0x47E0;
				if(character_num==1){character='X';color=0xF800;}
				HTFT_voidDrawFrameRect (  x*42,  x*42+42, y*53, y*53+53, 0,0xfff);
				HTFT_voidDrawChar (character, x*42+8,  y*53+8, 5,color);
			}
		}
	}


}

static void XO_EndGame(u8 Line){
	XO_Screen_Update();
	if (Line <=2){ HTFT_voidDrawRect (  6,  122,  Line*53+24, Line*53+27, 0xffff);}		//draw Horizontal line
	else if (Line<=5){																	//draw Vertical line
		Line %=3;
		HTFT_voidDrawRect (  Line*42+19,  Line*42+22,  6, 154, 0xffff);}
	else if (Line==6){																	//draw '\' diagonal line
		for(int i=18;i<120;i++){ HTFT_voidDrawRect (  i,  i+2,  i+12, i+14, 0xffff);}
	}
	else if (Line==7){																	//draw '/' diagonal line
		for(int i=120;i>18;i--){
			HTFT_voidDrawRect (  136-i,  136-i+2,  i+12, i+14, 0xffff);
		}
	}

}

static u8 XO_CheckWinner(){
	u8 element=XO_Area[0][0];
	u8 WIN =1;
	u8 line=0;
for(int y=0;y<3;y++){ //check row by row
	element=XO_Area[y][0];
	WIN=1;
	for(int x =1;x<3;x++){
		if(element!=XO_Area[y][x] || element==0){WIN=0;break;}
	}
	if (WIN){XO_global_u8Winner=element;return line;}
	line++;
}


for(int x=0;x<3;x++){ //check column by column
	element=XO_Area[0][x];
	WIN=1;
	for(int y =1;y<3;y++){
		if(element!=XO_Area[y][x]  || element==0){WIN=0;break;}
	}
	if (WIN){XO_global_u8Winner=element;return line;}
	line++;
}

WIN=1;
element=XO_Area[0][0];
for(int i=1;i<3;i++){    				 /*check '\' line */

	if(element!=XO_Area[i][i]|| element==0){WIN=0;break;}
}
if (WIN){XO_global_u8Winner=element;return line;}
line++;

WIN=1;
element=XO_Area[0][2];
for(int i=1;i<3;i++){ //check '/' line
	if(element!=XO_Area[i][2-i]|| element==0){WIN=0;break;}
}
if (WIN){XO_global_u8Winner=element;return line;}
else{return 10;} // no XO_global_u8Winner
}

static void XO_Start(){
		u8 w =XO_CheckWinner();
		if (w!=10){
			XO_EndGame(w);
			TIM3_voidSetBusyWait(3000000);
			HTFT_voidFillColor ( 0x0060);
			if(XO_global_u8Winner==1){HTFT_voidDrawChar ( 'X', 48, 80, 6,0xF800);}
			else{HTFT_voidDrawChar ( 'O', 52, 80, 6,0x07E0);}
			HTFT_voidDrawText ("WINNER|", 124,40, 3,0xffff);
			return;
		}

		XO_Screen_Update();
		XO_global_u8Turn_Num++;
		if(XO_global_u8Turn_Num==10)
		{
			HTFT_voidFillColor ( 0x6808);
			HTFT_voidDrawText ("DRAW|", 105,55, 3,0xffff);
			return;
		}
}

static void XO_RESET(){
	for(int i =0;i<3;i++)
	{
		for(int j =0 ; j<3;j++){
		XO_Area[i][j]=0;
	}
	}

}

static void PLAY_XO(){
u8 Key;
Key= HIR_voidReturnKey();
switch (Key){

case 14:
	TIM4_voidStopInterval();
	TIM3_voidStopInterval();
	GameConsoleStart();
	break;

case 0:	if (XO_Area[2][2]==0){				// Key  1
		SET_BIT(XO_Area [2][2],XO_global_u8Shape_Num-1);
		XO_global_u8Shape_Num=2-(XO_global_u8Shape_Num+1)%2;
		XO_Start();
		}
	break;

case 1:	if (XO_Area[2][1]==0){				// Key  2
		SET_BIT(XO_Area [2][1],XO_global_u8Shape_Num-1);
		XO_global_u8Shape_Num=2-(XO_global_u8Shape_Num+1)%2;
		XO_Start();
		}
	break;

case 2:	if (XO_Area[2][0]==0){				// Key  3
		SET_BIT(XO_Area [2][0],XO_global_u8Shape_Num-1);
		XO_global_u8Shape_Num=2-(XO_global_u8Shape_Num+1)%2;
		XO_Start();
		}
	break;

case 3:	if (XO_Area[1][2]==0){				// Key  4
		SET_BIT(XO_Area [1][2],XO_global_u8Shape_Num-1);
		XO_global_u8Shape_Num=2-(XO_global_u8Shape_Num+1)%2;
		XO_Start();

		}
	break;

case 4:	if (XO_Area[1][1]==0){				// Key  5
		SET_BIT(XO_Area [1][1],XO_global_u8Shape_Num-1);
		XO_global_u8Shape_Num=2-(XO_global_u8Shape_Num+1)%2;
		XO_Start();

		}
	break;

case 5:	if (XO_Area[1][0]==0){				// Key  6
		SET_BIT(XO_Area [1][0],XO_global_u8Shape_Num-1);
		XO_global_u8Shape_Num=2-(XO_global_u8Shape_Num+1)%2;
		XO_Start();

		}
	break;

case 6:	if (XO_Area[0][2]==0){				// Key  7
		SET_BIT(XO_Area [0][2],XO_global_u8Shape_Num-1);
		XO_global_u8Shape_Num=2-(XO_global_u8Shape_Num+1)%2;
		XO_Start();

		}
	break;

case 7:	if (XO_Area[0][1]==0){				// Key  8
		SET_BIT(XO_Area [0][1],XO_global_u8Shape_Num-1);
		XO_global_u8Shape_Num=2-(XO_global_u8Shape_Num+1)%2;
		XO_Start();

		}
	break;

case 8:	if (XO_Area[0][0]==0){				// Key  9
		SET_BIT(XO_Area [0][0],XO_global_u8Shape_Num-1);
		XO_global_u8Shape_Num=2-(XO_global_u8Shape_Num+1)%2;
		XO_Start();

		}
	break;

}

}

void XO_voidStart(){
	XO_global_u8Turn_Num=0;
	XO_global_u8Shape_Num=0;
	XO_global_u8Winner=0;
	XO_RESET();
	XO_Start();
	XO_global_u8Turn_Num=0;
	HIR_voidSetCallBack(PLAY_XO);
}
