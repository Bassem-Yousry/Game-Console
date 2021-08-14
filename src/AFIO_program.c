/**********************************************************/
/* Author    : Bassem Yousry                              */
/* Date      : 22 August 2020                             */
/* Version   : V01                                        */
/**********************************************************/
#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "AFIO_interface.h"
#include "AFIO_config.h"
#include "AFIO_private.h"



void	MAFIO_voidSetEXTIConfiguration(u8 Copy_u8Line ,u8 Copy_u8PortMap)
{
	u8 Local_u8RegIndex = Copy_u8Line/4 ;	
	Copy_u8Line %= 4;	
	/* Assign to EXTICRX register									*/

	AFIO->EXTICR[Local_u8RegIndex] &= ~((0b1111) << (Copy_u8Line * 4));
	AFIO->EXTICR[Local_u8RegIndex]  |= ((Copy_u8PortMap) << (Copy_u8Line * 4));	
}
