/***********************************************************/
/* Author    : Ahmed Assaf                                 */
/* Date      : 21 Sep 2020                                 */
/* Version   : V01                                         */
/***********************************************************/
#ifndef TFT_INTERFACE_H
#define TFT_INTERFACE_H

void HTFT_voidInitialize   (void);
void HTFT_voidDisplayImage (const u16* Copy_Image);
void HTFT_voidFillColor ( u16 Copy_u16Color);
void HTFT_voidDrawRect ( u8 x1, u8 x2, u8 y1, u8 y2, u16 Copy_u16Color);
void HTFT_voidDrawFrameRect ( u8 x1, u8 x2, u8 y1, u8 y2, u16 Copy_u16Color,u16 Copy_u16FColor);
void HTFT_voidDraw_u16Frame ( u16 * Copy_u16Frame,u8 X, u8 Y,u8 Copy_u8Height,u8 Copy_u8Width,u8 Size ,u16 Copy_u16Color);
void HTFT_voidDraw_u8Frame ( u8 * Copy_u16Frame,u8 X, u8 Y,u8 Copy_u8Height,u8 Copy_u8Width,u8 Size ,u16 Copy_u16Color);
void HTFT_voidDrawHeart ( u8 x1, u8 y1, u16 Copy_u16Color);
void HTFT_voidDrawChar (u8 Copy_u8Char, u8 x1, u8 y1, u8 Copy_u8Size,u16 Copy_u16Color);
void HTFT_voidDrawText (s8* Copy_u8Text, u8 x1, u8 y1, u8 Copy_u8Size,u16 Copy_u16Color);
void HTFT_voidDrawCircle ( u8 x1, u8 y1, u16 Copy_u16Color);
void HTFT_voidDrawPac ( u8 x1, u8 y1, u16 Copy_u16Color,u8 Rotation,u8 Mouth);
void HTFT_voidDrawGhost( u8 x1, u8 y1, u16 Copy_u16Color);

#endif
