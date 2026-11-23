#include "lvgl_init.h"


static void app_main_display(void)
{
    /* Show LVGL objects */
    lvgl_port_lock(0);

    // lv_demo_music();
    // lv_demo_benchmark();
    lv_demo_widgets();
    //lv_demo_keypad_encoder();

    /* Task unlock */
    lvgl_port_unlock();
}

void app_main(void)
{
    /* LCD HW initialization */
    ESP_ERROR_CHECK(app_lcd_init());

    /* Encoder initialization */
    ESP_ERROR_CHECK(app_encoder_init());

    /* LVGL initialization */
    ESP_ERROR_CHECK(app_lvgl_init());

    /* Show LVGL objects */
    app_main_display();
}