#pragma once

#include "esp_log.h"
#include "esp_lvgl_port.h"

#include "esp_err.h"

void board_init();
void cli_init();

esp_err_t app_lcd_init();
esp_err_t lvgl_port_display_init();


