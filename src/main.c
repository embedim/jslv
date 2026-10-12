#include "main.h"
#include "lvgl.h"

static const char* TAG = "main";

extern void create_screen_main();

void app_main() {
    board_init();

    ESP_LOGI(TAG, "Start init");
    ESP_ERROR_CHECK(app_lcd_init());
    ESP_ERROR_CHECK(lvgl_port_display_init());

    cli_init();

    ESP_LOGI(TAG, "Start LVGL");
    lvgl_port_lock(-1);
    create_screen_main();
    lvgl_port_unlock();

}
