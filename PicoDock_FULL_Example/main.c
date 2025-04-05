#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

// Display ILI9341 + Touch
#include "tft_lcd_ili9341/ili9341/ili9341.h"
#include "tft_lcd_ili9341/gfx/gfx.h"
#include "tft_lcd_ili9341/touch_resistive/touch_resistive.h"

// Display SSD1306 + Botões
#include "ssd1306/ssd1306.h"
#include "ssd1306/fonts/acme_5_outlines_font.h"
#include "ssd1306/fonts/bubblesstandard_font.h"
#include "ssd1306/fonts/crackers_font.h"
#include "ssd1306/fonts/BMSPA_font.h"

// === Definições para ILI9341 ===
const uint LITE = 15;
#define SCREEN_WIDTH 240

// === Definições para SSD1306 ===
ssd1306_t disp;
const uint8_t *fonts[4] = {acme_font, bubblesstandard_font, crackers_font, BMSPA_font};

const int BTN_PIN_R = 4;
const int BTN_PIN_G = 5;
const int BTN_PIN_B = 6;
const int LED_PIN_R = 7;
const int LED_PIN_G = 8;
const int LED_PIN_B = 9;
const int BUZZER_PIN = 10;

volatile int flag_BTN_R = 0;
volatile int flag_BTN_G = 0;
volatile int flag_BTN_B = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events & 0x4) {
        if (gpio == BTN_PIN_R) flag_BTN_R = 1;
        else if (gpio == BTN_PIN_G) flag_BTN_G = 1;
        else if (gpio == BTN_PIN_B) flag_BTN_B = 1;
    }

    if (events == 0x8) {
        if (gpio == BTN_PIN_R) flag_BTN_R = 0;
        else if (gpio == BTN_PIN_G) flag_BTN_G = 0;
        else if (gpio == BTN_PIN_B) flag_BTN_B = 0;
    }
}

void oled_init(void) {
    i2c_init(i2c1, 400000);
    gpio_set_function(2, GPIO_FUNC_I2C);
    gpio_set_function(3, GPIO_FUNC_I2C);
    gpio_pull_up(2);
    gpio_pull_up(3);

    disp.external_vcc = false;
    ssd1306_init(&disp, 128, 64, 0x3C, i2c1);
    ssd1306_clear(&disp);
    ssd1306_show(&disp);
}

int main(void) {
    stdio_init_all();

    // Inicialização LCD + Touch
    LCD_initDisplay();
    LCD_setRotation(0);
    GFX_createFramebuf();
    configure_touch();

    // Inicialização do pino de backlight e apagamento inicial
    gpio_init(LITE);
    gpio_set_dir(LITE, GPIO_OUT);
    gpio_put(LITE, 0);  // Apaga o backlight inicialmente

    // Inicialização OLED + Botões
    oled_init();

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);
    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

    gpio_init(BTN_PIN_B);
    gpio_set_dir(BTN_PIN_B, GPIO_IN);
    gpio_pull_up(BTN_PIN_B);
    gpio_set_irq_enabled(BTN_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

    gpio_init(LED_PIN_R); gpio_set_dir(LED_PIN_R, GPIO_OUT); gpio_put(LED_PIN_R, 1);
    gpio_init(LED_PIN_G); gpio_set_dir(LED_PIN_G, GPIO_OUT); gpio_put(LED_PIN_G, 1);
    gpio_init(LED_PIN_B); gpio_set_dir(LED_PIN_B, GPIO_OUT); gpio_put(LED_PIN_B, 1);

    gpio_init(BUZZER_PIN); gpio_set_dir(BUZZER_PIN, GPIO_OUT);

    int px, py;

    while (true) {
        // === Parte LCD Touch ===
        GFX_clearScreen();
        GFX_setCursor(0, 10);
        GFX_printf("Touch Demo\n");

        if (readPoint(&px, &py)) {
            gpio_put(LITE, 1); // Liga o backlight quando toca
            px = SCREEN_WIDTH - px;
            GFX_printf("X:%03d Y:%03d\n", px, py);
        } else {
            gpio_put(LITE, 0); // Apaga o backlight quando não toca
            GFX_printf("Sem toque\n");
        }

        GFX_flush();

        // === Parte OLED Botões ===
        if (flag_BTN_R == 1) {
            gpio_put(LED_PIN_R, 0);
            ssd1306_draw_string(&disp, 8, 24, 2, "RED");
            ssd1306_show(&disp);
            while (flag_BTN_R == 1) {
                gpio_put(BUZZER_PIN, 1); sleep_us(100);
                gpio_put(BUZZER_PIN, 0); sleep_us(100);
            }
            ssd1306_clear(&disp); ssd1306_show(&disp);
            gpio_put(LED_PIN_R, 1);
        }

        if (flag_BTN_G == 1) {
            gpio_put(LED_PIN_G, 0);
            ssd1306_draw_string(&disp, 8, 24, 2, "GREEN");
            ssd1306_show(&disp);
            while (flag_BTN_G == 1) {
                gpio_put(BUZZER_PIN, 1); sleep_us(750);
                gpio_put(BUZZER_PIN, 0); sleep_us(750);
            }
            ssd1306_clear(&disp); ssd1306_show(&disp);
            gpio_put(LED_PIN_G, 1);
        }

        if (flag_BTN_B == 1) {
            gpio_put(LED_PIN_B, 0);
            ssd1306_draw_string(&disp, 8, 24, 2, "BLUE");
            ssd1306_show(&disp);
            while (flag_BTN_B == 1) {
                gpio_put(BUZZER_PIN, 1); sleep_us(3000);
                gpio_put(BUZZER_PIN, 0); sleep_us(3000);
            }
            ssd1306_clear(&disp); ssd1306_show(&disp);
            gpio_put(LED_PIN_B, 1);
        }

        sleep_ms(1); // Controle geral de atualização
    }
}
