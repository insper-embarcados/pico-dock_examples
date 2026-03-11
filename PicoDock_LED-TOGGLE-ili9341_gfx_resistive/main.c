#include <stdio.h>
#include "pico/stdlib.h"

#include "tft_lcd_ili9341/gfx/gfx_ili9341.h"
#include "tft_lcd_ili9341/ili9341/ili9341.h"
#include "tft_lcd_ili9341/touch_resistive/touch_resistive.h"

#include "image_bitmap.h"           //Header com os bitmaps do LED ON e OFF

#define SCREEN_ROTATION 1           // 0 = RETRATO, 1 = PAISAGEM

// Estado do LED (0 = OFF, 1 = ON)
int ledState = 0;

// Posição da imagem na tela
int ledImgPosX, ledImgPosY;

// Desenha o LED conforme estado
void drawLed()
{
    gfx_fillRect(ledImgPosX, ledImgPosY, 47, 82, 0x0000);                       //Limpa a área do LED 

    if (ledState)
        gfx_drawBitmap(ledImgPosX, ledImgPosY, image_LED_ON, 47, 82, 0xF800);   // Desenha o LED aceso (vermelho)
    else
        gfx_drawBitmap(ledImgPosX, ledImgPosY, image_LED_OFF, 47, 82, 0xFFFF);  // Desenha o LED apagado (branco)
}

// callback do botão
void ledButtonCallback(GFX_Button *btn)
{
    ledState = !ledState;   // Alterna o estado do LED
    drawLed();              // Redesenha o LED com o novo estado
}

int main()
{
    stdio_init_all();

    //### LCD
    LCD_initDisplay();
    LCD_setRotation(SCREEN_ROTATION);   // Ajusta a rotação da tela conforme definido

    extern uint16_t _width;             // Variável global definida em gfx_ili9341.c que armazena a largura da tela
    extern uint16_t _height;            // Variável global definida em gfx_ili9341.c que armazena a altura da tela
    //=========
    
    //### TOUCH
    configure_touch();                  // Configura o touch resistivo
    //========

    //### GFX
    gfx_init();                         // Inicializa o GFX
    gfx_clear();                        // Limpa a tela
    //=========


    gfx_setTextSize(2);                                 // Define o tamanho do texto
    gfx_setTextColor(0x07E0);                           // Define a cor do texto (verde)

    gfx_drawText(
        _width/6,                                       // Posição horizontal do texto
        10,                                             // Posição vertical do texto
        "PicoDock LED Toggle"                           // Texto a ser exibido
    );

    // Define a posição da imagem do LED para centralizá-la na tela
    ledImgPosX = (_width - 47) / 2;
    ledImgPosY = (_height - 82) / 2;

    drawLed(); // Desenha o LED conforme o estado inicial (apagado)

    // Criação do botão para o LED, invisivel, mas que cobre a área da imagem do LED para detectar os toques
    GFX_Button ledButton = {            
        .x = ledImgPosX,                // Posição horizontal do botão (mesma da imagem do LED)
        .y = ledImgPosY,                // Posição vertical do botão (mesma da imagem do LED)
        .w = 47,                        // Largura do botão (mesma da largura da imagem do LED)
        .h = 82,                        // Altura do botão (mesma da altura da imagem do LED)
        .callback = ledButtonCallback   // Função callback que será chamada quando o botão for pressionado
    };

    gfx_registerButton(&ledButton);     // Registra o botão para que seja detectado toques pelo GFX

    while (true)
    {
        int touchRawX, touchRawY;               // Variaveis para armazenar as coordenadas brutas do toque
        int screenTouchX, screenTouchY  = 0;    // Variaveis para armazenar as coordenadas do toque transformadas para a tela

        int touchDetected = readPoint(&touchRawX, &touchRawY);  // Lê as coordenadas do toque e armazena em touchRawX e touchRawY,
                                                                // a função retorna 1 se um toque for detectado ou 0 caso contrário

        if (touchDetected)                                      
        {                                                       
            gfx_touchTransform(SCREEN_ROTATION,                 // Se um toque for detectado, transforma as coordenadas brutas do toque 
                               touchRawX, touchRawY,            // para as coordenadas da tela considerando a rotação
                               &screenTouchX, &screenTouchY);
        }

                                                                        // Atualiza o estado dos botões registrados no GFX, 
        gfx_updateButtons(screenTouchX, screenTouchY, touchDetected);   // verificando se o toque ocorreu dentro da área de 
                                                                        // algum botão e chamando a função callback correspondente

    }

    return 0;
}
