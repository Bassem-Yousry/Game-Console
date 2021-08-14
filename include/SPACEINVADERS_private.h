/********************************************************************************/
/* Author : Bassem																*/
/* Date   : 27 Sep 2020															*/
/* V01																			*/
/********************************************************************************/
#ifndef SPACEINVADERS_PRIVATE_H
#define SPACEINVADERS_PRIVATE_H


static void SP_GameOver();
static void SP_UpdateBullet(void);
static void SP_UpdateAlienBullet(void);
static void SP_UpdateAliens(s8 Increment_x , s8 Increment_y);
static void SP_Start(void);
static void SP_BulletHit(void);
static void SP_AliensMove(void);
static void SP_Update(void);
static void PLAY_SpaceInvaders(void);
#endif
