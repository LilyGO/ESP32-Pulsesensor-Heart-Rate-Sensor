#ifndef SSD1306_H
#define SSD1306_H



/* I2C address */
#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR         0x3C//0x78
/* Use defines.h for custom definitions */
//#define SSD1306_I2C_ADDR       0x7A
#endif

/* SSD1306 settings */
/* SSD1306 width in pixels */
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH            128
#endif
/* SSD1306 LCD height in pixels */
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT           64
#endif

/**
 * @brief  SSD1306 color enumeration
 */
 
 
 
		    						  
 				   
 	     
#define OLED_CMD  	0		 
#define OLED_DATA 	1		 
 
void OLED_WR_Byte(uint8_t dat,uint8_t cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);		   
							   		    
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot);
void OLED_ShowChar(uint8_t x,uint8_t y,char chr,uint8_t size,uint8_t mode);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num ,uint32_t len,uint8_t size);
void OLED_ShowString(uint8_t x,uint8_t y,const char *p,uint8_t size);	 
 void OLED_ShowBMP(uint8_t x,uint8_t y);
void OLED_Init(void);
 
#endif
