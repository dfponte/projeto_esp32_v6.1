
#ifndef I2C_BH1750_H
#define I2C_BH1750_H

#include "driver/i2c_master.h"

static const char *TAG = "BH1750_S3";

// Configuração de Pinos para o S3
#define I2C_SDA_PIN             8   // Seu pino SDA
#define I2C_SCL_PIN             9   // Sugestão para SCL (comum no S3)
#define BH1750_ADDR             0x23 
#define BH1750_CMD_CONT_HIGH    0x10 

i2c_master_dev_handle_t i2c_init(void);


#endif