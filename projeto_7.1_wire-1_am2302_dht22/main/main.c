
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// CONFIRME O PINO GPIO ONDE SEU SENSOR ESTÁ CONECTADO
#define DHT_GPIO_PIN    GPIO_NUM_4 

static const char *TAG = "TESTE_CORRETO";

static int wait_or_timeout(int micro_sec, int level) {
    int micros_elapsed = 0;
    while (gpio_get_level(DHT_GPIO_PIN) == level) {
        if (micros_elapsed > micro_sec) return -1; 
        ets_delay_us(1);
        micros_elapsed++;
    }
    // FILTRO DE RUÍDO: Se o pulso em HIGH durar menos de 12us, é ruído elétrico.
    // Ignoramos e retornamos um tempo mínimo padrão de bit 0 (17us) para não quebrar o loop.
    if (level == 1 && micros_elapsed < 12) {
        return 17; 
    }
    return micros_elapsed;
}


void test_sensor_task(void *pvParameters) {
    uint8_t data[5] = {0}; 
    int tempos_salvos[40] = {0}; // Array para guardar os tempos e imprimir depois

    while (1) {
        memset(data, 0, sizeof(data));
        memset(tempos_salvos, 0, sizeof(tempos_salvos));
        
        ESP_LOGI(TAG, "----------------------------------------");
        ESP_LOGI(TAG, "Iniciando leitura ultra-veloz...");

        // 1. Enviar sinal de Start
        gpio_set_direction(DHT_GPIO_PIN, GPIO_MODE_OUTPUT);
        gpio_set_level(DHT_GPIO_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(20)); 
        gpio_set_level(DHT_GPIO_PIN, 1);
        ets_delay_us(30);             
        
        // 2. Mudar para entrada
        gpio_set_direction(DHT_GPIO_PIN, GPIO_MODE_INPUT);

        // 3. Checar resposta do Sensor
        if (wait_or_timeout(200, 1) < 0 || wait_or_timeout(200, 0) < 0 || wait_or_timeout(200, 1) < 0) {
            ESP_LOGE(TAG, "[ERRO] Falha na resposta inicial do sensor.");
            vTaskDelay(pdMS_TO_TICKS(3000));
            continue;
        }

        // 4. Ler os 40 bits na velocidade máxima (Sem printfs aqui dentro!)
        bool erro_bit = false;
        for (int i = 0; i < 40; i++) {
            if (wait_or_timeout(200, 0) < 0) { erro_bit = true; break; }
            
            int high_time = wait_or_timeout(200, 1);
            if (high_time < 0) { erro_bit = true; break; }

            tempos_salvos[i] = high_time; // Salva o tempo na memória ram rapidamente

            int byte_idx = i / 8;
            data[byte_idx] <<= 1;
            if (high_time > 30) {
                data[byte_idx] |= 1;
            }
        }

        // 5. Imprimir os diagnósticos APÓS o término da leitura
        ESP_LOGI(TAG, "Análise dos tempos dos bits coletados:");
        for (int i = 0; i < 40; i++) {
            if (tempos_salvos[i] == 0 && erro_bit && i >= 14) {
                printf("Bit %02d: TRAVOU | ", i);
            } else {
                printf("Bit %02d:%dus | ", i, tempos_salvos[i]);
            }
            if ((i + 1) % 5 == 0) printf("\n");
        }
        printf("\n");

        if (erro_bit) {
            ESP_LOGE(TAG, "[ERRO] O sensor parou de responder durante a transmissão.");
        } else {
            // 6. Validar Checksum
            uint8_t checksum = (data[0] + data[1] + data[2] + data[3]) & 0xFF;
            if (data[4] == checksum) {
                int16_t raw_humidity = (data[0] << 8) | data[1];
                int16_t raw_temperature = ((data[2] & 0x7F) << 8) | data[3];
                if (data[2] & 0x80) raw_temperature = -raw_temperature;

                float humidity = raw_humidity / 10.0;
                float temperature = raw_temperature / 10.0;

                ESP_LOGW(TAG, "[SUCESSO] Leitura realizada com perfeição!");
                ESP_LOGI(TAG, "Resultados -> Umidade: %.1f%% | Temperatura: %.1f°C", humidity, temperature);
            } else {
                ESP_LOGE(TAG, "[ERRO] Checksum inválido.");
                ESP_LOGE(TAG, "Recebido: %02X %02X %02X %02X %02X | Esperado: %02X", 
                         data[0], data[1], data[2], data[3], data[4], checksum);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(3000)); 
    }
}


void app_main(void) {
    gpio_reset_pin(DHT_GPIO_PIN);
    gpio_set_pull_mode(DHT_GPIO_PIN, GPIO_PULLUP_ONLY);
    xTaskCreate(test_sensor_task, "test_sensor_task", 4096, NULL, 5, NULL);
}
