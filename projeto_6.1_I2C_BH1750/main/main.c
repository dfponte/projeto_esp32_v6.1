#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c_master.h"

#include "I2C_BH1750.h"

void app_main()
{
  // Declare e inicialize recebendo o retorno da função
        i2c_master_dev_handle_t dev_handle = i2c_init();

     while (1) {

      
        uint8_t data[2];
        // 4. Receber 2 bytes de dados
        esp_err_t ret = i2c_master_receive(dev_handle, data, 2, -1);
        
        if (ret == ESP_OK) {
            // Cálculo: (MSB << 8 | LSB) / 1.2
            float lux = (float)((data[0] << 8) | data[1]) / 1.2;
            ESP_LOGI(TAG, "Luminosidade: %.2f Lux", lux);
        } else {
            ESP_LOGE(TAG, "Erro ao ler sensor! Verifique conexões.");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}
