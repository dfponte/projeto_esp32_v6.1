#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "lvgl.h"
#include "st7789.h" 

static const char *TAG = "main_app";

// --- DECLARAÇÃO DAS TRÊS IMAGENS ---
LV_IMAGE_DECLARE(term_80x80); 
LV_IMAGE_DECLARE(Inte_80x80); 
LV_IMAGE_DECLARE(btn_80x80);  

// --- MONTAGEM DA INTERFACE GRÁFICA ---
void construir_interface(void) {
    static lv_style_t estilo_texto;
    lv_style_init(&estilo_texto);
    lv_style_set_text_font(&estilo_texto, &lv_font_montserrat_22); 
    lv_style_set_text_color(&estilo_texto, lv_color_white());

    lv_obj_t * img_termo = lv_image_create(lv_screen_active());
    lv_image_set_src(img_termo, &term_80x80);
    lv_obj_align(img_termo, LV_ALIGN_TOP_LEFT, 10, 15);

    lv_obj_t *label_temp = lv_label_create(lv_screen_active());
    lv_obj_add_style(label_temp, &estilo_texto, 0);
    lv_label_set_text(label_temp, "25 °C");
    lv_obj_align_to(label_temp, img_termo, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

    lv_obj_t * img_luz = lv_image_create(lv_screen_active());
    lv_image_set_src(img_luz, &Inte_80x80);
    lv_obj_align_to(img_luz, img_termo, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    lv_obj_t *label_lux_val = lv_label_create(lv_screen_active());
    lv_obj_add_style(label_lux_val, &estilo_texto, 0);
    lv_label_set_text(label_lux_val, "150 LUX");
    lv_obj_align_to(label_lux_val, img_luz, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

    lv_obj_t * img_btn = lv_image_create(lv_screen_active());
    lv_image_set_src(img_btn, &btn_80x80);
    lv_obj_align_to(img_btn, img_luz, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    lv_obj_t *label_status = lv_label_create(lv_screen_active());
    lv_obj_add_style(label_status, &estilo_texto, 0);
    lv_label_set_text(label_status, "PRONTO");
    lv_obj_set_style_text_color(label_status, lv_color_make(60, 255, 100), 0); 
    lv_obj_align_to(label_status, img_btn, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
}

void app_main(void) {
    ESP_LOGI(TAG, "Iniciando sistema...");

    // 1. Inicializa o hardware (Chama o componente isolado)
    st7789_init();

    // 2. Constrói a interface localmente (Onde as imagens existem de verdade)
    lvgl_port_lock(0);
    construir_interface(); 
    lvgl_port_unlock();

    ESP_LOGI(TAG, "Sistema rodando perfeitamente.");
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
