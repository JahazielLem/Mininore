#include <string.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "gps_module.h"
#include "menu_screens_modules.h"
#include "sd_card.h"
#include "wardriving_module.h"
#include "wardriving_screens_module.h"
#include "wifi_controller.h"
#include "wifi_scanner.h"

#define FILE_NAME      "test.csv"
#define FORMAT_VERSION "WigleWifi-1.6"
#define APP_VERSION    CONFIG_PROJECT_VERSION
#define MODEL          "MININO"
#define RELEASE        APP_VERSION
#define DEVICE         "MININO"
#define DISPLAY        "SH1106 OLED"
#define BOARD          "ESP32C6"
#define BRAND          "Electronic Cats"
#define STAR           "Sol"
#define BODY           "3"
#define SUB_BODY       "0"

#define MAC_ADDRESS_FORMAT       "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAX_CSV_LINES            100
#define CSV_LINE_SIZE            200  // Got it from real time tests
#define CSV_FILE_SIZE            CSV_LINE_SIZE* MAX_CSV_LINES
#define DISPLAY_REFRESH_RATE_SEC 5
#define WRITE_FILE_REFRESH_RATE  5

const char* TAG = "wardriving";
TaskHandle_t wardriving_module_scan_task_handle = NULL;
uint16_t csv_lines;
uint16_t wifi_scanned_packets;
char* csv_file = NULL;

const char* csv_header = FORMAT_VERSION
    ",appRelease=" APP_VERSION ",model=" MODEL ",release=" RELEASE
    ",device=" DEVICE ",display=" DISPLAY ",board=" BOARD ",brand=" BRAND
    ",star=" STAR ",body=" BODY ",subBody=" SUB_BODY
    "\n"
    "MAC,SSID,AuthMode,FirstSeen,Channel,Frequency,RSSI,CurrentLatitude,"
    "CurrentLongitude,AltitudeMeters,AccuracyMeters,RCOIs,MfgrId,Type";

static void print_auth_mode(int authmode) {
  switch (authmode) {
    case WIFI_AUTH_OPEN:
      ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_OPEN");
      break;
    case WIFI_AUTH_OWE:
      ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_OWE");
      break;
    case WIFI_AUTH_WEP:
      ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WEP");
      break;
    case WIFI_AUTH_WPA_PSK:
      ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_PSK");
      break;
    case WIFI_AUTH_WPA2_PSK:
      ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_PSK");
      break;
    case WIFI_AUTH_WPA_WPA2_PSK:
      ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_WPA2_PSK");
      break;
    case WIFI_AUTH_ENTERPRISE:
      ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_ENTERPRISE");
      break;
    case WIFI_AUTH_WPA3_PSK:
      ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA3_PSK");
      break;
    case WIFI_AUTH_WPA2_WPA3_PSK:
      ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_WPA3_PSK");
      break;
    case WIFI_AUTH_WPA3_ENT_192:
      ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA3_ENT_192");
      break;
    default:
      ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_UNKNOWN");
      break;
  }
}

