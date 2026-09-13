#ifndef RELAY_H
#define RELAY_H

typedef struct {
    int pin;        // Pino associado ao relé
    int state;      // Estado do relé
} Relay;

// Retornam a struct modificada por valor
Relay relay_init(int pin);
Relay relay_set_on(Relay relay);
Relay relay_set_off(Relay relay);

// Como essa função só lê o status (não modifica), pode receber por valor puro
int relay_get_status(Relay relay);

#endif
