#include "gps_screens.h"

#include "menus_module.h"
#include "oled_screen.h"

static void gps_screens_update_date_and_time(gps_t* gps) {
  char* str = (char*) malloc(20);
  sprintf(str, "Signal: %s  ", gps_module_get_signal_strength(gps));
  oled_screen_display_text(str, 0, 0, OLED_DISPLAY_NORMAL);
  sprintf(str, "Date: %d/%d/%d", gps->date.year, gps->date.month,
          gps->date.day);
  oled_screen_display_text(str, 0, 2, OLED_DISPLAY_NORMAL);
  sprintf(str, "Time: %d:%d:%d", gps->tim.hour, gps->tim.minute,
          gps->tim.second);
  oled_screen_display_text(str, 0, 3, OLED_DISPLAY_NORMAL);
  free(str);
}
static void gps_screens_update_location(gps_t* gps) {
  char* str = (char*) malloc(20);
  sprintf(str, "Signal: %s  ", gps_module_get_signal_strength(gps));
  oled_screen_display_text(str, 0, 0, OLED_DISPLAY_NORMAL);
  oled_screen_display_text("Latitude:", 0, 2, OLED_DISPLAY_NORMAL);
  sprintf(str, "  %.05f N", gps->latitude);
  oled_screen_display_text(str, 0, 3, OLED_DISPLAY_NORMAL);
  oled_screen_display_text("Longitude:", 0, 4, OLED_DISPLAY_NORMAL);
  sprintf(str, "  %.05f E", gps->longitude);
  oled_screen_display_text(str, 0, 5, OLED_DISPLAY_NORMAL);
  oled_screen_display_text("Altitude:", 0, 6, OLED_DISPLAY_NORMAL);
  sprintf(str, "  %.04fm", gps->altitude);
  oled_screen_display_text(str, 0, 7, OLED_DISPLAY_NORMAL);
  free(str);
}
static void gps_screens_update_speed(gps_t* gps) {
  char* str = (char*) malloc(20);
  sprintf(str, "Signal: %s  ", gps_module_get_signal_strength(gps));
  oled_screen_display_text(str, 0, 0, OLED_DISPLAY_NORMAL);
  sprintf(str, "Speed: %.02fm/s", gps->speed);
  oled_screen_display_text(str, 0, 2, OLED_DISPLAY_NORMAL);
  free(str);
}

void gps_screens_show_waiting_signal() {
  oled_screen_display_text_center("Waiting Signal", 0, OLED_DISPLAY_NORMAL);
}

void gps_screens_update_handler(gps_t* gps) {
  menu_idx_t current = menus_module_get_current_menu();
  switch (current) {
    case MENU_GPS_DATE_TIME_2:
      gps_screens_update_date_and_time(gps);
      break;
    case MENU_GPS_LOCATION_2:
      gps_screens_update_location(gps);
      break;
    case MENU_GPS_SPEED_2:
      gps_screens_update_speed(gps);
      break;
    default:
      return;
      break;
  }
}