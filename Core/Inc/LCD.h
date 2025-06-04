#ifndef LCD_H
#define LCD_H

#include "stm32f4xx_hal.h"

#define I2C_ADDR 0x4E // Default PCF8574T address (0x27 << 1). Change to 0x7E if address is 0x3F.
#define LCD_ROWS 2
#define LCD_COLS 16
#define RS_BIT 0 // Register select bit
#define EN_BIT 2 // Enable bit
#define BL_BIT 3 // Backlight bit
#define D4_BIT 4 // Data bit 4
#define D5_BIT 5 // Data bit 5
#define D6_BIT 6 // Data bit 6
#define D7_BIT 7 // Data bit 7

void lcd_init(I2C_HandleTypeDef *hi2c);
void lcd_send_cmd(uint8_t cmd);
void lcd_send_data(uint8_t data);
void lcd_send_string(char *str);
void lcd_clear(void);
void lcd_put_cur(int row, int col);

#endif
