#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// Define o pino físico da placa
#define LED_PIN GPIO_NUM_4

void app_main(void) {
    // 1. Limpa configurações anteriores do pino
    gpio_reset_pin(LED_PIN);
    
    // 2. Configura a GPIO como SAÍDA digital
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    // 3. Loop infinito do sistema
    while (1) {
        printf("[ESP32-S3] LED LIGADO\n");
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Aguarda 1s

        printf("[ESP32-S3] LED DESLIGADO\n");
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Aguarda 1s
    }
}
