
#include <stdio.h>
#include "bh1750.h"
#include "esp_log.h"
#include "driver/i2c_master.h"


i2c_master_dev_handle_t dev_handle;
 

i2c_master_dev_handle_t i2c_init(){

     // 1. Configurar o barramento I2C Master
    i2c_master_bus_config_t bus_cfg = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = I2C_SCL_PIN,
        .sda_io_num = I2C_SDA_PIN,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true, // Ativa pull-ups internos do S3
    };
    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &bus_handle));

        // 2. Adicionar o BH1750 como dispositivo no barramento
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = BH1750_ADDR,
        .scl_speed_hz = 100000, // 100kHz (Standard Mode)
    };
   
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    // 3. Comando para iniciar medição contínua
    uint8_t cmd = BH1750_CMD_CONT_HIGH;
    ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, &cmd, 1, -1));

  return dev_handle;
 }