
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "relay.h"


//Define os pinos do relé
#define RELAY1_PIN 21  //GPIO21
#define RELAY2_PIN 2  //GPIO2

void app_main(void)
{
    relay_init(false,RELAY1_PIN);
    relay_init(false,RELAY2_PIN);  
     while (true)
    {
        relay_set_on(RELAY1_PIN);
        printf("Status do relay1: ON \n");
        vTaskDelay(1000/portTICK_PERIOD_MS);
        relay_set_on(RELAY2_PIN);
         printf("Status do relay2: ON\n");
        vTaskDelay(1000/portTICK_PERIOD_MS);
        relay_set_off(RELAY1_PIN);
         printf("Status do relay1: OFF\n");
        vTaskDelay(1000/portTICK_PERIOD_MS);
        relay_set_off(RELAY2_PIN);
        printf("Status do relay2: OFF\n");
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }

}
