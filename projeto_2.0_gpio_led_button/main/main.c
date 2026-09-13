#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "driver/gpio.h"

#define LED_PIN_2 2
#define BUTTON_PIN_21 21

static const char *TAG = "BUTTON TESTE";

void app_main(void)
{
   int btn_state = 1;
   bool i = 0;
 //Configuração da porta 2: saída led
   gpio_reset_pin(LED_PIN_2);
   gpio_set_direction(LED_PIN_2,GPIO_MODE_OUTPUT);
   
 //Configuração da porta 21:entrada
    gpio_reset_pin(BUTTON_PIN_21);
    gpio_set_direction(BUTTON_PIN_21,GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_PIN_21,GPIO_PULLUP_ONLY);

    while(true){
     int novo_status = gpio_get_level(BUTTON_PIN_21);

     btn_state = novo_status;
     printf("BTN STATUS = %d\n",btn_state);
     printf("Novo Staus = %d\n",novo_status);
     vTaskDelay(1000/portTICK_PERIOD_MS);
    if(btn_state==0){
     vTaskDelay(1000/portTICK_PERIOD_MS);
       gpio_set_level(LED_PIN_2,i^=1);
      vTaskDelay(1000/portTICK_PERIOD_MS);
      ESP_LOGI(TAG,"Pressionado");
    }else{
       ESP_LOGI(TAG,"Botão Liberado");
       gpio_set_level(LED_PIN_2,0);
       vTaskDelay(1000/portTICK_PERIOD_MS);
    }
     
    }
    vTaskDelay(1000/portTICK_PERIOD_MS);
}
