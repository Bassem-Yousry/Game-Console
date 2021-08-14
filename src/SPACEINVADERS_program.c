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

#include "SPACEINVADERS_interface.h"
#include "SPACEINVADERS_private.h"
#include "SPACEINVADERS_config.h"

#define Null (void *) 0

static volatile u8 SP_globlal_u8AlienNum=23;
static volatile u8 SP_globlal_u8AlienShootTime=0;
static volatile u8 SP_globlal_u8LastAlien=0;
static volatile s8 SP_globlal_s8HMove=1;
static volatile s8 SP_globlal_s8VMove=0;
static volatile u8 SP_globlal_u8AliensKilled=0;
static volatile u8 SP_globlal_u8AlienArms =0;
static volatile u8 SP_globlal_u8Ship_x=30;
static volatile u8 SP_globlal_u8Lose=0;

u16 SP_SpaceFrame[18] = {0, 0, 975, 510, 252, 120, 120, 120, 120, 252, 510, 510, 462, 258, 390, 204, 120, 48};
u16 SP_AlienFrame[2][10] = {
							 {288, 144, 504, 2973, 3069, 2925, 1902, 504, 144, 264},
							 {72, 144, 504, 924, 1020, 876, 1902, 2553, 2193, 2313}
						   };


struct SpaceInvaders{
	u8 x;
	u8 y;
	struct SpaceInvaders * Next;
};
struct SpaceInvaders * AlienList ;
struct SpaceInvaders * Bullets ;
struct SpaceInvaders * AlienBullets  ;

static void SP_AddNode(struct SpaceInvaders ** Head ,u8 Node_x,u8 Node_y){

	struct SpaceInvaders * Node = Null;
	Node = (struct SpaceInvaders*)malloc(sizeof(struct SpaceInvaders)) ;
	Node->x=Node_x;
	Node->y=Node_y;
	Node->Next=*Head;
	*Head=Node;

}

static void SP_DeleteNode(struct SpaceInvaders **  Head ,u8 NodeNum){
	if (*Head==Null){return;}

	u8 Count =0;
	struct SpaceInvaders * LastNode, *PreLastNode=Null;
	LastNode=*Head;
	if (NodeNum==0){
		* Head = LastNode ->Next;
		free(LastNode);//LastNode);
		return;
	}
	while(LastNode->Next!=Null && Count !=NodeNum){
		PreLastNode=LastNode;
		LastNode=LastNode->Next;
		Count++;
	}
	if(Count != NodeNum){return;}
	PreLastNode->Next=LastNode->Next;
	free(LastNode);
}

static void SP_PrintBullets(struct SpaceInvaders * Bullet,u16 Color){
	if(Bullet==Null){return;}
	struct SpaceInvaders * Node;
		Node=Bullet;
		while(Node != Null){
			HTFT_voidDrawRect(2*Node->x,2*Node->x+1,2*Node->y,2*Node->y+4,Color);
			Node = Node->Next;
		}
}

static void SP_ClearList(struct SpaceInvaders * Head){
	if (Head==Null){return;}
	struct SpaceInvaders * Node;
		Node=Head;
		while(Node != Null){
			HTFT_voidDrawRect(Node->x*4,Node->x*4+13,Node->y*4,Node->y*4+14,0);
			Node = Node->Next;
		}
}

static void SP_PrintList(struct SpaceInvaders * Head,u16 Color){
	if(Head==Null){return;}
	struct SpaceInvaders * Node;
		Node=Head;
		while(Node != Null){
			HTFT_voidDraw_u16Frame(SP_AlienFrame[SP_globlal_u8AlienArms], Node->x*4 ,Node->y*4 ,12 ,10, 1, Color);
			Node = Node->Next;
		}
}

static void SP_UpdateBullet(){
	if(Bullets==Null){return;}
	struct SpaceInvaders * Node;
		Node=Bullets;
		while(Node != Null){
			Node->y++;
			Node = Node->Next;
		}
}

static void SP_DeleteList(struct SpaceInvaders ** Head){
	struct SpaceInvaders * node =*Head ;
	struct SpaceInvaders * deleteNode ;
	while(node!=Null){
		deleteNode=node;
		node=node->Next;
		free(deleteNode);
	}
}



