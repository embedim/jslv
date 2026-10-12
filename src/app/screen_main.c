#include "main.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "screen_all.h"

lv_obj_t * screen_main_ptr = NULL;

lv_obj_t * create_base_screen(const char * title_text) {
    lv_obj_t * scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x101010), LV_PART_MAIN);
    lv_obj_set_style_text_color(scr, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_border_width(scr, 0, LV_PART_MAIN);
    lv_obj_remove_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    if(title_text) {
        lv_obj_t * title = lv_label_create(scr);
        lv_label_set_text(title, title_text);
        lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(title, lv_color_hex(0xCCCCCC), 0);
        lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 25);
    }

    return scr;
}

void create_screen_main() {

    lv_obj_t * scr  = create_base_screen("MAIN");
    screen_main_ptr = scr;
    lv_screen_load(scr);
}

