#include "esp_check.h"
#include "esp_err.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_qemu_rgb.h"
#include "esp_log.h"
#include "lvgl.h"

#include "esp_lvgl_port.h"
#include "main.h"
#include "sdkconfig.h"

static const char *TAG = "qemu";

#define QEMU_LCD_H_RES (480)
#define QEMU_LCD_V_RES (480)

static esp_lcd_panel_handle_t lcd_panel = NULL;
static lv_display_t *qemu_disp = NULL;
static lv_color_t *qemu_draw_buf = NULL;

lv_indev_t *encoder_indev = NULL;

static void qemu_lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    esp_lcd_panel_handle_t panel = (esp_lcd_panel_handle_t)lv_display_get_user_data(disp);

    (void)area;
    if (lv_display_flush_is_last(disp)) {
        esp_lcd_panel_draw_bitmap(panel, 0, 0, QEMU_LCD_H_RES, QEMU_LCD_V_RES, px_map);
    }
    lv_display_flush_ready(disp);
}

static esp_err_t qemu_lvgl_display_add(void)
{
    ESP_RETURN_ON_ERROR(esp_lcd_rgb_qemu_get_frame_buffer(lcd_panel, (void **)&qemu_draw_buf),
                        TAG, "Get QEMU frame buffer failed");
    if (!qemu_draw_buf) {
        ESP_LOGE(TAG, "QEMU frame buffer is NULL");
        return ESP_FAIL;
    }

    const size_t buf_px = QEMU_LCD_H_RES * QEMU_LCD_V_RES;
    ESP_LOGI(TAG, "Using QEMU VRAM as LVGL draw buffer (%u x %u, %u px)",
             QEMU_LCD_H_RES, QEMU_LCD_V_RES, (unsigned)buf_px);

    lvgl_port_lock(-1);
    qemu_disp = lv_display_create(QEMU_LCD_H_RES, QEMU_LCD_V_RES);
    if (!qemu_disp) {
        lvgl_port_unlock();
        return ESP_FAIL;
    }
    lv_display_set_color_format(qemu_disp, LV_COLOR_FORMAT_RGB565);
    lv_display_set_flush_cb(qemu_disp, qemu_lvgl_flush_cb);
    lv_display_set_user_data(qemu_disp, lcd_panel);
    lv_display_set_buffers(qemu_disp, qemu_draw_buf, NULL, buf_px * sizeof(uint16_t),
                           LV_DISPLAY_RENDER_MODE_DIRECT);

    lv_obj_t *stub = lv_obj_create(NULL);
    if (stub) {
        lv_obj_set_size(stub, 10, 10);
        lv_screen_load(stub);
    }

    lvgl_port_unlock();

    ESP_LOGI(TAG, "LVGL display registered for QEMU virtual panel");
    return ESP_OK;
}

esp_err_t app_lcd_init(void)
{
    ESP_LOGI(TAG, "Initialize QEMU virtual RGB panel (%dx%d)",
             QEMU_LCD_H_RES, QEMU_LCD_V_RES);

    const esp_lcd_rgb_qemu_config_t panel_conf = {
        .width = QEMU_LCD_H_RES,
        .height = QEMU_LCD_V_RES,
        .bpp = RGB_QEMU_BPP_16,
    };

    ESP_RETURN_ON_ERROR(esp_lcd_new_rgb_qemu(&panel_conf, &lcd_panel),
                        TAG, "QEMU RGB init failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_init(lcd_panel), TAG, "QEMU LCD init failed");

    ESP_LOGW(TAG, "Touch, encoder and button are not available in QEMU, use CLI for input");
    return ESP_OK;
}

esp_err_t lvgl_port_display_init(void)
{
    const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 10,
        .task_stack = 6144 * 2,
        .task_affinity = 1,
        .task_max_sleep_ms = 500,
        .timer_period_ms = 25
    };

    ESP_RETURN_ON_ERROR(lvgl_port_init(&lvgl_cfg), TAG, "LVGL port initialization failed");
    ESP_RETURN_ON_ERROR(qemu_lvgl_display_add(), TAG, "QEMU LVGL display failed");
    return ESP_OK;
}
