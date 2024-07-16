#include <stdio.h>
#include "cat_console.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "keyboard_module.h"
#include "leds.h"
#include "menu_screens_modules.h"
#include "open_thread.h"
#include "preferences.h"
#include "sd_card.h"

static const char* TAG = "main";

void reboot_counter() {
  int32_t counter = preferences_get_int("reboot_counter", 0);
  ESP_LOGI(TAG, "Reboot counter: %ld", counter);
  counter++;
  preferences_put_int("reboot_counter", counter);
}

void app_main(void) {
  uint64_t start_time, end_time;
  start_time = esp_timer_get_time();

  buzzer_begin(GPIO_NUM_2);
  leds_init();
  preferences_begin();
  sd_card_init();
  keyboard_module_begin();
  menu_screens_begin();
  reboot_counter();
  leds_off();

  end_time = esp_timer_get_time();
  float time = (float) (end_time - start_time) / 1000000;
  char* time_str = malloc(sizeof(time) + 1);
  sprintf(time_str, "%2.2f", time);
  ESP_LOGI(TAG, "Total time taken: %s seconds", time_str);
  cat_console_begin();
}
