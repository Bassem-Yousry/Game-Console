/***********************************************************/
/* Author    : Ahmed Assaf                                 */
/* Date      : 21 Sep 2020                                 */
/* Version   : V01                                         */
/***********************************************************/
#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "DIO_interface.h"
#include "SPI_interface.h"
#include "STK_interface.h"

#include "TFT_interface.h"
#include "TFT_private.h"
#include "TFT_config.h"

void HTFT_voidInitialize   (void)
{
	/* Reset Pulse */
	DIO_voidSetPinValue(TFT_RST_PIN,HIGH);
	MSTK_voidSetBusyWait(100);
	DIO_voidSetPinValue(TFT_RST_PIN,LOW);
	MSTK_voidSetBusyWait(1);
	DIO_voidSetPinValue(TFT_RST_PIN,HIGH);
	MSTK_voidSetBusyWait(100);
	DIO_voidSetPinValue(TFT_RST_PIN,LOW);
	MSTK_voidSetBusyWait(100);
	DIO_voidSetPinValue(TFT_RST_PIN,HIGH);
	MSTK_voidSetBusyWait(120000);
	
	/* Sleep Out Command */
	voidWriteCommand(0x11);
	
	/* Wait 150 ms */
	MSTK_voidSetBusyWait(150000);
	
	/* Color Mode Command */
	voidWriteCommand(0x3A);
	voidWriteData   (0x05);
	
	/* Display On Command */
	voidWriteCommand(0x29);

}


void HTFT_voidDisplayImage (const u16* Copy_Image)
{
	u16 counter;
	u8 Data;

	/* Set X Address */
	voidWriteCommand(0x2A);
	voidWriteData(0);
	voidWriteData(0);
	voidWriteData(0);
	voidWriteData(127);
	
	/* Set Y Address */
	voidWriteCommand(0x2B);
	voidWriteData(0);
	voidWriteData(0);
	voidWriteData(0);
	voidWriteData(159);

	/* RAM Write */
	voidWriteCommand(0x2C);

	for(counter = 0; counter< 20480;counter++)
	{
		Data = Copy_Image[counter] >> 8;

		/* Write the high byte */
		voidWriteData(Data);
		/* Write the low byte */
		Data = Copy_Image[counter] & 0x00ff;
		voidWriteData(Data);
	}


}

void HTFT_voidFillColor ( u16 Copy_u16Color)
{
	u16 counter;
	u8 Data;

	/* Set X Address */
	voidWriteCommand(0x2A);
	voidWriteData(0);
	voidWriteData(0);
	voidWriteData(0);
	voidWriteData(127);

	/* Set Y Address */
	voidWriteCommand(0x2B);
	voidWriteData(0);
	voidWriteData(0);
	voidWriteData(0);
	voidWriteData(159);

	/* RAM Write */
	voidWriteCommand(0x2C);

	for(counter = 0; counter< 20480;counter++)
	{
		Data = Copy_u16Color >> 8;

		/* Write the high byte */
		voidWriteData(Data);
		/* Write the low byte */
		Data = Copy_u16Color & 0x00ff;
		voidWriteData(Data);
	}


}
void HTFT_voidDrawFrameRect ( u8 x1, u8 x2, u8 y1, u8 y2, u16 Copy_u16Color,u16 Copy_u16FColor)
{
	u8 Height = y2-y1+1;

	HTFT_voidDrawRect(x1,x2,y1,y1,Copy_u16FColor);
	Height--;
	y1++;
	while(Height>1){
		HTFT_voidDrawRect(x1,x1,y1,y1,Copy_u16FColor);
		HTFT_voidDrawRect(x1+1,x2-1,y1,y1,Copy_u16Color);
		HTFT_voidDrawRect(x2,x2,y1,y1,Copy_u16FColor);
		Height--;
		y1++;
		}
	HTFT_voidDrawRect(x1,x2,y1,y1,Copy_u16FColor);
}

