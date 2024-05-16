#include "sonneries.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "esp_err.h"

void ring(ledc_timer_config_t pwm_config, ledc_channel_config_t pwm_channel)
{
    int pwmChannel = 0;  // Choisit le canal 0
    int frequence = 440; // Fréquence PWM de 1 KHz
    int resolution = 8;  // Résolution de 8 bits, 256 valeurs possibles
    int pwmPin = 23;
    while (true)
    {
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, pwmChannel));                // Démarre le PWM à 0
        ESP_ERROR_CHECK(ledc_set_freq(pwm_channel.speed_mode, pwm_config.timer_num, 440)); // Fréquence à 440 Hz
        vTaskDelay(400 / portTICK_PERIOD_MS);                                              // Attend 1 seconde
        ESP_ERROR_CHECK(ledc_stop(LEDC_LOW_SPEED_MODE, pwmChannel, 0));                    // Arrête le PWM (0 = Ne pas attendre la fin du cycle actuel)
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, pwmChannel));                // Démarre le PWM à 0
        ESP_ERROR_CHECK(ledc_set_freq(pwm_channel.speed_mode, pwm_config.timer_num, 550)); // Fréquence à 550 Hz
        vTaskDelay(100 / portTICK_PERIOD_MS);                                              // Attend 1 seconde
        ESP_ERROR_CHECK(ledc_stop(LEDC_LOW_SPEED_MODE, pwmChannel, 0));                    // Arrête le PWM (0 = Ne pas attendre la fin du cycle actuel)
    }
}