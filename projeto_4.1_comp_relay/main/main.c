#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "relay.h"

//Define os pinos do relé
#define RELAY1_PIN 21  //GPIO21
#define RELAY2_PIN 2  //GPIO2

void app_main(void) {
    // Inicializa passando e recebendo por valor (sem usar &)
    Relay meu_rele_1 = relay_init(RELAY1_PIN);
    Relay meu_rele_2 = relay_init(RELAY2_PIN);


    while (true)
    {
        /* code */

         // Para ligar, você atualiza a variável com o retorno da função
        meu_rele_1 = relay_set_on(meu_rele_1);
        printf("Status de Relé 1 : %d\n",relay_get_status(meu_rele_1));
        vTaskDelay(1000/portTICK_PERIOD_MS);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        meu_rele_2 = relay_set_on(meu_rele_2);
        printf("Status de Relé 2 : %d\n",relay_get_status(meu_rele_2));
        vTaskDelay(1000/portTICK_PERIOD_MS);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        meu_rele_1 = relay_set_off(meu_rele_1);
        printf("Status de Relé 1 : %d\n",relay_get_status(meu_rele_1));
        vTaskDelay(1000/portTICK_PERIOD_MS);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        meu_rele_2 = relay_set_off(meu_rele_2);
        printf("Status de Relé 2 : %d\n",relay_get_status(meu_rele_2));
        vTaskDelay(1000/portTICK_PERIOD_MS);
        vTaskDelay(1000/portTICK_PERIOD_MS);


      
    }
    
   
}
