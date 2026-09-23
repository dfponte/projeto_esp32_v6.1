
#ifndef BH1750_H
#define BH1750_H

#include "driver/i2c_master.h" // <-- ESSENCIAL: Diz ao .h o que é i2c_master_dev_handle_t

// --- CONFIGURAÇÃO DO SENSOR BH1750 ---
#define BH1750_ADDR          0x23   // Endereço I2C padrão com pino ADDR no GND
#define BH1750_CMD_CONT_HIGH 0x10   // Comando de amostragem contínua de alta resolução

// --- DEFINIÇÃO DOS PINOS I2C (Ajuste conforme seus pinos reais se necessário) ---
#define I2C_SDA_PIN          4      // GPIO4 para o SDA
#define I2C_SCL_PIN          5      // GPIO5 para o SCL

// Assinatura pública da função de inicialização
i2c_master_dev_handle_t i2c_init(void);

#endif // BH1750_H
