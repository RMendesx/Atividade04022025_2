#include "pico/stdlib.h"
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
volatile bool button_pressed = false;

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
    if (button_pressed) {
        if (current_state == ALL_ON) {
            current_state = TWO_ON;
        } else if (current_state == TWO_ON) {
            current_state = ONE_ON;
        } else {
            current_state = ALL_ON;
        }
        
        update_leds();
        button_pressed = false;  // Resetando para permitir o próximo clique
    }
    return true;  // Retornar true para continuar o temporizador
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
        if (gpio_get(BUTTON_PIN) == 0) {  // Botão pressionado
            button_pressed = true;
        }
    }

    return 0;
}
