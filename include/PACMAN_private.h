/********************************************************************************/
/* Author : Bassem																*/
/* Date   : 27 Sep 2020															*/
/* V01																			*/
/********************************************************************************/
#ifndef PACMAN_PRIVATE_H
#define PACMAN_PRIVATE_H



static void PM_ClearGhosts(void);
static void PM_PrintGhosts(void);
static u8 PM_ChasePac(u8 Pac_x,u8 Pac_y,u8 Ghost_x,u8 Ghost_y,u8 MoveDir);
static void PM_UpdateGhosts(void);

static void PM_PrintDots(void);
static void PM_PrintObstacles(void);
static void PM_CheckGhostContact(void);
static void PM_Update(void);
static void PM_Start(void);

static void PM_CheckWin(void);

static void PLAY_PM(void);

#endif