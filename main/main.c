#include <stdio.h>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "driver/rmt_rx.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_types_legacy.h"
#include "driver/rmt.h"
#include "sonneries.h"
#include "driver/timer.h"
#include <string.h>
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_wifi.h"
#include "esp_now.h"

#define Val_Trame 128
#define EXAMPLE_ESP_WIFI_SSID "MonESP32"
#define EXAMPLE_ESP_WIFI_PASS "motDePasse"
#define EXAMPLE_MAX_STA_CONN 4

const char *ssid = "Xiaomi_12T";
const char *pass = "jesaispas";

static const char *TAG = "wifi softAP";

const char *serverIP = "192.168.4.1";
const uint16_t serverPort = 80;

typedef enum
{
    INIT,
    ATT_START,
    START,
    ATT_END,
    END,
} Tetat;

Tetat m_etat_courant;
Tetat m_etat_old;
bool m_bool_data_ir = false; // Initialisation correcte
int l_int_rising_count = 0;
uint64_t tickCount1 = 0, tickCount2 = 0;

int l_int_i = 0;
int pwmChannel = 0;
int frequence = 440;
int resolution = 8;
int pwmPin = 23;

int l_int_tab_values[Val_Trame] = {0}; // Initialisation correcte
int l_int_tab_test_Start[46] = {1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0};
int l_int_tab_test_Play[36] = {1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0};
int l_int_tab_test_Stop[35] = {1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0};

bool m_bool_ENDTRAME = false;
int retry_num = 0;
int l_int_cptStart = 0;
int l_int_cptPlay = 0;
int l_int_cptStop = 0;

void Init()
{
    m_etat_courant = INIT;
    m_etat_old = m_etat_courant;
}

void LireEntree()
{
    m_bool_data_ir = gpio_get_level(GPIO_NUM_3);
}

void Evoluer()
{
    switch (m_etat_courant)
    {
    case INIT:
        m_etat_courant = ATT_START;
        break;
    case ATT_START:
        if (m_bool_data_ir == 0)
        {
            m_etat_courant = START;
        }
        break;
    case START:
        if (m_bool_data_ir == 1)
        {
            m_etat_courant = ATT_END;
        }
        break;
    case ATT_END:
        if ((m_bool_data_ir == 1) && (m_bool_ENDTRAME == 1))
        {
            m_etat_courant = END;
        }
        break;
    case END:
        m_etat_courant = ATT_START;
        break;
    default:
        break;
    }
}

void MiseAJour()
{
    if (m_etat_courant != m_etat_old)
    {
        m_etat_old = m_etat_courant;
    }
    switch (m_etat_courant)
    {
    case INIT:
        timer_start(TIMER_GROUP_0, TIMER_0);
        break;
    case ATT_START:
        l_int_cptStart = 0;
        l_int_cptPlay = 0;
        l_int_cptStop = 0;
        for (size_t i = 0; i < Val_Trame; i++)
        {
            l_int_tab_values[i] = 0;
        }
        break;
    case ATT_END:
        timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &tickCount1);
        do
        {
            timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &tickCount2);
        } while ((tickCount2 - tickCount1) < 125);
        for (size_t i = 0; i < Val_Trame; i++)
        {
            timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &tickCount1);
            l_int_tab_values[i] = gpio_get_level(GPIO_NUM_3);
            do
            {
                timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &tickCount2);
            } while ((tickCount2 - tickCount1) < 500);
            l_int_rising_count++;
        }
        timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &tickCount1);
        do
        {
            timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &tickCount2);
        } while ((tickCount2 - tickCount1) < 10000);

        for (int i = 0; i < Val_Trame; i++)
        {

            printf("%i", l_int_tab_values[i]);
        }

        printf("\n");

        m_bool_ENDTRAME = true;
        break;
    case END:
        for (int ibcl = 0; ibcl < 35; ibcl++)
        {
            if (l_int_tab_values[ibcl] == l_int_tab_test_Start[ibcl])
            {
                l_int_cptStart++;
            }
            if (l_int_tab_values[ibcl] == l_int_tab_test_Play[ibcl])
            {
                l_int_cptPlay++;
            }
            if (l_int_tab_test_Stop[ibcl] == l_int_tab_values[ibcl])
            {

                l_int_cptStop++;
            }
        }
        if (l_int_cptStart == 35)
        {
            printf("Start\n");
        }
        else if (l_int_cptPlay == 35)
        {
            printf("Play\n");
        }
        else if (l_int_cptStop == 35)
        {

            printf("Stop \n");
        }
        break;
    default:
        break;
    }
}

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_id == WIFI_EVENT_STA_START)
    {
        printf("WIFI CONNECTING....\n");
    }
    else if (event_id == WIFI_EVENT_STA_CONNECTED)
    {
        printf("WiFi CONNECTED\n");
    }
    else if (event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        printf("WiFi lost connection\n");
        if (retry_num < 5)
        {
            esp_wifi_connect();
            retry_num++;
            printf("Retrying to Connect...\n");
        }
    }
    else if (event_id == IP_EVENT_STA_GOT_IP)
    {
        printf("Wifi got IP...\n\n");
    }
}