static void SP_UpdateAlienBullet(){
	if(AlienBullets==Null){return;}
	struct SpaceInvaders * Node;
	u8 BulletHitNum[10];
	u8 BulletNum =0;
	u8 BulletCount=0;
	Node=AlienBullets;
	while(Node != Null)
	{
		if(Node->y==0)
			{
			SP_PrintBullets(AlienBullets,0);
			BulletHitNum[BulletNum++]=BulletCount;
			}
		else if((Node->x >= SP_globlal_u8Ship_x && Node->x < SP_globlal_u8Ship_x+5  ) &&Node->y<=9)
		{
			SP_globlal_u8Lose=1;
			BulletHitNum[BulletNum++]=BulletCount;
		}
		else
		{
			Node->y--;
		}
		BulletCount++;
		Node = Node->Next;
	}
	for(int i =BulletNum-1;i>=0;i-- )
	{
		SP_DeleteNode(&AlienBullets,BulletHitNum[i]);
	}

}
//increment
static void SP_UpdateAliens(s8 Increment_x , s8 Increment_y){
	struct SpaceInvaders * Node;
			Node=AlienList;
			while(Node != Null){
				Node->x+=Increment_x;
				Node->y+=Increment_y;
				Node = Node->Next;
				if (Node->y<=4 )//&& Alien->x *2 ==Ship_x)
				{
					SP_globlal_u8Lose=1;
					return;
				}
			}

}

static void SP_Start(){
for(int j =0 ; j<4;j++){
	for(int i =0 ; i<6;i++){
		SP_AddNode(&AlienList,i*5,23+j*4);
	}
}
	SP_PrintList(AlienList,0xafff);
	HTFT_voidDraw_u16Frame(SP_SpaceFrame ,2*SP_globlal_u8Ship_x, 0,10,18,1, 0xF800);
	//HTFT_voidDrawShip ( 2*SP_globlal_u8Ship_x, 0, 0xF800);

}

static void SP_GameOver(){
	TIM3_voidStopInterval();
	TIM4_voidStopInterval();
	HTFT_voidFillColor ( 0);
	if(SP_globlal_u8Lose){
	HTFT_voidDrawText("U LOST",110,80,3,0xfff);
	}
	else if (SP_globlal_u8AliensKilled==24){HTFT_voidDrawText("U WON",110,80,3,0xfff);}
	SP_DeleteList(&AlienList);
	SP_DeleteList(&AlienBullets);
	SP_DeleteList(&Bullets);
}
static void SP_BulletHit(){
	u8 BulletHitNum[10];
	u8 BulletNum =0;
	struct SpaceInvaders * Bullet,*Alien;
	int BulletCount=0 , AlienCount=0;
	Bullet=Bullets;
	Alien=AlienList;
	while(Bullet != Null)
	{
		AlienCount=0;
		Alien=AlienList;
		if(Bullet->y>=77)
		{
			SP_PrintBullets(Bullets,0);
			BulletHitNum[BulletNum++]=BulletCount;
		}
		else{
			while(Alien != Null)
			{
				if (Alien->y<=4 )//&& Alien->x *2 ==SP_globlal_u8Ship_x)
				{
					TIM3_voidStopInterval();
					TIM4_voidStopInterval();
					return;
				}
				if( (Bullet->x >= Alien->x*2 && Bullet->x <(Alien->x+3)*2  ) &&Bullet->y==Alien->y *2)
				{
					SP_globlal_u8AlienNum--;
					HTFT_voidDrawRect (  Alien->x*4,  Alien->x*4+16, Alien->y*4, Alien->y*4+11, 0);
					SP_globlal_u8AliensKilled++;
					BulletHitNum[BulletNum++]=BulletCount;
					SP_DeleteNode(&AlienList,AlienCount);

					break;
				}
				AlienCount++;
				Alien = Alien->Next;
			}
		}
		Bullet = Bullet->Next;
		BulletCount++;
	}

	for(int i =BulletNum-1;i>=0;i-- )
	{
		SP_DeleteNode(&Bullets,BulletHitNum[i]);
	}

}

