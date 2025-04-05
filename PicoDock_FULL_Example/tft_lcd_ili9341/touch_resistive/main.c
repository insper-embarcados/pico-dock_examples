#include <stdio.h>
#include "pico/stdlib.h"
#include "touch_resistive.h"

int main(void)
{
          stdio_init_all();  // Inicializa a saída padrão na Pic 
    // Inicializa o touch
    printf("ADC INIT\n");
    configure_touch();

    int px, py;
printf("TESTE\n");
    while (1) {
        if (readPoint(&px, &py)) {
           printf("%03d, %03d\n", px, py);
        }
        sleep_ms(100);  // Função equivalente a delay_ms no SDK da Pico
    }
}