#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define LED_BLUE 11
#define LED_RED 12
#define LED_GREEN 13
#define BUTTON 5

bool is_active = false;  // Flag para impedir múltiplos disparos

// Callback para desligar o LED vermelho e manter o verde
int64_t turn_off_red(alarm_id_t id, void *user_data) {
    gpio_put(LED_RED, 0);
    return 0;
}

// Callback para desligar o LED azul e manter o vermelho
int64_t turn_off_blue(alarm_id_t id, void *user_data) {
    gpio_put(LED_BLUE, 0);
    add_alarm_in_ms(3000, turn_off_red, NULL, false);
    return 0;
}

// Callback para desligar o LED verde e permitir novo acionamento
int64_t turn_off_green(alarm_id_t id, void *user_data) {
    gpio_put(LED_GREEN, 0);
    is_active = false; // Permite novo acionamento
    return 0;
}

void button_callback(uint gpio, uint32_t events) {
    if (!is_active) {
        is_active = true; // Bloqueia novos acionamentos até o ciclo terminar
        gpio_put(LED_BLUE, 1);
        gpio_put(LED_RED, 1);
        gpio_put(LED_GREEN, 1);
        add_alarm_in_ms(3000, turn_off_blue, NULL, false);
        add_alarm_in_ms(9000, turn_off_green, NULL, false);
    }
}

int main() {
    stdio_init_all();
    gpio_init(LED_BLUE);
    gpio_init(LED_RED);
    gpio_init(LED_GREEN);
    gpio_init(BUTTON);
    
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_set_dir(BUTTON, GPIO_IN);
    gpio_pull_up(BUTTON);
    
    gpio_set_irq_enabled_with_callback(BUTTON, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    
    while (1) {
        tight_loop_contents();
    }
}