void HTFT_voidDrawRect ( u8 x1, u8 x2, u8 y1, u8 y2, u16 Copy_u16Color)
{
	if(x1>x2){x1=0;}
	if(y1>y2){y1=0;}
	u16 counter , size;
	u8 Data;
	size = (x2-x1+1)*(y2-y1+1);
	/* Set X Address */
	voidWriteCommand(0x2A);
	voidWriteData(0);
	voidWriteData(x1);
	voidWriteData(0);
	voidWriteData(x2);

	/* Set Y Address */
	voidWriteCommand(0x2B);
	voidWriteData(0);
	voidWriteData(y1);
	voidWriteData(0);
	voidWriteData(y2);

	/* RAM Write */
	voidWriteCommand(0x2C);

	for(counter = 0; counter< size;counter++)
	{
		Data = Copy_u16Color >> 8;

		/* Write the high byte */
		voidWriteData(Data);
		/* Write the low byte */
		Data = Copy_u16Color & 0x00ff;
		voidWriteData(Data);
	}


}

void HTFT_voidDraw_u16Frame ( u16 * Copy_u16Frame,u8 X, u8 Y,u8 Copy_u8Width,u8 Copy_u8Height,u8 Size, u16 Copy_u16Color)
{
	for(int i =0 ; i<(Copy_u8Height*Size);i++){
		for(int j =0;j<Copy_u8Width*Size;j++){
			if (GET_BIT(Copy_u16Frame[i/Size],j/Size) )
			{
			 HTFT_voidDrawRect (  X+j,  X+j,  Y+(i),  Y+(i), Copy_u16Color);
			}
		}
	}
}


void HTFT_voidDraw_u8Frame ( u8 * Copy_u16Frame,u8 X, u8 Y,u8 Copy_u8Width,u8 Copy_u8Height,u8 Size, u16 Copy_u16Color)
{
	for(int i =0 ; i<(Copy_u8Height*Size);i++){
		for(int j =0;j<Copy_u8Width*Size;j++){
			if (GET_BIT(Copy_u16Frame[i/Size],j/Size) )
			{
			 HTFT_voidDrawRect (  X+j,  X+j,  Y+(i),  Y+(i), Copy_u16Color);
			}
		}
	}
}



void HTFT_voidDrawCircle ( u8 x1, u8 y1, u16 Copy_u16Color)
{
u8 Copy_u8Char[8] = {0, 28, 62, 127, 127, 127, 62, 28};
for(int i =0 ; i<(8);i++){
	for(int j =0;j<8;j++){
		if (GET_BIT(Copy_u8Char[i],j) ){
		 HTFT_voidDrawRect (  x1+j,  x1+j+1,  y1+(i),  y1+(i)+1, Copy_u16Color);
		}
	}
}

}

void HTFT_voidDrawEgg ( u8 x1, u8 y1, u16 Copy_u16Color,u8 GoldBomb){
	u8 Egg[10]={60, 126, 255, 255, 255, 255, 255, 126, 60, 24};
	if(GoldBomb){Egg[5]=126;Egg[6]=60;Egg[7]=16;Egg[8]=16;Egg[9]=8;}
	for(int i =0 ; i<(10);i++){
		for(int j =0;j<8;j++){
			if (GET_BIT(Egg[i],j) ){
			 HTFT_voidDrawRect (  x1+j,  x1+j,  y1+(i),  y1+(i), Copy_u16Color);
			 if(GoldBomb){Copy_u16Color ^= 0x400;}
			}
		}
	}

}

void HTFT_voidDrawPac ( u8 x1, u8 y1, u16 Copy_u16Color,u8 Rotation,u8 Mouth)
{
	//0 -> Mouth Closed     , else Opened
u8 Pac [8] = {28, 62, 112, 96, 112, 62, 28,0};
if (!Mouth){				//close the Mouth
	for(int m=2;m<=4;m++){
		Pac[m]=127;
	}
}
int y,x ;
for( int v =0 ; v<(8);v++){
	for(int h =0;h<8;h++){
		if(Rotation==0){x = 7-v; y=h;}
		else if(Rotation==1){x=7-h; y=v;}
		else if(Rotation==2){x = h; y=v;}
		else{x = v; y=h;}

		if (GET_BIT(Pac[y],x) ){
		 HTFT_voidDrawRect (  x1+h,  x1+h,  y1+(v),  y1+(v), Copy_u16Color);
		}
		else{
			 HTFT_voidDrawRect (  x1+h,  x1+h,  y1+(v),  y1+(v), 0);

		}
	}
}
}