static void print_cipher_type(int pairwise_cipher, int group_cipher) {
  switch (pairwise_cipher) {
    case WIFI_CIPHER_TYPE_NONE:
      ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_NONE");
      break;
    case WIFI_CIPHER_TYPE_WEP40:
      ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP40");
      break;
    case WIFI_CIPHER_TYPE_WEP104:
      ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP104");
      break;
    case WIFI_CIPHER_TYPE_TKIP:
      ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP");
      break;
    case WIFI_CIPHER_TYPE_CCMP:
      ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_CCMP");
      break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
      ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
      break;
    case WIFI_CIPHER_TYPE_AES_CMAC128:
      ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_AES_CMAC128");
      break;
    case WIFI_CIPHER_TYPE_SMS4:
      ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_SMS4");
      break;
    case WIFI_CIPHER_TYPE_GCMP:
      ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP");
      break;
    case WIFI_CIPHER_TYPE_GCMP256:
      ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP256");
      break;
    default:
      ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
      break;
  }

  switch (group_cipher) {
    case WIFI_CIPHER_TYPE_NONE:
      ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_NONE");
      break;
    case WIFI_CIPHER_TYPE_WEP40:
      ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP40");
      break;
    case WIFI_CIPHER_TYPE_WEP104:
      ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP104");
      break;
    case WIFI_CIPHER_TYPE_TKIP:
      ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP");
      break;
    case WIFI_CIPHER_TYPE_CCMP:
      ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_CCMP");
      break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
      ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
      break;
    case WIFI_CIPHER_TYPE_SMS4:
      ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_SMS4");
      break;
    case WIFI_CIPHER_TYPE_GCMP:
      ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP");
      break;
    case WIFI_CIPHER_TYPE_GCMP256:
      ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP256");
      break;
    default:
      ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
      break;
  }
}

char* get_mac_address(uint8_t* mac) {
  char* mac_address = malloc(18);
  sprintf(mac_address, MAC_ADDRESS_FORMAT, mac[0], mac[1], mac[2], mac[3],
          mac[4], mac[5]);
  return mac_address;
}

char* get_auth_mode(int authmode) {
  switch (authmode) {
    case WIFI_AUTH_OPEN:
      return "OPEN";
    case WIFI_AUTH_WEP:
      return "WEP";
    case WIFI_AUTH_WPA_PSK:
      return "WPA_PSK";
    case WIFI_AUTH_WPA2_PSK:
      return "WPA2_PSK";
    case WIFI_AUTH_WPA_WPA2_PSK:
      return "WPA_WPA2_PSK";
    case WIFI_AUTH_WPA2_ENTERPRISE:
      return "WPA2_ENTERPRISE";
    case WIFI_AUTH_WPA3_PSK:
      return "WPA3_PSK";
    case WIFI_AUTH_WPA2_WPA3_PSK:
      return "WPA2_WPA3_PSK";
    case WIFI_AUTH_WAPI_PSK:
      return "WAPI_PSK";
    case WIFI_AUTH_OWE:
      return "OWE";
    case WIFI_AUTH_WPA3_ENT_192:
      return "WPA3_ENT_SUITE_B_192_BIT";
    case WIFI_AUTH_DPP:
      return "DPP";
    default:
      return "Uncategorized";
  }
}

char* get_full_date_time(gps_t* gps) {
  char* date_time = malloc(sizeof(char) * 30);
  sprintf(date_time, "%d-%d-%d %d:%d:%d", gps->date.year, gps->date.month,
          gps->date.day, gps->tim.hour, gps->tim.minute, gps->tim.second);
  return date_time;
}

uint16_t get_frequency(uint8_t primary) {
  return 2412 + 5 * (primary - 1);
}