static void SP_AliensMove(){
	//
	SP_globlal_s8VMove=0;
	u8 i =0;
	struct SpaceInvaders * Node=AlienList;
	while(i<6 && Node !=Null)
	{
		if(Node->x ==0)	//IF REACH THE FAR EAST MOVE LEFTWARDS
		{
			SP_globlal_s8HMove=1;	
			break;
		}
		else if(Node->x ==29)//IF REACH THE FAR WEST MOVE RIGHTWARDS & DOWNWARDS
		{
			SP_globlal_s8VMove=-1;
			SP_globlal_s8HMove=-1;
			break;
		}
		Node=Node->Next;
		i++;
	}

	SP_ClearList(AlienList);
	SP_UpdateAliens(SP_globlal_s8HMove,SP_globlal_s8VMove);

	SP_PrintList(AlienList,0xafff);
	SP_globlal_u8AlienArms=(SP_globlal_u8AlienArms+1)%2;
	TIM4_voidSetIntervalSingle  ( 10000000-SP_globlal_u8AliensKilled*420000, SP_AliensMove );
}

static void SP_AlienShoot(u8 SP_globlal_u8AlienNum){
	if(AlienList==Null){return;}
	struct SpaceInvaders* Node=AlienList;
	u8 i=0;
	while(Node->Next!=Null && i!=SP_globlal_u8AlienNum){
		Node=Node->Next;
		i++;
	}
	SP_AddNode(&AlienBullets,Node->x*2+3,Node->y*2-3);

}

static void SP_Update(){

				/*SpaceShip Bullet */
	SP_PrintBullets(Bullets,0);
	SP_UpdateBullet();
	SP_PrintBullets(Bullets,0xD680);

	SP_BulletHit();
		/* Shoot When SP_globlal_u8AlienShootTime=100*/
	SP_globlal_u8AlienShootTime++;
	if(SP_globlal_u8AlienShootTime==100)
	{
		SP_globlal_u8AlienShootTime=0;
		SP_AlienShoot(SP_globlal_u8AlienNum-SP_globlal_u8LastAlien);
		SP_globlal_u8LastAlien++;
		if(SP_globlal_u8LastAlien==6){SP_globlal_u8LastAlien=0;}
	}
		/* Alien's Bullet Speed = SpaceShip speed / 4 */
	if(SP_globlal_u8AlienShootTime%4==0)
	{
		SP_PrintBullets(AlienBullets,0);
		SP_UpdateAlienBullet();
		SP_PrintBullets(AlienBullets,0xafff);
	}
	if(SP_globlal_u8Lose){SP_GameOver();}
	if(SP_globlal_u8AliensKilled==24){SP_GameOver();}

}

static void PLAY_SpaceInvaders(){
	HTFT_voidDraw_u16Frame(SP_SpaceFrame, 2*SP_globlal_u8Ship_x, 0 ,10 ,18, 1, 0);
u8 Key;
Key= HIR_voidReturnKey();
switch (Key){
case 14:
	SP_GameOver();
	TIM4_voidStopInterval();
	TIM3_voidStopInterval();
	GameConsoleStart();

	return;
	break;

case 12:			//move rightwards

if( SP_globlal_u8Ship_x > 1 )
	{
		SP_globlal_u8Ship_x-=2;
	}

	break;

case 13:			//move leftwards

if(SP_globlal_u8Ship_x <58 )
	{

		SP_globlal_u8Ship_x+=2;

	}

	break;

case 25:
		SP_AddNode(&Bullets,SP_globlal_u8Ship_x+2,10);
		 break;	

}
HTFT_voidDraw_u16Frame(SP_SpaceFrame, 2*SP_globlal_u8Ship_x, 0 ,10 ,18, 1, 0xF800);

}

void SPACEINVADERS_voidStart(){
	AlienList = Null;
	Bullets =Null ;
	AlienBullets =Null ;
	SP_globlal_u8AlienNum=23;
	SP_globlal_u8AlienShootTime=0;
	SP_globlal_u8LastAlien=0;
	SP_globlal_s8HMove=1;
	SP_globlal_s8VMove=0;
	SP_globlal_u8AliensKilled=0;
	SP_globlal_u8AlienArms =0;
	SP_globlal_u8Ship_x=30;
	SP_globlal_u8Lose=0;

	HTFT_voidFillColor ( 0);
	HTFT_voidDrawRect (  0,  63, 0, 160, 0);
	HTFT_voidDrawRect (  64,  127, 0, 160, 0);
	SP_Start();
	TIM3_voidSetIntervalPeriodic  ( 100000, SP_Update );
	TIM4_voidSetIntervalSingle ( 10000000, SP_AliensMove );
	HIR_voidSetCallBack(PLAY_SpaceInvaders);
}
