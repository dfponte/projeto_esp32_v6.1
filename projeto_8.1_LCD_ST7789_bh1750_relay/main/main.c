#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "lvgl.h"
#include "st7789.h" 

#include "relay.h"
#include "bh1750.h"

// --- DEFINE OS PINOS DO RELÉ ---
#define RELAY1_PIN 21  // GPIO21
#define RELAY2_PIN 2   // GPIO2

static const char *TAG = "main_app";

// --- PONTEIROS GLOBAIS DA INTERFACE (MUDANÇA DINÂMICA) ---
static lv_obj_t *label_lux_val = NULL;
static lv_obj_t *label_status = NULL;

// --- DECLARAÇÃO DAS TRÊS IMAGENS ---
LV_IMAGE_DECLARE(term_80x80); 
LV_IMAGE_DECLARE(Inte_80x80); 
LV_IMAGE_DECLARE(btn_80x80);  

// --- MONTAGEM DA INTERFACE GRÁFICA ---
void construir_interface(void) {
    // Estilo base para textos grandes ao lado das figuras
    static lv_style_t estilo_texto;
    lv_style_init(&estilo_texto);
    lv_style_set_text_font(&estilo_texto, &lv_font_montserrat_22); 
    lv_style_set_text_color(&estilo_texto, lv_color_white());

    // ---- PRIMEIRA IMAGEM (Termômetro) ----
    lv_obj_t * img_termo = lv_image_create(lv_screen_active());
    lv_image_set_src(img_termo, &term_80x80);
    lv_obj_align(img_termo, LV_ALIGN_TOP_LEFT, 10, 15);

    lv_obj_t *label_temp = lv_label_create(lv_screen_active());
    lv_obj_add_style(label_temp, &estilo_texto, 0);
    lv_label_set_text(label_temp, "15 °C"); // Deixado temporariamente em 15 °C conforme pedido
    lv_obj_align_to(label_temp, img_termo, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

    // ---- SEGUNDA IMAGEM (Luz / Intensidade) ----
    lv_obj_t * img_luz = lv_image_create(lv_screen_active());
    lv_image_set_src(img_luz, &Inte_80x80);
    lv_obj_align_to(img_luz, img_termo, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    // Texto DINÂMICO do valor do Lux
    label_lux_val = lv_label_create(lv_screen_active());
    lv_obj_add_style(label_lux_val, &estilo_texto, 0);
    lv_label_set_text(label_lux_val, "--- LUX");
    lv_obj_align_to(label_lux_val, img_luz, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

    // ---- TERCEIRA IMAGEM (Botão / Relé) ----
    lv_obj_t * img_btn = lv_image_create(lv_screen_active());
    lv_image_set_src(img_btn, &btn_80x80);
    lv_obj_align_to(img_btn, img_luz, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    // Texto DINÂMICO do Estado do Relé / LED ao lado do botão
    label_status = lv_label_create(lv_screen_active());
    lv_obj_add_style(label_status, &estilo_texto, 0);
    lv_label_set_text(label_status, "PRONTO");
    lv_obj_set_style_text_color(label_status, lv_color_make(60, 255, 100), 0); 
    lv_obj_align_to(label_status, img_btn, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
}

// --- FUNÇÃO PRINCIPAL APP_MAIN ---
void app_main(void) {
    char buf_lux[32];

    ESP_LOGI(TAG, "Iniciando relés...");
    relay_init(false, RELAY1_PIN);
    relay_init(false, RELAY2_PIN);  

    ESP_LOGI(TAG, "Iniciando barramento I2C para BH1750...");
    i2c_master_dev_handle_t dev_handle = i2c_init();

    ESP_LOGI(TAG, "Iniciando hardware do Display ST7789...");
    st7789_init();

    ESP_LOGI(TAG, "Montando interface gráfica LVGL...");
    lvgl_port_lock(0);
    construir_interface(); 
    lvgl_port_unlock();

    ESP_LOGI(TAG, "Sistema e controle lógico ativos em loop principal.");

    while (1) {
        uint8_t data[2];
        // Receber 2 bytes de dados do sensor BH1750
        esp_err_t ret = i2c_master_receive(dev_handle, data, 2, -1);
                                                                      
        if (ret == ESP_OK) {
            // Cálculo da luminosidade bruta convertida para Lux
            float lux = (float)((data[0] << 8) | data[1]) / 1.2;
            ESP_LOGI(TAG, "Luminosidade medida: %.2f Lux", lux);

            // Atualiza a string do Lux no display com segurança mutex
            lvgl_port_lock(0);
            snprintf(buf_lux, sizeof(buf_lux), "%.1f LUX", lux);
            lv_label_set_text(label_lux_val, buf_lux);
            lvgl_port_unlock();

            // Lógica de Comparação: Lux abaixo de 10
            if (lux < 10.0f) {
                relay_set_on(RELAY1_PIN);
                printf("Status do relay1: ON \n");
                
                lvgl_port_lock(0);
                lv_label_set_text(label_status, "LED LIGADO");
                lv_obj_set_style_text_color(label_status, lv_color_make(255, 60, 60), 0); // Texto Vermelho/Alerta
                lvgl_port_unlock();

                vTaskDelay(pdMS_TO_TICKS(1000));
                
                relay_set_on(RELAY2_PIN);
                printf("Status do relay2: ON\n");
            } else {
                relay_set_off(RELAY1_PIN);
                printf("Status do relay1: OFF\n");
                
                lvgl_port_lock(0);
                lv_label_set_text(label_status, "LED DESLIGADO");
                lv_obj_set_style_text_color(label_status, lv_color_make(60, 255, 100), 0); // Texto Verde/Estável
                lvgl_port_unlock();

                vTaskDelay(pdMS_TO_TICKS(1000));
                
                relay_set_off(RELAY2_PIN);
                printf("Status do relay2: OFF\n");
            }

        } else {
            ESP_LOGE(TAG, "Erro ao ler sensor! Verifique conexões.");
            
            lvgl_port_lock(0);
            lv_label_set_text(label_lux_val, "ERRO SENS");
            lv_obj_set_style_text_color(label_lux_val, lv_color_make(255, 60, 60), 0);
            lvgl_port_unlock();
        }

        // Aguarda 1 segundo antes de realizar o próximo ciclo completo de amostragem
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