void HTFT_voidDrawGhost( u8 x1, u8 y1, u16 Copy_u16Color)
{
u8 Ghost [7] = {73, 107, 127, 127, 107, 62, 28, 0};
for(int i =0 ; i<(7);i++){
	for(int j =0;j<7;j++){
		if (GET_BIT(Ghost[i],j) ){
		 HTFT_voidDrawRect (  x1+j,  x1+j,  y1+(i),  y1+(i), Copy_u16Color);
		}
	}
}
}


void HTFT_voidDrawIcyTower( u8 x1, u8 y1, u16 Copy_u16Color)
{
u16 Ghost [16] = {15996, 7800, 1632, 15996, 10212, 16380, 3120, 15996, 32766, 55707, 59799, 61431, 62415, 31806, 16380, 8184};
for(int i =0 ; i<(16);i++){
	for(int j =0;j<16;j++){
		if (GET_BIT(Ghost[i],j) ){
		 HTFT_voidDrawRect (  x1+j,  x1+j,  y1+(i),  y1+(i), Copy_u16Color);
		}
	}
}
}


void HTFT_voidDrawHeart ( u8 x1, u8 y1, u16 Copy_u16Color)
{
u16 Copy_u8Char[11] = {192, 480, 2040, 4092, 4092, 8190, 16383, 16383, 16383, 7998, 3612};
for(int i =0 ; i<(11);i++){
	for(int j =0;j<16;j++){
		if (GET_BIT(Copy_u8Char[i],j) ){
		 HTFT_voidDrawRect (  x1+j,  x1+j,  y1+(i),  y1+(i), Copy_u16Color);
		}
	}
}
}
void HTFT_voidDrawCactus ( u8 x1, u8 y1,u8 Size, u16 Copy_u16Color)
{
u16 Copy_u8Char[13] = { 3840, 8064, 4032, 448, 448, 32767, 65535, 32767, 1792, 1792, 16128, 32256, 15360};
for(int i =0 ; i<(13);i++){
	for(int j =0;j<16*Size;j++){
		if (GET_BIT(Copy_u8Char[i],j/Size) ){
		 HTFT_voidDrawRect (  x1+j,  x1+j,  y1+(i),  y1+(i), Copy_u16Color);
		}
	}
}
}


void HTFT_voidDrawCloud ( u8 x1, u8 y1, u16 Copy_u16Color)
{
u16 Copy_u8Char[16] = { 6, 10, 10, 18, 18, 33, 65, 130, 258, 258, 258, 130, 66, 50, 10, 6};
for(int i =0 ; i<(16*2);i++){
	for(int j =0;j<16*2;j++){
		if (GET_BIT(Copy_u8Char[i/2],j/2) ){
		 HTFT_voidDrawRect (  x1+j,  x1+j,  y1+(i),  y1+(i), Copy_u16Color);
		}
	}
}
}


void HTFT_voidDrawBird ( u8 x1, u8 y1,u8 Wing, u16 Copy_u16Color)
{
u16 Copy_u8Char[16] = { 512, 1536, 3584, 5632, 15872, 3072, 1792,     1920,2046, 2044, 2040, 992,         448, 320, 320, 256};
if(Wing){
	u16 WingUp[5]={65408, 32704, 16320, 1984, 960};
	for(int i =0 ; i<5;i++)
	{
		Copy_u8Char[i+7]=WingUp[i];
	}
}
for(int i =0 ; i<(16);i++){
	for(int j =0;j<16;j++){
		if (GET_BIT(Copy_u8Char[i],j) ){
		 HTFT_voidDrawRect (  x1+j,  x1+j,  y1+(i),  y1+(i), Copy_u16Color);
		}
	}
}
}

