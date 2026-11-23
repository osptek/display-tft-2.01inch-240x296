
#include "config.h"

static esp_err_t app_lcd_init(void)
{
    esp_err_t ret = ESP_OK;

#if CURRENT_SCREEN_SIZE == YDP169H003V3_240X280

    ESP_LOGI(TAG, "Turn off LCD backlight");
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_BK_LIGHT
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));

    ESP_LOGI(TAG, "Initialize SPI bus");
    spi_bus_config_t buscfg = {
        .sclk_io_num = EXAMPLE_PIN_NUM_SCLK,
        .mosi_io_num = EXAMPLE_PIN_NUM_MOSI,
        .miso_io_num = EXAMPLE_PIN_NUM_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = EXAMPLE_LCD_H_RES * EXAMPLE_LCD_DRAW_BUFF_HEIGHT * sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = EXAMPLE_PIN_NUM_LCD_DC,
        .cs_gpio_num = EXAMPLE_PIN_NUM_LCD_CS,
        .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,
        .lcd_param_bits = EXAMPLE_LCD_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    // 将 LCD 连接到 SPI 总线
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));

    st7789v_vendor_config_t vendor_config = {
        .init_cmds = lcd_init_cmds,
        .init_cmds_size = sizeof(lcd_init_cmds) / sizeof(st7789v_lcd_init_cmd_t),
    };

    ESP_LOGI(TAG, "Install ST7789V panel driver");
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
        .bits_per_pixel = 16,
        .vendor_config = &vendor_config,
    };

    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_st7789v(io_handle, &panel_config, &panel_handle), TAG, "LCD init failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_reset(panel_handle), TAG, "LCD init failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_init(panel_handle), TAG, "LCD init failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_set_gap(panel_handle, 0, 20), TAG, "LCD init failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_disp_on_off(panel_handle, true), TAG, "LCD init failed");

    ESP_LOGI(TAG, "Turn on LCD backlight");
    gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_ON_LEVEL);

#elif CURRENT_SCREEN_SIZE == YDP201H001V3_240X296

    ESP_LOGI(TAG, "Turn off LCD backlight");
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_BK_LIGHT
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));

    ESP_LOGI(TAG, "Initialize Intel 8080 bus");
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
    .clk_src = LCD_CLK_SRC_DEFAULT,
    .dc_gpio_num = EXAMPLE_PIN_NUM_LCD_DC,
    .wr_gpio_num = EXAMPLE_PIN_NUM_LCD_WR,
    .data_gpio_nums = {
        EXAMPLE_PIN_NUM_LCD_DATA0,
        EXAMPLE_PIN_NUM_LCD_DATA1,
        EXAMPLE_PIN_NUM_LCD_DATA2,
        EXAMPLE_PIN_NUM_LCD_DATA3,
        EXAMPLE_PIN_NUM_LCD_DATA4,
        EXAMPLE_PIN_NUM_LCD_DATA5,
        EXAMPLE_PIN_NUM_LCD_DATA6,
        EXAMPLE_PIN_NUM_LCD_DATA7,
    },
    .bus_width = 8,
    .max_transfer_bytes = EXAMPLE_LCD_H_RES * EXAMPLE_LCD_DRAW_BUFF_HEIGHT * sizeof(uint16_t),
    .dma_burst_size = 64,
};
    ESP_ERROR_CHECK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));

    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = EXAMPLE_PIN_NUM_LCD_CS, 
        .pclk_hz = 20 * 1000 * 1000,           
        .trans_queue_depth = 10,               
        .dc_levels = {                         
            .dc_idle_level = 0,                
            .dc_cmd_level = 0,                 
            .dc_dummy_level = 0,               
            .dc_data_level = 1,                
        },                                     
        .flags = {                             
            .swap_color_bytes = 0,             
        },                                             
        .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,                          
        .lcd_param_bits = EXAMPLE_LCD_PARAM_BITS,                        
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));

    st7789v_vendor_config_t vendor_config = {
        .init_cmds = lcd_init_cmds,
        .init_cmds_size = sizeof(lcd_init_cmds) / sizeof(st7789v_lcd_init_cmd_t),
    };

    ESP_LOGI(TAG, "Install ST7789V panel driver");
    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_LCD_RST,      // Set to -1 if not use
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
        .vendor_config = &vendor_config,
    };

    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_st7789v(io_handle, &panel_config, &panel_handle), TAG, "LCD init failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_reset(panel_handle), TAG, "LCD init failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_init(panel_handle), TAG, "LCD init failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_set_gap(panel_handle, 0, 12), TAG, "LCD init failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_invert_color(panel_handle, 1), TAG, "LCD init failed");//反转颜色 
    ESP_RETURN_ON_ERROR(esp_lcd_panel_disp_on_off(panel_handle, true), TAG, "LCD init failed");

    ESP_LOGI(TAG, "Turn on LCD backlight");
    gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_ON_LEVEL);

