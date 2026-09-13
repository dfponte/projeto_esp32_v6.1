#include "relay.h"
#include "driver/gpio.h" // Se estiver usando o ESP-IDF para acionar o pino

Relay relay_init(int pin) {
    Relay r;
    r.pin = pin;
    r.state = 0;
    
    // Configuração do pino no ESP-IDF
    gpio_reset_pin(pin);
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
    
    return r; // Retorna a struct inteira criada
}

Relay relay_set_on(Relay relay) {
    relay.state = 1;
    gpio_set_level(relay.pin, 1);
    return relay; // Retorna a cópia modificada
}

Relay relay_set_off(Relay relay) {
    relay.state = 0;
    gpio_set_level(relay.pin, 0);
    return relay; // Retorna a cópia modificada
}

int relay_get_status(Relay relay) {
    return relay.state; // Apenas lê o valor recebido por cópia
}
