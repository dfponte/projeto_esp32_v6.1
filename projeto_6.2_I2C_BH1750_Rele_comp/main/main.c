#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "relay.h"
#include "bh1750.h"


//Define os pinos do relé
#define RELAY1_PIN 21  //GPIO21
#define RELAY2_PIN 2  //GPIO2



void app_main()
{
  // Inicializa o relé 

   relay_init(false,RELAY1_PIN);
   relay_init(false,RELAY2_PIN);  


  // Inicialize I2C recebendo o retorno da função
        i2c_master_dev_handle_t dev_handle = i2c_init();

     while (1) {

      
        uint8_t data[2];
        // 4. Receber 2 bytes de dados
        esp_err_t ret = i2c_master_receive(dev_handle, data, 2, -1);  // dev_handle (Manipulador do Dispositivo) endereço identificador
                                                                      // O que é: É o "endereço de identificação" do dispositivo 
                                                                      
        
        if (ret == ESP_OK) {
            // Cálculo: (MSB << 8 | LSB) / 1.2
            float lux = (float)((data[0] << 8) | data[1]) / 1.2;
            if(lux<10){
                  relay_set_on(RELAY1_PIN);
                  printf("Status do relay1: ON \n");
                  vTaskDelay(1000/portTICK_PERIOD_MS);
                  relay_set_on(RELAY2_PIN);
                  printf("Status do relay2: ON\n");
                  vTaskDelay(1000/portTICK_PERIOD_MS);

            }else{
                
                relay_set_off(RELAY1_PIN);
                printf("Status do relay1: OFF\n");
                vTaskDelay(1000/portTICK_PERIOD_MS);
                relay_set_off(RELAY2_PIN);
                printf("Status do relay2: OFF\n");
                vTaskDelay(1000/portTICK_PERIOD_MS);

            }


            ESP_LOGI(TAG, "Luminosidade: %.2f Lux", lux);
        } else {
            ESP_LOGE(TAG, "Erro ao ler sensor! Verifique conexões.");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}

