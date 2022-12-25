#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/binary_info.h"

#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/vreg.h"
#include "hardware/clocks.h"

#include "modules/pico-onewire/api/one_wire.h"

#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/apps/mqtt.h"

#include "settings.h"

mqtt_client_t *client;
struct mqtt_connect_client_info_t ci;
// ip_addr_t ip;

void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    if(status == MQTT_CONNECT_ACCEPTED) {
        printf("MQTT connected\n");
    }
}

// void mqtt_dns_cb(const char *name, const ip4_addr *ip, void *args)
// {
//     err_t err;

//     if (ip == NULL) {
//         printf("Unable to resolve %s\n", name);
//     } else {
//         err = mqtt_client_connect(client, ip, MQTT_PORT, mqtt_connection_cb, 0, &ci);
//         if (err != ERR_OK) {
//             printf("mqtt_client_connect returned %d\n", err);
//         }
//     }
// }

void mqtt_connect()
{
    err_t err;

    memset(&ci, 0, sizeof(ci));
    ci.client_id = MQTT_CLIENT_ID;

    err = mqtt_client_connect(client, &ip, MQTT_PORT, mqtt_connection_cb, 0, &ci);
    if (err != ERR_OK) {
        printf("mqtt_client_connect returned %d\n", err);
    }
    // err = dns_gethostbyname(MQTT_SERVER, &ip, mqtt_dns_cb, NULL);
    // if (err = ERR_OK) {
    //     printf("No callback needed\n");
    //     mqtt_dns_cb(MQTT_SERVER, &ip, NULL);
    // }    
}

/* Called when publish is complete either with sucess or failure */
void mqtt_pub_request_cb(void *arg, err_t result)
{
    if(result != ERR_OK) {
        printf("Publish result: %d\n", result);
    }
}

void mqtt_publish(float int_temp, float ext_temp)
{
    char pub_payload[128];

    snprintf(pub_payload, 128, "{ \"internal\": %.1f, \"temperature\": %.1f }", int_temp, ext_temp);
    printf("%s\n", pub_payload);

    err_t err;
    err = mqtt_publish(client, MQTT_TOPIC, pub_payload, strlen(pub_payload), 2, 1, mqtt_pub_request_cb, NULL);
    if (err != ERR_OK) {
        printf("Publish err: %d\n", err);
    }
}

int main() {
    bi_decl(bi_program_description("Pico W DS18B20 MQTT"));
    bi_decl(bi_1pin_with_name(CYW43_WL_GPIO_LED_PIN, "On-board LED"));

    // Setup
    if (cyw43_arch_init_with_country(CYW43_COUNTRY_UK)) {
        printf("WiFi init failed");
        return -1;
    }
    cyw43_arch_enable_sta_mode();

    stdio_init_all();
    sleep_ms(5000);
    printf("Pico W DS18B20 MQTT\n");
    printf("Build: %s %s\n", __DATE__, __TIME__);

    int err;
    do {
        err = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000);
        if (err) {
            printf("WiFi failed to connect: %d\n", err);
            sleep_ms(1000);
        }
    }
    while (err);

    printf("WiFi connected\n");

    client = mqtt_client_new();
    if(client != NULL) {
        mqtt_connect();
    }

    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(ADC_PIN);

    printf("ADC Configured\n");

    One_wire one_wire(ONEWIRE_PIN);
    one_wire.init();
    rom_address_t address{};

    one_wire.single_device_read_rom(address);
    printf("Onewire configured, address %02x%02x%02x%02x%02x%02x%02x%02x\n",
           address.rom[0], address.rom[1], address.rom[2], address.rom[3],
           address.rom[4], address.rom[5], address.rom[6], address.rom[7]);
    // First reading is usually garbage, throw it away.
    one_wire.single_device_read_rom(address);
    one_wire.convert_temperature(address, true, false);
    float ext_temp = one_wire.temperature(address);

    sleep_ms(250);

    // Begin main loop
    while (true) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

        const double conversion_factor = 3.3f / (1 << 12);
        uint16_t raw = adc_read();
        float voltage = raw * conversion_factor;
        float int_temp = 27 - (voltage - 0.706)/0.001721;
        // printf("On board temperature %.1fºC\n", int_temp);

        one_wire.single_device_read_rom(address);
        one_wire.convert_temperature(address, true, false);
        ext_temp = one_wire.temperature(address);
        // printf("Onewire Temperature: %.1fºC\n", ext_temp);

        if (ext_temp > -100.0) {
            mqtt_publish(int_temp, ext_temp);
        }

        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(DELAY);
    }
}
