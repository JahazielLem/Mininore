#ifndef DISPLAY_H
#define DISPLAY_H

#include "sh1106.h"
#include "display_helper.h"
#include "wifi_sniffer.h"
#include "buzzer.h"
#include "bluetooth_scanner.h"

#define INVERT 1
#define NO_INVERT 0

extern uint8_t selected_option;
extern Layer previous_layer;
extern Layer current_layer;
extern int num_items;

// 'logo-1', 128x64px
static const unsigned char epd_bitmap_logo_1[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x07, 0xff, 0xc0, 0x21, 0xfc, 0x3f, 0xfe, 0x00, 0x7f, 0x0f, 0xff, 0x80, 0x1f, 0xff, 0xfc, 0x00,
    0x03, 0xff, 0xc0, 0x70, 0xfe, 0x1f, 0xfe, 0x0c, 0x3f, 0x87, 0xff, 0x83, 0x0f, 0xff, 0xfe, 0x00,
    0x03, 0xff, 0xc0, 0x78, 0x7f, 0x0f, 0xff, 0x0e, 0x1f, 0xc3, 0xff, 0xc3, 0x87, 0xff, 0xff, 0x00,
    0x01, 0xff, 0xc0, 0xf8, 0x7f, 0x87, 0xff, 0x0e, 0x1f, 0xe1, 0xff, 0xc3, 0x83, 0xff, 0xff, 0x80,
    0x00, 0xff, 0xc1, 0xfc, 0x3f, 0xc3, 0xff, 0x1f, 0x0f, 0xf0, 0xff, 0xc7, 0xc1, 0xff, 0xff, 0xc0,
    0x00, 0x7f, 0xc3, 0xfe, 0x1f, 0xc3, 0xff, 0x1f, 0x87, 0xf0, 0xff, 0xc7, 0xe1, 0xff, 0xff, 0xc0,
    0x00, 0x7f, 0xc3, 0xfe, 0x1f, 0xc1, 0xff, 0x1f, 0x87, 0xf0, 0x7f, 0xc7, 0xe0, 0xff, 0x3f, 0xc0,
    0x00, 0x3f, 0xc7, 0xfe, 0x1f, 0xc1, 0xff, 0xbf, 0x87, 0xf0, 0x7f, 0xef, 0xe0, 0xfe, 0x0f, 0xc0,
    0x00, 0x7f, 0xcf, 0xfe, 0x1f, 0xc3, 0xff, 0xbf, 0x87, 0xf0, 0xff, 0xef, 0xe0, 0xfc, 0x0f, 0xc0,
    0x00, 0x7f, 0xcf, 0xfe, 0x1f, 0xc3, 0xff, 0xff, 0x07, 0xf0, 0xff, 0xff, 0xc1, 0xfc, 0x1f, 0xc0,
    0x00, 0xff, 0xdf, 0xfc, 0x3f, 0x87, 0xff, 0xff, 0x0f, 0xe1, 0xff, 0xff, 0xc1, 0xf8, 0x1f, 0xc0,
    0x00, 0xff, 0xff, 0xfc, 0x3f, 0x87, 0xff, 0xff, 0x0f, 0xe1, 0xff, 0xff, 0xc1, 0xf8, 0x1f, 0x80,
    0x00, 0xff, 0xff, 0xfc, 0x3f, 0x87, 0xff, 0xff, 0x0f, 0xe1, 0xff, 0xff, 0xc3, 0xf8, 0x3f, 0x80,
    0x01, 0xff, 0xff, 0xf8, 0x7f, 0x07, 0xff, 0xfe, 0x1f, 0xc1, 0xff, 0xff, 0x83, 0xf8, 0x3f, 0x80,
    0x01, 0xff, 0xff, 0xf8, 0x7f, 0x0f, 0xff, 0xfe, 0x1f, 0xc3, 0xff, 0xff, 0x83, 0xf0, 0x3f, 0x00,
    0x01, 0xff, 0xff, 0xf8, 0x7f, 0x0f, 0xef, 0xfe, 0x1f, 0xc3, 0xfb, 0xff, 0x83, 0xf0, 0x7f, 0x00,
    0x01, 0xff, 0xff, 0xf8, 0x7f, 0x0f, 0xcf, 0xfc, 0x1f, 0xc3, 0xf3, 0xff, 0x07, 0xf0, 0x7f, 0x00,
    0x03, 0xff, 0xfb, 0xf0, 0xfe, 0x1f, 0xcf, 0xfc, 0x3f, 0x87, 0xf3, 0xff, 0x07, 0xfd, 0xfe, 0x00,
    0x03, 0xff, 0xf3, 0xf0, 0xfe, 0x1f, 0xcf, 0xfc, 0x3f, 0x87, 0xf3, 0xff, 0x07, 0xff, 0xfe, 0x00,
    0x03, 0xf7, 0xf7, 0xf0, 0xfe, 0x1f, 0xc7, 0xfc, 0x3f, 0x87, 0xf1, 0xff, 0x07, 0xff, 0xfc, 0x00,
    0x03, 0xf3, 0xe7, 0xe0, 0xfc, 0x1f, 0x87, 0xf8, 0x3f, 0x07, 0xe1, 0xfe, 0x07, 0xff, 0xfc, 0x00,
    0x07, 0xf3, 0xc7, 0xe1, 0xfc, 0x3f, 0x87, 0xf8, 0x7f, 0x0f, 0xe1, 0xfe, 0x07, 0xff, 0xf8, 0x00,
    0x07, 0xf3, 0x8f, 0xe1, 0xf8, 0x3f, 0x87, 0xf8, 0x7e, 0x0f, 0xe1, 0xfe, 0x03, 0xff, 0xf0, 0x00,
    0x07, 0xe3, 0x0f, 0xc0, 0xf8, 0x3f, 0x03, 0xf0, 0x3e, 0x0f, 0xc0, 0xfc, 0x01, 0xff, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// 'face-logo', 32x32px
static const unsigned char epd_bitmap_face_logo[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x30, 0x0e, 0x00, 0x00, 0x70,
    0x1f, 0x00, 0x00, 0xf8, 0x1f, 0x80, 0x01, 0xf8, 0x3f, 0xe0, 0x07, 0xfc, 0x3f, 0xf0, 0x0f, 0xfc,
    0x3f, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xfe,
    0x7f, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xfe,
    0x7e, 0x1f, 0xf8, 0x7e, 0x7c, 0x0f, 0xf0, 0x3e, 0x78, 0x07, 0xe0, 0x1e, 0x78, 0x07, 0xe0, 0x1e,
    0x78, 0x07, 0xe0, 0x1e, 0x3c, 0x0f, 0xf0, 0x3c, 0x3e, 0x1f, 0xf8, 0x7c, 0x1f, 0xff, 0xff, 0xf8,
    0x0f, 0xfe, 0x7f, 0xf0, 0x07, 0xfc, 0x3f, 0xe0, 0x03, 0xff, 0xff, 0xc0, 0x00, 0xff, 0xff, 0x00,
    0x00, 0x3f, 0xfc, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void display_init(void);
void display_clear(void);
void display_show(void);
void display_text(const char* text, int x, int page, int invert);
void display_clear_line(int page, int invert);
void display_selected_item_box();
char** add_empty_strings(char** array, int length);
char** remove_srolling_text_flag(char** items, int length);
char** get_menu_items();
void display_menu_items(char** items);
void display_scrolling_text(char** text);
void display_menu();
void display_wifi_sniffer(wifi_sniffer_record_t record);
void display_bluetooth_scanner(bluetooth_scanner_record_t record);
void display_thread_cli();
void display_about_info();
void display_in_development_banner();
void display_gps_init();
void display_gps_deinit();

#endif // DISPLAY_H