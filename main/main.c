#include <stdio.h>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include <driver/rmt_rx.h>
#include <driver/rmt_tx.h>
#include <driver/rmt_types_legacy.h>
#include "driver/rmt.h"
#include "sonneries.h"
#include "driver/gpio.h"
#include "driver/timer.h"



/*Include pour WIFI
#include "esp_wifi.h"  //fonctions esp_wifi_init et opérations wifi
#include <string.h>  //pour la gestion des chaînes
#include "esp_system.h "  //Fonctions esp_init esp_err_t
#include "esp_log.h"  //pour afficher les journaux
#include "esp_event.h"  //pour l'événement wifi
#include "nvs_flash.h "  //stockage non volatile
#include "lwip/err.h"  //gestion des erreurs des paquets IP légers
#include "lwip/sys.h"  //applications système pour les applications IP légères
*/

int pwmChannel = 0;  // Choisit le canal 0
int frequence = 440; // Fréquence PWM de 1 KHz
int resolution = 8;  // Résolution de 8 bits, 256 valeurs possibles
int pwmPin = 23;

void app_main(void)
{

    /*/**
     Configuration du timer PWM.
     *
     * Cette structure contient les paramètres de configuration du timer PWM.
     *
     * - duty_resolution : La résolution du signal PWM en bits. Il y a 256 valeurs possibles pour une résolution de 8 bits.
     * - freq_hz : La fréquence du signal PWM en Hz. Dans ce cas, la fréquence est de 1 KHz.
     * - speed_mode : Le mode de vitesse du timer. Dans ce cas, il est configuré en mode de vitesse faible.
     * - timer_num : Le numéro du timer. Dans ce cas, il est configuré pour le timer 0.
     */
    ledc_timer_config_t pwm_config = {

        .duty_resolution = resolution,     // Résolution de 8 bits, 256 valeurs possibles
        .freq_hz = frequence,              // Fréquence PWM de 1 KHz
        .speed_mode = LEDC_LOW_SPEED_MODE, // Mode de vitesse faible
        .timer_num = LEDC_TIMER_0          // Timer 0

    };
    /**
     * Configuration du canal PWM.
     *
     * - channel: Choix du canal (0).
     * - duty: Cycle de service initial à 50%.
     * - gpio_num: GPIO 23.
     * - intr_type: Désactivation de l'interruption.
     * - speed_mode: Mode de vitesse faible.
     * - timer_sel: Timer 0.
     */

    ledc_channel_config_t pwm_channel = {
        .channel = pwmChannel,             // Choisit le canal 0
        .duty = 50,                        // Duty cycle de départ à 0
        .gpio_num = pwmPin,                // GPIO 23
        .intr_type = LEDC_INTR_DISABLE,    // Désactiver l'interruption
        .speed_mode = LEDC_LOW_SPEED_MODE, // Mode de vitesse faible
        .timer_sel = LEDC_TIMER_0          // Timer 0
    };

    // printf("Debug_V14\n"); // Print "Debug_V14" to the console
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_channel));             // Configure le canal PWM
    ESP_ERROR_CHECK(ledc_timer_config(&pwm_config));                // Configure le timer PWM
    ESP_ERROR_CHECK(ledc_stop(LEDC_LOW_SPEED_MODE, pwmChannel, 0)); // Arrête le PWM (0 = Ne pas attendre la fin du cycle actuel)
                                                                    /*
                                                                        rmt_channel_handle_t rx_chan = NULL; // Initialize rx_chan with an empty struct
                                                                        rmt_rx_channel_config_t rx_chan_config = {
                                                                
                                                                            // Configuration du module RMT.
                                                                            .clk_src = RMT_CLK_SRC_DEFAULT, // clk_src: Source d'horloge par défaut pour le module RMT.
                                                                            .resolution_hz = 1 * 1000*1000,    // resolution_hz: Résolution en hertz pour le module RMT.
                                                                            .mem_block_symbols = 64,        // mem_block_symbols: Nombre de symboles dans le bloc mémoire du module RMT.
                                                                            .gpio_num = GPIO_NUM_3,         // gpio_num: Numéro du GPIO utilisé pour le module RMT.
                                                                            .flags.invert_in = true,        // flags.invert_in: Indique si le signal d'entrée doit être inversé ou non.
                                                                            .flags.with_dma = false,        // flags.with_dma: Indique si le module RMT doit être utilisé avec le DMA ou non.
                                                                
                                                                        };
                                                                        rmt_receive_config_t rmt_receive_config = {
                                                                
                                                                            .signal_range_min_ns = 1250,    // signal_range_min_ns: Durée minimale d'un signal en nanosecondes.
                                                                            .signal_range_max_ns = 2500000, // signal_range_max_ns: Durée maximale d'un signal en nanosecondes.
                                                                
                                                                        };
                                                                
                                                                        esp_log_level_set("*", ESP_LOG_VERBOSE); // Set the log level for all components to VERBOSE
                                                                
                                                                        rmt_symbol_word_t raw_symbols[32];                              // Initialize raw_symbols with an empty struct
                                                                        ESP_ERROR_CHECK(rmt_new_rx_channel(&rx_chan_config, &rx_chan)); // Configure the RMT receiver channel
                                                                
                                                                        printf("rx_chan pointer value: %p\n", (void *)rx_chan);
                                                                
                                                                        ESP_ERROR_CHECK(rmt_enable(rx_chan));
                                                                            // Enable the RMT receiver channel interrupt
                                                                            rmt_rx_done_event_data_t rmt_rx_done_event_data;
                                                                            ESP_ERROR_CHECK(rmt_rx_register_event_callbacks(rx_chan, &rmt_rx_done_event_data, NULL)); // Register the RMT receiver channel interrupt callback function
                                                                        ESP_ERROR_CHECK(rmt_receive(rx_chan, raw_symbols, sizeof(raw_symbols), &rmt_receive_config)); // Start the RMT receiver channel
                                                                
                                                                        while (true)
                                                                        {
                                                                            // printf("Valeur : %d\n", rx_chan); // Print the GPIO number of the RMT receiver channel
                                                                            vTaskDelay(1000 / portTICK_PERIOD_MS); // Wait for 1 second
                                                                
                                                                
                                                                            rmt_item32_t *items = (rmt_item32_t *)raw_symbols;                           // Initialize items with the raw_symbols struct
                                                                            rmt_item32_t *item = items;                                                  // Initialize item with the items struct
                                                                            rmt_item32_t *item_end = items + sizeof(raw_symbols) / sizeof(rmt_item32_t); // Initialize item_end with the items struct
                                                                            for (int i = 0; i < sizeof(raw_symbols) / sizeof(rmt_item32_t); i++)         // Loop through the raw_symbols array
                                                                            {
                                                                                printf("%d", item->level0); // Print the level0 value of the current item
                                                                                item++;                     // Increment the item pointer
                                                                                                            //   int level = gpio_get_level(GPIO_NUM_3); // Lire le niveau de la broche GPIO3
                                                                                                            //  printf("%d", level); // Imprimer le niveau de la broche
                                                                            }
                                                                            ESP_ERROR_CHECK(rmt_disable(rx_chan)); // Disable the RMT receiver channel interrupt
                                                                        }*/

    // rmt_write_items(0, raw_symbols, sizeof(raw_symbols), true); // Write the raw_symbols array to the RMT receiver channel

    //  vTaskDelay(1000 / portTICK_PERIOD_MS); // Wait for 1 second
    //  printf("\n");                          // Print a newline character

    // printf("Debug_V14\n"); // Print "Debug_V14" to the console
    //   vTaskDelay(10000 / portTICK_PERIOD_MS); // Wait for 1 second
    // ring(pwm_config, pwm_channel); // Call the ring function with the pwm_config and pwm_channel structs as arguments

    /**
     * @brief
     *
     * // CONFIGURATION DU Compteur de front montant
     *
     *
     **/

    gpio_config_t gpio_configuration = {
        .pin_bit_mask = (1 << 3),              // GPIO 3
        .mode = GPIO_MODE_INPUT,               // Configure le GPIO 3 comme entrée
        .pull_up_en = GPIO_PULLUP_DISABLE,     // Désactive PULLUP
        .pull_down_en = GPIO_PULLDOWN_DISABLE, // Désactive PULLDOWN
        .intr_type = GPIO_INTR_NEGEDGE         // Compteur de front descendant
    };

    gpio_config(&gpio_configuration); // Configure le GPIO

    int l_int_time = 0;      // Déclarer une variable pour stocker le temps
    int l_int_time_old = 0;  // Déclarer une variable pour stocker le temps précédent
    int l_inttab_values[33]; // Déclarer un tableau de 33 entiers

    timer_config_t timer_config = {
        .alarm_en = TIMER_ALARM_EN,         // Activer l'alarme
        .counter_en = TIMER_START,          // Démarrer le compteur
        .intr_type = TIMER_INTR_LEVEL,      // Type d'interruption
        .counter_dir = TIMER_COUNT_UP,      // Compteur vers le haut
        .auto_reload = TIMER_AUTORELOAD_EN, // Activer le rechargement automatique
        .divider = 80                       // Diviseur de 80

    };
    timer_init(TIMER_GROUP_0, TIMER_0, &timer_config);

    /*


            if (gpio_get_level(GPIO_NUM_3) == 0) // Si le niveau de la broche GPIO3 est à 0
            {
                for (size_t i = 0; i < 33; i++)
                {
                    l_inttab_values[i] = gpio_get_level(GPIO_NUM_3); // Lire le niveau de la broche GPIO3
                    vTaskDelay(2 / portTICK_PERIOD_MS);            // Attendre 100 ms
                    printf("%i", l_inttab_values[i]);                // Imprimer le niveau de la broche
                }
                printf("\n");                // Imprimer le niveau de la broche

            }
            else // Sinon
            {
            }
            vTaskDelay(1);
    */
    TickType_t tickCount1, tickCount2;


/*    
    while (true)
    {

        if (gpio_get_level(GPIO_NUM_3) == 0)
        {
            timer_start(TIMER_GROUP_0, TIMER_0);

            if (gpio_get_level(GPIO_NUM_3) == 1)
            {
                timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &tickCount1);
                do
                {
                    timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &tickCount2);

                } while ((tickCount1 - tickCount2) < 2500);

                printf("TEST\n");
            }
        }
    }

    /*        while (1)
            {
                timer_get_counter_value(TIMER_GROUP_0,TIMER_0, &tickCount1);
                vTaskDelay(1000/portTICK_PERIOD_MS);
                timer_get_counter_value(TIMER_GROUP_0,TIMER_0, &tickCount2);
                printf("[%ld] %lu ms have passed!\r\n",
                       tickCount2, (tickCount2 - tickCount1)*1000/CONFIG_FREERTOS_HZ);
            }
    */
    // Boucle infinie pour prendre la valeur attendre 1 secondes pour test le timer avec résultat en MicroSeconde
}
