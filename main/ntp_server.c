#include "ntp_server.h"
#include "ntp_server.h"
#include "ntp_packet.h"
#include "time_sync.h"
#include "wifi_manager.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include <string.h>
#include <sys/time.h>

static const char *TAG = "ntp_server";
static uint32_t s_packets_served = 0;

static void ntp_server_task(void *pvParameters) {
    while (!wifi_manager_is_connected()) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
        ESP_LOGE(TAG, "Failed to create socket: %d", sock);
        vTaskDelete(NULL);
    }

    struct timeval timeout = {1, 0}; // 1000ms timeout
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(NTP_PORT)
    };

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        ESP_LOGE(TAG, "Failed to bind socket: %d", errno);
        close(sock);
        vTaskDelete(NULL);
    }

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    ntp_packet_t req, resp;

    while (1) {
        int len = recvfrom(sock, &req, sizeof(req), 0, (struct sockaddr *)&client_addr, &addr_len);
        if (len < (int)sizeof(ntp_packet_t)) {
            continue;
        }

        struct timeval now;
        gettimeofday(&now, NULL);
        ntp_time_t now_ntp = timeval_to_ntp(&now);

        gnss_status_t gnss_stat;
        bool locked = time_sync_get_status(&gnss_stat);

        memset(&resp, 0, sizeof(resp));

#define NTP_REF_ID_PPS 0x50505300ULL /* "PPS\0" */

        uint8_t li = locked ? 0 : 3;
        resp.li_vn_mode = (li << 6) | (4 << 3) | 4;
        resp.stratum = locked ? 1 : 16;
        resp.poll = req.poll;
        resp.precision = (locked && gnss_stat.pps_synced) ? -20 : -16;
        resp.root_delay = 0; // Stratum 1 primary server has 0 root delay
        resp.root_dispersion = locked ? (gnss_stat.pps_synced ? htonl(0x00000010) : htonl(0x00000100)) : htonl(0x00010000);
        if (locked) {
            resp.ref_id = gnss_stat.pps_synced ? htonl((uint32_t)NTP_REF_ID_PPS) : htonl((uint32_t)NTP_REF_ID_GPS);
        } else {
            resp.ref_id = htonl((uint32_t)NTP_REF_ID_INIT);
        }
        resp.ref_time = now_ntp;
        resp.origin_time = req.transmit_time;
        resp.receive_time = now_ntp;
        resp.transmit_time = now_ntp;

        sendto(sock, &resp, sizeof(resp), 0, (struct sockaddr *)&client_addr, addr_len);
        s_packets_served++;
    }

    close(sock);
    vTaskDelete(NULL);
}

esp_err_t ntp_server_start(void) {
    TaskHandle_t task_handle;
    BaseType_t ret = xTaskCreatePinnedToCore(ntp_server_task, "ntp_server", 4096, NULL, 15, &task_handle, 0);
    return ret == pdPASS ? ESP_OK : ESP_FAIL;
}

uint32_t ntp_server_get_packet_count(void) {
    return s_packets_served;
}
