#include <stdbool.h>
#include <stdint.h>
#ifndef HID_SCREENS_H
  #define HID_SCREENS_H

typedef enum { HID_TREE_MENU, HID_TREE_DEVICE, HID_TREE_COUNT } hid_menu_t;

enum {
  HID_CONFIG_NAME,
  HID_CONFIG_MAC,
  HID_CONFIG_START,
  HID_MENU_COUNT
} hid_menu_item = HID_CONFIG_NAME;
const char* hid_menu_items[HID_MENU_COUNT] = {"Device name", "Device MAC",
                                              "Start"};

enum {
  HID_DEVICE_VOL_UP,
  HID_DEVICE_VOL_DOWN,
  HID_DEVICE_PLAY,
  HID_DEVICE_COUNT
} hid_device_item = HID_DEVICE_VOL_UP;
const char* hid_device_items[HID_DEVICE_COUNT] = {
    "Volumen Up",
    "Volumen Down",
    "Play/Pause",
};

void hid_module_register_menu(hid_menu_t menu);
void hid_clear_screen();
void hid_module_display_menu(uint16_t current_item);
void hid_module_display_device_name();
void hid_module_display_device_mac();
void hid_module_display_notify_volumen_up();
void hid_module_display_notify_volumen_down();
void hid_module_display_notify_play_pause();
void hid_module_display_device_connection(bool status);
void hid_module_display_device_pairing();
#endif  // HID_SCREENS_H
