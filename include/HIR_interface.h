/********************************************************************************/
/* Author : Bassem																*/
/* Date   : 21 Aug 2020															*/
/* V01																			*/
/********************************************************************************/
#ifndef HIR_INTERFACE_H
#define HIR_INTERFACE_H


#define PortA	0000
#define PortB	0001
#define PortC	0010


void HIR_voidInit(void);
void HIR_voidSetCallBack(void(*ptr)(void));
u8 HIR_voidReturnKey();



#endif
