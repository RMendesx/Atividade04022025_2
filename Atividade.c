#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

#define LED_RED 11
#define LED_YELLOW 12
#define LED_GREEN 13

volatile int state = 0; // Estado do semáforo

// Função de callback para o temporizador
bool repeating_timer_callback(struct repeating_timer *t) {
    switch (state) {
        case 0:
            gpio_put(LED_RED, 1);
            gpio_put(LED_YELLOW, 0);
            gpio_put(LED_GREEN, 0);
            printf("Semáforo: VERMELHO\n");
            break;
        case 1:
            gpio_put(LED_RED, 0);
            gpio_put(LED_YELLOW, 1);
            gpio_put(LED_GREEN, 0);
            printf("Semáforo: AMARELO\n");
            break;
        case 2:
            gpio_put(LED_RED, 0);
            gpio_put(LED_YELLOW, 0);
            gpio_put(LED_GREEN, 1);
            printf("Semáforo: VERDE\n");
            break;
    }
    state = (state + 1) % 3; // Alterna entre os estados
    return true; // Mantém o temporizador repetindo
}

int main() {
    stdio_init_all(); // Inicializa a comunicação serial

    gpio_init(LED_RED);
    gpio_init(LED_YELLOW);
    gpio_init(LED_GREEN);
    
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_set_dir(LED_YELLOW, GPIO_OUT);
    gpio_set_dir(LED_GREEN, GPIO_OUT);

    struct repeating_timer timer;
    add_repeating_timer_ms(3000, repeating_timer_callback, NULL, &timer); // Timer de 3 segundos

    while (1) {
        printf("Sistema rodando...\n");
        sleep_ms(1000); // Mensagem a cada 1 segundo
    }

    return 0;
}