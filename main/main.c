#include <stdio.h>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

int pwmChannel = 0; //Choisit le canal 0
int frequence = 1000; //Fréquence PWM de 1 KHz
int resolution = 8; // Résolution de 8 bits, 256 valeurs possibles
int pwmPin = 23;

void app_main(void)
{

    ledc_timer_config_t pwm_config = {
        .duty_resolution = resolution, // Résolution de 8 bits, 256 valeurs possibles
        .freq_hz = frequence, //Fréquence PWM de 1 KHz
        .speed_mode = LEDC_LOW_SPEED_MODE, //Mode de vitesse faible
        .timer_num = LEDC_TIMER_0 //Timer 0
        
    };

    ledc_channel_config_t pwm_channel = {
        .channel = pwmChannel, //Choisit le canal 0
        .duty = 50, //Duty cycle de départ à 0
        .gpio_num = pwmPin, //GPIO 23
        .intr_type = LEDC_INTR_DISABLE, //Désactiver l'interruption
        .speed_mode = LEDC_LOW_SPEED_MODE, //Mode de vitesse faible
        .timer_sel = LEDC_TIMER_0 //Timer 0
    };

    printf("Debug_V13\n"); // Print "Debug_V13" to the console   
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_channel)); //Configure le canal PWM
    ESP_ERROR_CHECK(ledc_timer_config(&pwm_config)); //Configure le timer PWM



    ESP_ERROR_CHECK(ledc_fade_func_install(0)); //Installe la fonction de fondu
}  