void wardriving_module_scan_task(void* pvParameters) {
  wifi_driver_init_sta();

  while (true) {
    wifi_scanner_module_scan();
    // wardriving_screens_module_scanning(wifi_scanned_packets,
    //                                    gps_module_get_signal_strength(gps));
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

/**
 * @brief Callback function for GPS events
 *
 * @param event_data
 *
 * @note This function is called every time a GPS event is triggered, its
 * ussually every second.
 */
void wardriving_gps_event_handler_cb(gps_t* gps) {
  static uint32_t counter = 0;
  counter++;

  ESP_LOGI(TAG,
           "Satellites in use: %d, signal: %s \r\n"
           "\t\t\t\t\t\tlatitude   = %.05f°N\r\n"
           "\t\t\t\t\t\tlongitude = %.05f°E\r\n",
           gps->sats_in_use, gps_module_get_signal_strength(gps), gps->latitude,
           gps->longitude);

  if (counter % DISPLAY_REFRESH_RATE_SEC == 0 || counter == 1) {
    wardriving_screens_module_scanning(wifi_scanned_packets,
                                       gps_module_get_signal_strength(gps));
  }

  uint64_t start_time, end_time;
  start_time = esp_timer_get_time();

  if (gps->sats_in_use == 0) {
    return;
  }

  if (counter % WRITE_FILE_REFRESH_RATE != 0) {
    return;
  }

  wifi_scanner_ap_records_t* ap_records = wifi_scanner_get_ap_records();
  char* csv_line = malloc(CSV_LINE_SIZE);

  // Append records to csv file buffer
  for (int i = 0; i < ap_records->count; i++) {
    csv_lines++;
    wifi_scanned_packets++;
    char* mac_address_str = get_mac_address(ap_records->records[i].bssid);
    char* auth_mode_str = get_auth_mode(ap_records->records[i].authmode);
    char* full_date_time = get_full_date_time(gps);

    sprintf(csv_line, "%s,%s,%s,%s,%d,%u,%d,%f,%f,%f,%f,%s,%s,%s\n",
            /* MAC */
            mac_address_str,
            /* SSID */
            ap_records->records[i].ssid,
            /* AuthMode */
            auth_mode_str,
            /* FirstSeen */
            full_date_time,
            /* Channel */
            ap_records->records[i].primary,
            /* Frequency */
            get_frequency(ap_records->records[i].primary),
            /* RSSI */
            ap_records->records[i].rssi,
            /* CurrentLatitude */
            gps->latitude,
            /* CurrentLongitude */
            gps->longitude,
            /* AltitudeMeters */
            gps->altitude,
            /* AccuracyMeters */
            GPS_ACCURACY,
            /* RCOIs */
            "",
            /* MfgrId */
            "",
            /* Type */
            "WIFI");

    free(mac_address_str);
    free(full_date_time);

    // ESP_LOGI(TAG, "CSV Line: %s", csv_line);
    // ESP_LOGI(TAG, "Line size %d bytes", strlen(csv_line));
    strcat(csv_file, csv_line);
  }
  ESP_LOGW(TAG, "File size %d bytes, lines: %u", strlen(csv_file), csv_lines);
  sd_card_write_file(FILE_NAME, csv_file);

  end_time = esp_timer_get_time();
  float time = (float) (end_time - start_time) / 1000000;
  char* time_str = malloc(sizeof(time) + 1);
  sprintf(time_str, "%2.2f", time);
  ESP_LOGI(TAG, "Total time taken: %s seconds", time_str);
}

void wardriving_module_start_scan() {
#if !defined(CONFIG_WARDRIVING_MODULE_DEBUG)
  esp_log_level_set(TAG, ESP_LOG_NONE);
#endif
  ESP_LOGI(TAG, "Start scan");
  csv_lines = 2;  // Two header lines
  wifi_scanned_packets = 0;
  csv_file = malloc(CSV_FILE_SIZE);
  sprintf(csv_file, "%s\n", csv_header);  // Append header to csv file

  sd_card_mount();
  xTaskCreate(wardriving_module_scan_task, "wardriving_module_scan_task", 4096,
              NULL, 5, &wardriving_module_scan_task_handle);
  gps_module_register_cb(wardriving_gps_event_handler_cb);
  wardriving_screens_module_loading_text();
  gps_module_start_scan();
}

void wardriving_module_stop_scan() {
  ESP_LOGI(TAG, "Stop scan");
  gps_module_stop_read();
  gps_module_unregister_cb();
  wifi_driver_deinit();
  sd_card_read_file(FILE_NAME);
  sd_card_unmount();
  if (wardriving_module_scan_task_handle != NULL) {
    vTaskDelete(wardriving_module_scan_task_handle);
    wardriving_module_scan_task_handle = NULL;
    ESP_LOGI(TAG, "Task deleted");
  } else {
    ESP_LOGW(TAG, "Task deleted?");
  }
}
