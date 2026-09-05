#include <stdio.h>
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"

#define I2C_MASTER_NUM      I2C_NUM_0
#define I2C_SDA_IO          GPIO_NUM_7
#define I2C_SCL_IO          GPIO_NUM_6
#define I2C_FREQ_HZ         400000

#define MPU6050_ADDR        0x68
#define SSD1306_ADDR        0x3C

#define PIN_NUM_MISO        GPIO_NUM_20
#define PIN_NUM_MOSI        GPIO_NUM_19
#define PIN_NUM_CLK         GPIO_NUM_18
#define PIN_NUM_CS          GPIO_NUM_5

#define MOUNT_POINT         "/sdcard"

static const char *TAG = "System";

static const uint8_t font5x7[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // Space
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // "
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // #
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // &
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // (
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // )
    {0x14, 0x08, 0x3E, 0x08, 0x14}, // *
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // +
    {0x00, 0x50, 0x30, 0x00, 0x00}, // ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // -
    {0x00, 0x60, 0x60, 0x00, 0x00}, // .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // /
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ;
    {0x08, 0x14, 0x22, 0x41, 0x00}, // <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // =
    {0x00, 0x41, 0x22, 0x14, 0x08}, // >
    {0x02, 0x01, 0x51, 0x09, 0x06}, // ?
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // @
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // F
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
    {0x3F, 0x40, 0x38, 0x40, 0x3F}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
};

static uint8_t oled_buf[1024];

static esp_err_t i2c_master_init(void) {
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_SDA_IO;
    conf.scl_io_num = I2C_SCL_IO;
    conf.sda_pullup_en = GPIO_PULLUP_DISABLE;
    conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
    conf.master.clk_speed = I2C_FREQ_HZ;

    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
    return i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

static void ssd1306_cmd(uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};
    i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_ADDR, buf, 2, pdMS_TO_TICKS(100));
}

static void ssd1306_init(void) {
    ssd1306_cmd(0xAE); 
    ssd1306_cmd(0x20); ssd1306_cmd(0x00); 
    ssd1306_cmd(0xB0); ssd1306_cmd(0xC8);
    ssd1306_cmd(0x00); ssd1306_cmd(0x10); 
    ssd1306_cmd(0x40);
    ssd1306_cmd(0x81); ssd1306_cmd(0xFF);
    ssd1306_cmd(0xA1);
    ssd1306_cmd(0xA6);
    ssd1306_cmd(0xA8); ssd1306_cmd(0x3F);
    ssd1306_cmd(0xA4);
    ssd1306_cmd(0xD3); ssd1306_cmd(0x00);
    ssd1306_cmd(0xD5); ssd1306_cmd(0x80);
    ssd1306_cmd(0xD9); ssd1306_cmd(0xF1);
    ssd1306_cmd(0xDA); ssd1306_cmd(0x12);
    ssd1306_cmd(0xDB); ssd1306_cmd(0x40);
    ssd1306_cmd(0x8D); ssd1306_cmd(0x14);
    ssd1306_cmd(0xAF);
}

static void oled_clear(void) {
    memset(oled_buf, 0, sizeof(oled_buf));
}

static void oled_draw_string(int x, int page, const char *str) {
    while (*str) {
        char c = *str;
        if (c >= 'a' && c <= 'z') c -= 32;
        if (c >= 32 && c <= 90) {
            int font_idx = c - 32;
            for (int i = 0; i < 5; i++) {
                if (x + i < 128) {
                    oled_buf[page * 128 + x + i] = font5x7[font_idx][i];
                }
            }
        }
        x += 6;
        str++;
    }
}

static void oled_flush(void) {
    ssd1306_cmd(0x21); ssd1306_cmd(0); ssd1306_cmd(127);
    ssd1306_cmd(0x22); ssd1306_cmd(0); ssd1306_cmd(7);
    
    uint8_t packet[1025];
    packet[0] = 0x40;
    memcpy(&packet[1], oled_buf, 1024);
    i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_ADDR, packet, 1025, pdMS_TO_TICKS(100));
}