void HTFT_voidDrawTRex ( u8 x1, u8 y1,u8 LegMove ,u8 Size,u16 Copy_u16Color)
{
u16 Copy_u8Char[16] = {1984, 480, 240, 120, 255, 505, 496, 1016, 32764, 65524, 49136, 65504, 62592, 62656, 62464, 28672};
if(LegMove){
Copy_u8Char[4]=252;
Copy_u8Char[5]=500;
Copy_u8Char[8]=32767;
Copy_u8Char[9]=65529;
}
for(int i =0 ; i<(16*Size);i++){
	for(int j =0;j<16*Size;j++){
		if (GET_BIT(Copy_u8Char[i/Size],j/Size) ){
		 HTFT_voidDrawRect (  x1+j,  x1+j,  y1+(i),  y1+(i), Copy_u16Color);
		}
	}
}
}


void HTFT_voidDrawAlien ( u8 x1, u8 y1, u16 Copy_u16Color,u8 MoveArms)
{
u16 Copy_u8Char[10] = {288, 144, 504, 2973, 3069, 2925, 1902, 504, 144, 264};
if (MoveArms)
{
	u16 Array[10]={72, 144, 504, 924, 1020, 876, 1902, 2553, 2193, 2313};
for(int i =0 ; i<10; i++){
	Copy_u8Char[i]=Array[i];
}


}
for(int i =0 ; i<(10);i++){
	for(int j =0;j<12;j++){
		if (GET_BIT(Copy_u8Char[i],j) ){
		 HTFT_voidDrawRect (  x1+j,  x1+j,  y1+(i),  y1+(i), Copy_u16Color);
		}
	}
}
}

void HTFT_voidDrawShip ( u8 x1, u8 y1, u16 Copy_u16Color)
{
u16 Copy_u8Char[18] = {0, 0, 975, 510, 252, 120, 120, 120, 120, 252, 510, 510, 462, 258, 390, 204, 120, 48};
for(int i =0 ; i<(18);i++){
	for(int j =0;j<10;j++){
		if (GET_BIT(Copy_u8Char[i],j) ){
		 HTFT_voidDrawRect (  x1+j,  x1+j,  y1+(i),  y1+(i), Copy_u16Color);
		}
	}
}
}


void HTFT_voidDrawChar (u8 Copy_u8Char, u8 x1, u8 y1, u8 Copy_u8Size,u16 Copy_u16Color)
{
	if(Copy_u8Char==32){return;}
	 Copy_u8Char -=48;

for(int i =0 ; i<(8* Copy_u8Size);i++){
	for(int j =0;j<(5*Copy_u8Size);j++){
		if (Char_Array[Copy_u8Char][i/Copy_u8Size] >>(j/Copy_u8Size) &1){
		 HTFT_voidDrawRect (  x1+(j),  x1+(j)+1,  y1+(i),  y1+(i), Copy_u16Color);
		}
	}
}

}


void HTFT_voidDrawText (s8* Copy_u8Text, u8 x1, u8 y1, u8 Copy_u8Size,u16 Copy_u16Color){
	for(; *Copy_u8Text; Copy_u8Text++ ){
		if(*Copy_u8Text==32){x1-=3;}
		else{x1-=6*Copy_u8Size;
		HTFT_voidDrawChar ( *Copy_u8Text,   x1,  y1,  Copy_u8Size, Copy_u16Color);
		}
	}

}

void voidWriteCommand(u8 Copy_u8Command)
{
	u8 Local_u8Temp;
	
	/* Set A0 Pin to Low */
	DIO_voidSetPinValue(TFT_A0_PIN,LOW);
	
	/* Send Command over SPI */
	MSPI1_voidSendReceiveSynch(Copy_u8Command,&Local_u8Temp);
}

static void voidWriteData   (u8 Copy_u8Data)
{
	u8 Local_u8Temp;
	
	/* Set A0 Pin to High */
	DIO_voidSetPinValue(TFT_A0_PIN,HIGH);
	
	/* Send data over SPI */
	MSPI1_voidSendReceiveSynch(Copy_u8Data,&Local_u8Temp);
}





