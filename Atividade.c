#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/timer.h"

// Definir pinos dos LEDs e do botão
#define LED_BLUE_PIN 11
#define LED_RED_PIN 12
#define LED_GREEN_PIN 13
#define BUTTON_PIN 5

// Definir estados para os LEDs
enum led_state {
    ALL_ON,
    TWO_ON,
    ONE_ON
};

volatile enum led_state current_state = ALL_ON;

// Função para alterar os LEDs com base no estado
void update_leds() {
    switch (current_state) {
        case ALL_ON:
            gpio_put(LED_BLUE_PIN, 1);
            gpio_put(LED_RED_PIN, 1);
            gpio_put(LED_GREEN_PIN, 1);
            break;
        case TWO_ON:
            gpio_put(LED_BLUE_PIN, 0);
            gpio_put(LED_RED_PIN, 1);
            gpio_put(LED_GREEN_PIN, 1);
            break;
        case ONE_ON:
            gpio_put(LED_BLUE_PIN, 0);
            gpio_put(LED_RED_PIN, 0);
            gpio_put(LED_GREEN_PIN, 1);
            break;
    }
}

// Função de callback do temporizador para mudar o estado
bool timer_callback(repeating_timer_t *rt) {
    if (current_state == ALL_ON) {
        current_state = TWO_ON;
    } else if (current_state == TWO_ON) {
        current_state = ONE_ON;
    } else {
        current_state = ALL_ON;
    }
    
    update_leds();
    
    return true;  // Retornar true para continuar o temporizador
}

// Função de debounce de botão
bool debounce_button() {
    static uint32_t last_time = 0;
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    
    if (current_time - last_time > 200) {  // 200ms debounce
        last_time = current_time;
        return true;
    }
    return false;
}

// Função principal
int main() {
    stdio_init_all();

    // Inicializar os pinos dos LEDs
    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);

    // Inicializar o pino do botão
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);  // Ativar o resistor pull-up interno

    // Configurar o temporizador para chamar a função de callback a cada 3 segundos
    repeating_timer_t timer;
    add_repeating_timer_ms(3000, timer_callback, NULL, &timer);

    // Loop principal
    while (true) {
        if (gpio_get(BUTTON_PIN) == 0 && debounce_button()) {  // Botão pressionado
            if (current_state == ONE_ON) {  // Só permite o clique quando o último LED estiver desligado
                current_state = ALL_ON;  // Iniciar a sequência
                update_leds();
            }
        }
    }

    return 0;
}