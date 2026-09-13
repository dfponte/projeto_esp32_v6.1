#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

static const char* TAG = "MEU LOG:";
void app_main(void)
{
 
 while(1){
     vTaskDelay(1000/portTICK_PERIOD_MS);
     ESP_LOGE(TAG,"Erro");
     vTaskDelay(1000/portTICK_PERIOD_MS);
     ESP_LOGW(TAG,"Warging");
     vTaskDelay(1000/portTICK_PERIOD_MS);
     ESP_LOGI(TAG,"Informação");
     vTaskDelay(1000/portTICK_PERIOD_MS);
     ESP_LOGD(TAG,"Debug");

 }

}
