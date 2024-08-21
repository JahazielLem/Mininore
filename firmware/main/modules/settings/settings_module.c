#include "settings_module.h"
#include <string.h>
#include "coroutine.h"
#include "display_settings.h"
#include "esp_log.h"
#include "file_manager_module.h"
#include "gps_module.h"
#include "menu_screens_modules.h"
#include "menus_module.h"
#include "modals_module.h"
#include "modules/settings/wifi/wifi_settings.h"
#include "oled_screen.h"
#include "preferences.h"
#include "sd_card.h"
#include "sd_card_settings_module.h"
#include "settings_module.h"
#include "stealth_mode.h"
#include "web_file_browser_module.h"

static const char* TAG = "settings_module";

void update_time_zone_options() {
  uint8_t selected_option = gps_module_get_time_zone();
  menu_screens_update_options(gps_time_zone_options, selected_option);
}

void update_sd_card_info() {
  sd_card_mount();
  oled_screen_clear();
  menu_screens_display_text_banner("Loading...");
  vTaskDelay(1000 / portTICK_PERIOD_MS);  // Wait for the SD card to be mounted
  sd_card_info_t sd_info = sd_card_get_info();

  char* name_str = malloc(strlen(sd_info.name) + 1);
  sprintf(name_str, "Name: %s", sd_info.name);
  char* capacity_str = malloc(20);
  sprintf(capacity_str, "Space: %.2fGB", ((float) sd_info.total_space) / 1024);
  char* speed_str = malloc(25);
  sprintf(speed_str, "Speed: %.2fMHz", sd_info.speed);
  char* type_str = malloc(strlen(sd_info.type) + 1 + 6);
  sprintf(type_str, "Type: %s", sd_info.type);

  sd_card_info[3] = name_str;
  sd_card_info[4] = capacity_str;
  sd_card_info[5] = speed_str;
  sd_card_info[6] = type_str;
  sd_card_unmount();
}

void settings_module_exit_submenu_cb() {
  screen_module_menu_t current_menu = menu_screens_get_current_menu();
  switch (current_menu) {
    case MENU_SETTINGS:
      settings_module_exit();
      break;
    default:
      break;
  }
}

void set_stealth_status() {
  uint8_t stealth_mode = preferences_get_bool("stealth_mode", false);
  char* stealth_options[] = {"Disabled", "Enabled"};
  stealth_mode = modals_module_get_radio_selection(
      stealth_options, "Stealth Mode", stealth_mode);
  preferences_put_bool("stealth_mode", stealth_mode);
  menus_module_set_app_state(false, NULL);
  menu_screens_exit_submenu();
  vTaskDelete(NULL);
}

void settings_module_enter_submenu_cb(screen_module_menu_t user_selection) {
  uint8_t selected_item = menu_screens_get_selected_item();
  ESP_LOGI(TAG, "Selected item: %d", selected_item);
  switch (user_selection) {
    case MENU_FILE_MANAGER_LOCAL:
      file_manager_module_init();
      break;
    case MENU_FILE_MANAGER_WEB:
      web_file_browser_module_begin();
      break;
    case MENU_SETTINGS_DISPLAY:
      display_config_module_begin();
      break;
    case MENU_SETTINGS_TIME_ZONE:
      if (menu_screens_is_configuration(user_selection)) {
        gps_module_set_time_zone(selected_item);
      }
      update_time_zone_options();
      break;
    case MENU_SETTINGS_WIFI:
      config_module_begin(MENU_SETTINGS_WIFI);
      break;
    case MENU_SETTINGS_SD_CARD_INFO:
      update_sd_card_info();
      break;
    case MENU_SETTINGS_SD_CARD_FORMAT:
      sd_card_settings_verify_sd_card();
      menus_module_set_app_state(true, sd_card_settings_keyboard_cb);
      break;
    case MENU_STEALTH_MODE:
      stealth_mode_open_menu();
      break;
    default:
      break;
  }
}

void settings_module_begin() {
  ESP_LOGI(TAG, "Settings module begin");
  menu_screens_register_exit_submenu_cb(settings_module_exit_submenu_cb);
  menu_screens_register_enter_submenu_cb(settings_module_enter_submenu_cb);
}

void settings_module_exit() {
  menu_screens_unregister_submenu_cbs();
}