#elif CURRENT_SCREEN_SIZE == YDP180B001V1_240X284

    ESP_LOGI(TAG, "Turn off LCD backlight");
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_BK_LIGHT
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));

    ESP_LOGI(TAG, "Initialize SPI bus");
    spi_bus_config_t buscfg = {
        .sclk_io_num = EXAMPLE_PIN_NUM_SCLK,
        .mosi_io_num = EXAMPLE_PIN_NUM_MOSI,
        .miso_io_num = EXAMPLE_PIN_NUM_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = EXAMPLE_LCD_H_RES * EXAMPLE_LCD_DRAW_BUFF_HEIGHT * sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = EXAMPLE_PIN_NUM_LCD_DC,
        .cs_gpio_num = EXAMPLE_PIN_NUM_LCD_CS,
        .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,
        .lcd_param_bits = EXAMPLE_LCD_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    // 将 LCD 连接到 SPI 总线
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));

    nv3030b_vendor_config_t vendor_config = {
        .init_cmds = lcd_init_cmds,
        .init_cmds_size = sizeof(lcd_init_cmds) / sizeof(nv3030b_lcd_init_cmd_t),
    };

    ESP_LOGI(TAG, "Install NV3030B panel driver");
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
        .bits_per_pixel = 16,
        .vendor_config = &vendor_config,
    };
    
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_nv3030b(io_handle, &panel_config, &panel_handle), TAG, "LCD init failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_reset(panel_handle), TAG, "LCD init failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_init(panel_handle), TAG, "LCD init failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_disp_on_off(panel_handle, true), TAG, "LCD init failed");

    ESP_LOGI(TAG, "Turn on LCD backlight");
    gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_ON_LEVEL);

#else
  #error "Unsupported screen size"
#endif

    return ret;
}

static esp_err_t app_encoder_init(void)
{

    const button_config_t btn_cfg = {0};
    ESP_ERROR_CHECK(iot_button_new_gpio_device(&btn_cfg, &encoder_btn_config, &encoder_btn_handle));

    return ESP_OK;
}

static esp_err_t app_lvgl_init(void)
{
    /* Initialize LVGL */
    const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 4,         /* LVGL task priority */
        .task_stack = 4096*2,         /* LVGL task stack size */
        .task_affinity = -1,        /* LVGL task pinned to core (-1 is no affinity) */
        .task_max_sleep_ms = 500,   /* Maximum sleep in LVGL task */
        .timer_period_ms = 5        /* LVGL timer tick period in ms */
    };
    ESP_RETURN_ON_ERROR(lvgl_port_init(&lvgl_cfg), TAG, "LVGL port initialization failed");

    /* Add LCD screen */
    ESP_LOGD(TAG, "Add LCD screen");
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = EXAMPLE_LCD_H_RES * EXAMPLE_LCD_DRAW_BUFF_HEIGHT,
        .double_buffer = true,
        .hres = EXAMPLE_LCD_H_RES,
        .vres = EXAMPLE_LCD_V_RES,
        .monochrome = false,
#if LVGL_VERSION_MAJOR >= 9
        .color_format = LV_COLOR_FORMAT_RGB565,
#endif
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        },
        .flags = {
            .buff_dma = true,
            .buff_spiram = false,
#if LVGL_VERSION_MAJOR >= 9
            .swap_bytes = true,
#endif
        }
    };
    lvgl_disp = lvgl_port_add_disp(&disp_cfg);

    /* Encoder configuration structure */
    const lvgl_port_encoder_cfg_t encoder = {
        .disp = lvgl_disp,
        .encoder_a_b = &encoder_a_b_config,
        .encoder_enter = encoder_btn_handle,
    };

    /* Add encoder input (for selected screen) */
    disp_indev = lvgl_port_add_encoder(&encoder);

    return ESP_OK;
}
