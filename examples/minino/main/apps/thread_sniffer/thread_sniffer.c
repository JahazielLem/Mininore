#include "thread_sniffer.h"

#include <stdbool.h>
#include <stdio.h>
#include "esp_check.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "pcap.h"
#include "sd_card.h"

#define THREAD_SNIFFER_PCAP_LINKTYPE 230

#define PCAP_FILE_NAME_MAX_LEN  32
#define PCAP_MEMORY_BUFFER_SIZE 4096

#define THREAD_SNIFFER_QUEUE_SIZE                32
#define THREAD_SNIFFER_PROCESS_PACKET_TIMEOUT_MS 100

#define TAG "thread_sniffer"

typedef struct {
  char* buffer;
  uint32_t buffer_size;
} pcap_memory_buffer_t;

typedef struct {
  bool is_opened;
  bool is_writing;
  bool link_type_set;
  char filename[PCAP_FILE_NAME_MAX_LEN];
  pcap_file_handle_t pcap_handle;
  pcap_link_type_t link_type;
  pcap_memory_buffer_t pcap_buffer;
} thread_pcap_handler_t;

typedef struct {
  void* payload;
  uint32_t length;
  uint32_t seconds;
  uint32_t microseconds;
} sniffer_packet_info_t;

thread_pcap_handler_t thread_pcap = {0};
thread_sniffer_show_event_cb_t thread_sniffer_show_event_cb = NULL;
static QueueHandle_t packet_rx_queue = NULL;
static uint32_t packets_count = 0;

static esp_err_t pcap_start();
static esp_err_t pcap_stop();
static esp_err_t pcap_capture(void* payload,
                              uint32_t length,
                              uint32_t seconds,
                              uint32_t microseconds);

void on_pcap_receive(const otRadioFrame* aFrame, bool aIsTx);
static void thread_sniffer_show_event(thread_sniffer_events_t event,
                                      void* context);
static void debug_handler_task();

void thread_sniffer_init() {
  openthread_init();
  esp_log_level_set("OPENTHREAD", ESP_LOG_NONE);
  packet_rx_queue =
      xQueueCreate(THREAD_SNIFFER_QUEUE_SIZE, sizeof(sniffer_packet_info_t));
  xTaskCreate(debug_handler_task, "debug_handler_task", 8192, NULL, 20, NULL);
}
void thread_sniffer_run() {
  pcap_start();
  printf("START SESSION\n");
  packets_count = 0;
  thread_sniffer_show_event(THREAD_SNIFFER_START_EV, NULL);
  thread_sniffer_show_event_cb(THREAD_SNIFFER_NEW_PACKET_EV, packets_count);
  openthread_enable_promiscous_mode(&on_pcap_receive);
}
void thread_sniffer_stop() {
  printf("STOP SESSION\n");
  pcap_stop();
  thread_sniffer_show_event(THREAD_SNIFFER_STOP_EV, NULL);
  openthread_disable_promiscous_mode();
}

static esp_err_t pcap_start() {
  esp_err_t ret = ESP_OK;
  FILE* fp = NULL;
  bool save_in_sd = false;
  if (sd_card_mount() == ESP_OK) {
    fp = fopen("/sdcard/thread.pcap", "w");
    save_in_sd = true;
  } else {
    thread_pcap.pcap_buffer.buffer =
        calloc(PCAP_MEMORY_BUFFER_SIZE, sizeof(char));
    ESP_GOTO_ON_FALSE(thread_pcap.pcap_buffer.buffer, ESP_ERR_NO_MEM, err, TAG,
                      "pcap buffer calloc failed");
    fp = fmemopen(thread_pcap.pcap_buffer.buffer, PCAP_MEMORY_BUFFER_SIZE,
                  "wb+");
  }
  ESP_GOTO_ON_FALSE(fp, ESP_FAIL, err, TAG, "open file failed");
  pcap_config_t pcap_cfg = {
      .fp = fp,
      .major_version = PCAP_DEFAULT_VERSION_MAJOR,
      .minor_version = PCAP_DEFAULT_VERSION_MINOR,
      .time_zone = PCAP_DEFAULT_TIME_ZONE_GMT,
  };
  ESP_GOTO_ON_ERROR(pcap_new_session(&pcap_cfg, &thread_pcap.pcap_handle), err,
                    TAG, "pcap init failed");
  thread_pcap.is_opened = true;
  ESP_GOTO_ON_ERROR(
      pcap_write_header(thread_pcap.pcap_handle, THREAD_SNIFFER_PCAP_LINKTYPE),
      err, TAG, "Write header failed");
  thread_pcap.is_writing = true;
  ESP_LOGI(TAG, "open file successfully");

  thread_sniffer_show_event(THREAD_SNIFFER_DESTINATION_EV, &save_in_sd);
  return ret;

err:
  if (fp) {
    fclose(fp);
  }
  thread_pcap.is_opened = false;
  // show error
  return ret;
}

static esp_err_t pcap_stop() {
  esp_err_t ret = ESP_OK;
  ESP_GOTO_ON_FALSE(thread_pcap.is_opened, ESP_ERR_INVALID_STATE, err, TAG,
                    ".pcap file is already closed");
  ESP_GOTO_ON_ERROR(pcap_del_session(thread_pcap.pcap_handle), err, TAG,
                    "stop pcap session failed");
  thread_pcap.is_opened = false;
  thread_pcap.is_writing = false;
  thread_pcap.link_type_set = false;
  thread_pcap.pcap_handle = NULL;
err:
  // show err
  return ret;
}

void on_pcap_receive(const otRadioFrame* aFrame, bool aIsTx) {
  BaseType_t task;
  xQueueSendToBackFromISR(packet_rx_queue, aFrame, &task);
}

static esp_err_t pcap_capture(void* payload,
                              uint32_t length,
                              uint32_t seconds,
                              uint32_t microseconds) {
  if (pcap_capture_packet(thread_pcap.pcap_handle, payload, length, seconds,
                          microseconds) != ESP_OK) {
    printf("PCAP CAPTURE FAILED\n");
    return ESP_FAIL;
  }
  // show packet
  return ESP_OK;
}

static void debug_handler_task() {
  otRadioFrame packet;
  while (xQueueReceive(packet_rx_queue, &packet, portMAX_DELAY) != pdFALSE) {
    packets_count++;
    thread_sniffer_show_event_cb(THREAD_SNIFFER_NEW_PACKET_EV, packets_count);
    pcap_capture(packet.mPsdu, packet.mLength,
                 packet.mInfo.mRxInfo.mTimestamp / 1000000u,
                 packet.mInfo.mRxInfo.mTimestamp % 1000000u);
  }
  ESP_LOGE("debug_handler_task", "Terminated");
  vTaskDelete(NULL);
}

void thread_sniffer_set_show_event_cb(thread_sniffer_show_event_cb_t cb) {
  thread_sniffer_show_event_cb = cb;
}

static void thread_sniffer_show_event(thread_sniffer_events_t event,
                                      void* context) {
  if (thread_sniffer_show_event_cb != NULL) {
    thread_sniffer_show_event_cb(event, context);
  }
}