static esp_err_t mpu6050_init(void) {
    uint8_t pwr_mgmt[2] = {0x6B, 0x00};
    return i2c_master_write_to_device(I2C_MASTER_NUM, MPU6050_ADDR, pwr_mgmt, 2, pdMS_TO_TICKS(100));
}

static esp_err_t mpu6050_read_accel(float *ax, float *ay, float *az) {
    uint8_t reg = 0x3B;
    uint8_t raw[6];
    esp_err_t err = i2c_master_write_read_device(I2C_MASTER_NUM, MPU6050_ADDR, &reg, 1, raw, 6, pdMS_TO_TICKS(100));
    if (err == ESP_OK) {
        int16_t raw_x = (raw[0] << 8) | raw[1];
        int16_t raw_y = (raw[2] << 8) | raw[3];
        int16_t raw_z = (raw[4] << 8) | raw[5];
        
        *ax = (raw_x / 16384.0f) * 9.80665f;
        *ay = (raw_y / 16384.0f) * 9.80665f;
        *az = (raw_z / 16384.0f) * 9.80665f;
    }
    return err;
}

static sdmmc_card_t *card = NULL;

static esp_err_t sd_card_init(void) {
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {};
    mount_config.format_if_mount_failed = true;
    mount_config.max_files = 5;
    mount_config.allocation_unit_size = 16 * 1024;

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    
    spi_bus_config_t bus_cfg = {};
    bus_cfg.mosi_io_num = PIN_NUM_MOSI;
    bus_cfg.miso_io_num = PIN_NUM_MISO;
    bus_cfg.sclk_io_num = PIN_NUM_CLK;
    bus_cfg.quadwp_io_num = -1;
    bus_cfg.quadhd_io_num = -1;
    bus_cfg.max_transfer_sz = 4000;

    esp_err_t ret = spi_bus_initialize((spi_host_device_t)host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK) return ret;

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = (spi_host_device_t)host.slot;

    return esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);
}

static void sd_card_write_log(uint32_t seconds, float ax, float ay, float az) {
    FILE *f = fopen(MOUNT_POINT "/log.txt", "a");
    if (f == NULL) {
        printf("[ERRO] Falha ao abrir arquivo log.txt no Cartao SD!\n");
        return;
    }
    printf("[%lus] Accel: X=%.2f Y=%.2f Z=%.2f\n", 
            (unsigned long)seconds, ax, ay, az);
    fclose(f);
    printf("[OK] Anais salvos no Cartao SD (SPI)\n");
}

void app_main(void) {
    printf("Iniciando o sensor");

    if (i2c_master_init() == ESP_OK) {
        printf("Barramento I2C configurado.\n");
    } else {
        printf("Falha na inicializacao do Barramento I2C!\n");
    }

    if (mpu6050_init() == ESP_OK) {
        printf("[OK] MPU6050 acordado com sucesso.\n");
    } else {
        printf("Falha de comunicacao com o MPU6050!\n");
    }
    
    ssd1306_init();
    oled_clear();
    oled_flush();

    if (sd_card_init() == ESP_OK) {
        printf("Cartao SD montado com sucesso.\n");
    } else {
        printf("Falha ao montar o Cartao SD!\n");
    }

    uint32_t step_count = 0;

    while (1) {
        float ax = 0, ay = 0, az = 0;
        step_count++;

        if (mpu6050_read_accel(&ax, &ay, &az) == ESP_OK) {
            oled_clear();
            
            char line_str[32];
            snprintf(line_str, sizeof(line_str), "X:%.1f", ax);
            oled_draw_string(0, 0, line_str);

            snprintf(line_str, sizeof(line_str), "Y:%.1f", ay);
            oled_draw_string(0, 2, line_str);

            snprintf(line_str, sizeof(line_str), "Z:%.1f", az);
            oled_draw_string(0, 4, line_str);

            oled_flush();

            printf("[OK] Led sincronizado (I2C)\n");
            if (step_count % 2 == 0) {
                sd_card_write_log(step_count / 2, ax, ay, az);
            }

        } else {
            printf("[ERRO] Falha de leitura do I2C no sensor MPU6050!\n");
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}