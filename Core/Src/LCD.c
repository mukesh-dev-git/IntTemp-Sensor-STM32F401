#include "LCD.h"
#include <string.h>

I2C_HandleTypeDef *lcd_i2c;

void lcd_write_nibble(uint8_t nibble, uint8_t rs) {
    uint8_t data = (nibble << D4_BIT) | (rs << RS_BIT) | (1 << BL_BIT) | (1 << EN_BIT);
    HAL_I2C_Master_Transmit(lcd_i2c, I2C_ADDR, &data, 1, HAL_MAX_DELAY);
    HAL_Delay(1);
    data &= ~(1 << EN_BIT); // Clear EN bit
    HAL_I2C_Master_Transmit(lcd_i2c, I2C_ADDR, &data, 1, HAL_MAX_DELAY);
}

void lcd_send_cmd(uint8_t cmd) {
    lcd_write_nibble(cmd >> 4, 0);   // High nibble
    lcd_write_nibble(cmd & 0x0F, 0); // Low nibble
    HAL_Delay(2);
}

void lcd_send_data(uint8_t data) {
    lcd_write_nibble(data >> 4, 1);   // High nibble
    lcd_write_nibble(data & 0x0F, 1); // Low nibble
    HAL_Delay(2);
}

void lcd_init(I2C_HandleTypeDef *hi2c) {
    lcd_i2c = hi2c;
    HAL_Delay(50);
    lcd_write_nibble(0x03, 0); HAL_Delay(5);
    lcd_write_nibble(0x03, 0); HAL_Delay(5);
    lcd_write_nibble(0x03, 0); HAL_Delay(5);
    lcd_write_nibble(0x02, 0); // Set 4-bit mode
    lcd_send_cmd(0x28); // Function set: 4-bit, 2 lines, 5x8 font
    lcd_send_cmd(0x0C); // Display ON, cursor OFF
    lcd_send_cmd(0x06); // Entry mode: increment cursor
    lcd_send_cmd(0x01); // Clear display
    HAL_Delay(2);
}

void lcd_send_string(char *str) {
    while (*str) lcd_send_data(*str++);
}

void lcd_clear(void) {
    lcd_send_cmd(0x01);
    HAL_Delay(2);
}

void lcd_put_cur(int row, int col) {
    uint8_t addr = (row == 0) ? 0x80 + col : 0xC0 + col;
    lcd_send_cmd(addr);
}
