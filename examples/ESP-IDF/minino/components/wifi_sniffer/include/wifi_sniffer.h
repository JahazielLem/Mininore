#pragma once

#include "cmd_sniffer.h"

#define SIMPLE_SNIFFER_H
#define CHANNEL_MAX 13  // US = 11, EU = 13, JP = 14

void wifi_sniffer_begin();
void wifi_sniffer_start();
void wifi_sniffer_stop();
void wifi_sniffer_exit();
