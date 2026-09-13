#include <stdio.h>
#include "relay.h"

#include <driver/gpio.h>

//Funções de inicialização do relé

void relay_init(int state,int pin){
  
   gpio_set_direction(pin,GPIO_MODE_OUTPUT);
   gpio_set_level(pin,state);
   
}

void relay_set_on(int pin){
  int state = true;
  gpio_set_level(pin,state);
}

void relay_set_off(int pin){
   int state = false;
   gpio_set_level(pin,state);
}

