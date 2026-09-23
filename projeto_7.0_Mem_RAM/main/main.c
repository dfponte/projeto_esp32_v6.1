#include <stdio.h>
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_partition.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "ESTUDO_SRAM_N16R8";

void app_main(void)
{
    ESP_LOGI(TAG, "=== DIAGNÓSTICO EXCLUSIVO SRAM INTERNA (Hardware: N16R8) ===");

    // =========================================================================
    // 1. AVALIAÇÃO DETALHADA DA SRAM INTERNA
    // =========================================================================
    ESP_LOGI(TAG, "[ MEMÓRIA SRAM INTERNA ]");

    // DRAM: Memória de Dados Interna (Onde o seu código roda e aloca por padrão)
    size_t free_dram = heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    size_t total_dram = heap_caps_get_total_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    size_t min_dram = heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    
    // Métrica Científica de Fragmentação: Maior bloco contíguo livre no Heap interno
    size_t max_block_dram = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);

    // IRAM: Memória de Instruções Interna
    size_t free_iram = heap_caps_get_free_size(MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL) - free_dram;
    size_t total_iram = heap_caps_get_total_size(MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL) - total_dram;

    // Uso correto do formatador '%zu' para tipos 'size_t' (SRAM interna)
    printf("\n");
    printf("SRAM de Dados (DRAM) Total : %7zu Bytes (%zu KB)\n", total_dram, total_dram / 1024);
    printf("SRAM de Dados (DRAM) Livre : %7zu Bytes (%zu KB)\n", free_dram, free_dram / 1024);
    printf("Maior Bloco Livre na DRAM  : %7zu Bytes (%zu KB) <-- (Métrica de Fragmentação)\n", max_block_dram, max_block_dram / 1024);
    printf("Menor RAM Livre já atingida: %7zu Bytes (%zu KB) <-- (Métrica Crítica / Watermark)\n", min_dram, min_dram / 1024);
    printf("------------------------------------------------------------\n");
    printf("SRAM de Código (IRAM) Total: %7zu Bytes (%zu KB)\n", total_iram, total_iram / 1024);
    printf("SRAM de Código (IRAM) Livre: %7zu Bytes (%zu KB)\n", free_iram, free_iram / 1024);
    printf("\n");

    // Validação de isolamento da PSRAM (Como está desativada no menuconfig, deve retornar 0)
    size_t total_psram = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    if (total_psram > 0) {
        ESP_LOGW(TAG, "AVISO: PSRAM de %zu MB detectada! Desative-a no menuconfig para isolar o estudo.", total_psram / (1024 * 1024));
    } else {
        ESP_LOGI(TAG, "PSRAM Externa: ISOLADA / DESATIVADA com sucesso via menuconfig.");
    }

    // =========================================================================
    // 2. MAPEAMENTO DE PARTIÇÕES DA FLASH (16MB)
    // =========================================================================
    ESP_LOGI(TAG, "[ TABELA DE PARTIÇÕES DA FLASH (Capacidade: 16MB) ]");
    printf("\n%-10s %-10s %-12s %-10s %-10s\n", "Tipo", "Subtipo", "Endereço", "Tamanho", "Label");
    printf("------------------------------------------------------------\n");

    // Procura por todas as partições cadastradas na memória Flash
    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL);
    
    while (it != NULL) {
        const esp_partition_t *part = esp_partition_get(it);
        
        const char* type_str = "UNKNOWN";
        if (part->type == ESP_PARTITION_TYPE_APP) {
            type_str = "APP";
        } else if (part->type == ESP_PARTITION_TYPE_DATA) {
            type_str = "DATA";
        }

        // Correção de Tipo: Cast explícito para (unsigned long) com o formatador '%lu'
        printf("%-10s 0x%02X       0x%08lx   %-10lu %-10s\n", 
               type_str, 
               part->subtype, 
               (unsigned long)part->address, 
               (unsigned long)part->size, 
               part->label);
                      
        it = esp_partition_next(it);
    }
    
    // Libera o iterador de partições de forma segura
    esp_partition_iterator_release(it);
    printf("------------------------------------------------------------\n\n");
}