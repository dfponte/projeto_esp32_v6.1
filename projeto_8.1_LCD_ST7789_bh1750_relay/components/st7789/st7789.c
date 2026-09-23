#include <stdio.h>
#include "st7789.h"
#include "esp_log.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

#define LCD_HOST         SPI2_HOST
#define PIN_NUM_SCLK     12
#define PIN_NUM_MOSI     11
#define PIN_NUM_LCD_DC   9
#define PIN_NUM_LCD_CS   10
#define PIN_NUM_LCD_RST  3
#define PIN_NUM_BK_LIGHT 46

#define LCD_H_RES        240
#define LCD_V_RES        320

static const char *TAG = "st7789_driver";
static lv_disp_t *lvgl_disp = NULL;

void st7789_init(void)
{
    ESP_LOGI(TAG, "Configurando GPIO do Backlight...");
    gpio_config_t bk_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << PIN_NUM_BK_LIGHT),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&bk_conf);
    gpio_set_level(PIN_NUM_BK_LIGHT, 1); 

    ESP_LOGI(TAG, "Inicializando Barramento SPI do Display...");
    spi_bus_config_t buscfg = {
        .sclk_io_num = PIN_NUM_SCLK,
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_H_RES * 80 * sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

    ESP_LOGI(TAG, "Configurando Interface SPI (IO) do Driver...");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = PIN_NUM_LCD_DC,
        .cs_gpio_num = PIN_NUM_LCD_CS,
        .pclk_hz = 40 * 1000 * 1000, 
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));

    ESP_LOGI(TAG, "Instanciando o driver nativo do ST7789...");
    esp_lcd_panel_handle_t panel_handle = NULL;
    
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true)); 
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    ESP_LOGI(TAG, "Inicializando esp_lvgl_port...");
    
    // --- CONFIGURAÇÃO CORRIGIDA PARA A TASK DO PORT EM ESP-IDF V6.1 ---
    lvgl_port_cfg_t lvgl_port_cfg = {
        .task_priority = 4,       
        .task_stack = 4096,       
        .task_affinity = -1,      
        .task_max_sleep_ms = 10,  
        .timer_period_ms = 2      
    };
    ESP_ERROR_CHECK(lvgl_port_init(&lvgl_port_cfg));

    ESP_LOGI(TAG, "Registrando display no ecossistema do LVGL...");
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = LCD_H_RES * 40,
        .double_buffer = true,
        .hres = LCD_H_RES,
        .vres = LCD_V_RES,
        .monochrome = false,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        },
        .flags = {
            .buff_dma = true,
        }
    };
    lvgl_disp = lvgl_port_add_disp(&disp_cfg);
    
    ESP_LOGI(TAG, "Driver ST7789 e inicializador do LVGL prontos!");
}