void wifi_connection()
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = "",
            .password = "",
        }};
    strcpy((char *)wifi_configuration.sta.ssid, ssid);
    strcpy((char *)wifi_configuration.sta.password, pass);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    esp_wifi_start();
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_connect();
    printf("wifi_init_softap finished. SSID:%s  password:%s", ssid, pass);
}

void wifiAP()
{
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "ESP32",
            .ssid_len = 0,
        }};
    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);
    esp_wifi_start();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("wifi_init_softap finished. SSID:%s password:%s", ssid, pass);
}

void app_main()
{
    gpio_config_t gpio_configuration = {
        .pin_bit_mask = (1 << 3),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE};
    gpio_config(&gpio_configuration);

    timer_config_t timer_config = {
        .alarm_en = TIMER_ALARM_EN,
        .counter_en = TIMER_START,
        .intr_type = TIMER_INTR_LEVEL,
        .counter_dir = TIMER_COUNT_UP,
        .auto_reload = TIMER_AUTORELOAD_EN,
        .divider = 80};
    timer_init(TIMER_GROUP_0, TIMER_0, &timer_config);

    ledc_channel_config_t pwm_channel = {
        .channel = pwmChannel,
        .duty = 50,
        .gpio_num = pwmPin,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel = LEDC_TIMER_0};

    ledc_timer_config_t pwm_config = {
        .duty_resolution = resolution,
        .freq_hz = frequence,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0};
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_channel));
    ESP_ERROR_CHECK(ledc_timer_config(&pwm_config));
    ESP_ERROR_CHECK(ledc_stop(LEDC_LOW_SPEED_MODE, pwmChannel, 0));
    /*
        // WIFI

        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }
        ESP_ERROR_CHECK(ret);

        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        esp_netif_create_default_wifi_ap();

        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

        ESP_ERROR_CHECK(esp_wifi_init(&cfg));
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

        wifi_config_t wifi_config = {
            .ap = {
                .ssid = EXAMPLE_ESP_WIFI_SSID,
                .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
                .password = EXAMPLE_ESP_WIFI_PASS,
                .max_connection = EXAMPLE_MAX_STA_CONN,
                .authmode = WIFI_AUTH_WPA_WPA2_PSK},
        };

        if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0)
        {
            wifi_config.ap.authmode = WIFI_AUTH_OPEN;
        }

        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());

        ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    */
    Init();

    // Afficher l'adresse IP de l'ESP32

    while (1)
    {
        LireEntree();
        Evoluer();
        MiseAJour();
        ring(pwm_config, pwm_channel);

        if (l_int_cptPlay == 35)
        {
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, pwmChannel));                // Démarre le PWM à 0
            ESP_ERROR_CHECK(ledc_set_freq(pwm_channel.speed_mode, pwm_config.timer_num, 500)); // Définit la fréquence de la note
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
        if (l_int_cptStart == 35)
        {
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, pwmChannel));                 // Démarre le PWM à 0
            ESP_ERROR_CHECK(ledc_set_freq(pwm_channel.speed_mode, pwm_config.timer_num, 5000)); // Définit la fréquence de la note
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
        if (l_int_cptStop == 35)
        {
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, pwmChannel));                  // Démarre le PWM à 0
            ESP_ERROR_CHECK(ledc_set_freq(pwm_channel.speed_mode, pwm_config.timer_num, 15000)); // Définit la fréquence de la note
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
    }
}