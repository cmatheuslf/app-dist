#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/cyw43_arch.h"
#include "lwip/apps/mqtt.h"
#include "lwip/ip_addr.h"

mqtt_client_t *global_mqtt_client = NULL;

#define LED_PIN 11


static int scan_result(void *env, const cyw43_ev_scan_result_t *result) {
    if (result) {
        printf("ssid: %-32s rssi: %4d chan: %3d mac: %02x:%02x:%02x:%02x:%02x:%02x sec: %u\n",
            result->ssid, result->rssi, result->channel,
            result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4], result->bssid[5],
            result->auth_mode);
    }
    return 0;
}


char WIFI_SSID[] = "LESC";
char WIFI_PASSWORD[] = "A33669608F";



// Define os pinos I2C
#define I2C_SDA_PIN 2
#define I2C_SCL_PIN 3
#define I2C_PORT i2c1

// Endereço I2C do SCD30
#define SCD30_ADDRESS 0x61

void soft_reset_scd30();
void stop_continuous_scd30();
void scd30_read_date(uint8_t *reg, uint8_t *data, uint8_t size);


void scd30_init(){
    sleep_ms(2000);
    printf("Inicializando\n");
    soft_reset_scd30();    
    sleep_ms(2000);
    stop_continuous_scd30();    
}
void soft_reset_scd30(){
    uint8_t reg[18]={0};
    uint16_t command = 0xd304;
    uint8_t offset=0;
    reg[offset++] = (uint8_t)((command & 0xFF00) >> 8);
    reg[offset++] = (uint8_t)((command & 0x00FF) >> 0);
    printf("Soft Reset");
    i2c_write_blocking(I2C_PORT, SCD30_ADDRESS, &reg, 1, true);
}

void stop_continuous_scd30(){
    uint8_t reg[18]={0};
    uint16_t command = 0x0104;
    uint8_t offset=0;
    reg[offset++] = (uint8_t)((command & 0xFF00) >> 8);
    reg[offset++] = (uint8_t)((command & 0x00FF) >> 0);
    printf("Stop Continuous scd30\n");
    i2c_write_blocking(I2C_PORT, SCD30_ADDRESS, &reg, 1, true);
}

void get_measurament(float *co2, float *temp, float *rh){
    uint8_t reg[18]={0};
        uint16_t command = 0x0300;
        uint8_t offset=0;
        reg[offset++] = (uint8_t)((command & 0xFF00) >> 8);
        reg[offset++] = (uint8_t)((command & 0x00FF) >> 0);

        uint8_t data[20]={0};
        scd30_read_date(reg, data, offset);
        unsigned int tempU32 = (data[0] << 24) | (data[1] << 16) | (data[3] << 8) | data[4];
        *co2 = *(float*)&tempU32;
        tempU32 = (data[6] << 24) | (data[7] << 16) | (data[9] << 8) | data[10];
        *temp = *(float*)&tempU32;
        tempU32 = (data[12] << 24) | (data[13] << 16) | (data[15] << 8) | data[16];
        *rh = *(float*)&tempU32;
        printf("CO2: %.2f\nTemp: %.2f\nRH: %.2f", *co2, *temp, *rh);
}


// Função para escrever um byte no DS1307
void ds1307_write_byte(uint8_t reg, uint8_t data) {
    uint8_t buf[] = {reg, data};
    i2c_write_blocking(I2C_PORT, SCD30_ADDRESS, buf, 2, false);
}

// Função para ler dados do SCD30
void scd30_read_date(uint8_t *reg, uint8_t *data, uint8_t size) {
    printf("reg on read: %x, %x\n", reg[0], reg[1]); 
    i2c_write_blocking(I2C_PORT, SCD30_ADDRESS, reg, size, false);
    sleep_ms(300);
    i2c_read_blocking(I2C_PORT, SCD30_ADDRESS, data, 18, true);
}

//mqtt functions
void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    if (status == MQTT_CONNECT_ACCEPTED) {
        printf("Conexão MQTT bem-sucedida!\n");
        gpio_put(LED_PIN, 1);
        sleep_ms(500);
        gpio_put(LED_PIN, 0);
    } else {
        printf("Falha na conexão MQTT: %d\n", status);
    }
}

void start_mqtt_client(void) {
    global_mqtt_client = mqtt_client_new();
    if (!global_mqtt_client) {
        printf("Falha ao criar cliente MQTT\n");
        return;
    }

    ip_addr_t broker_ip;
    IP4_ADDR(&broker_ip, 192, 168, 2, 117); // Change for you broker ip

    struct mqtt_connect_client_info_t client_info = {
        .client_id = "pico_client", //your client id
        .client_user = NULL,
        .client_pass = NULL,
        .keep_alive = 60,
    };

    mqtt_client_connect(global_mqtt_client, &broker_ip, MQTT_PORT, mqtt_connection_cb, NULL, &client_info);
}

int main() {
    stdio_init_all();
    sleep_ms(10000);
    printf("SSID: %s, pwd: %s", WIFI_SSID, WIFI_PASSWORD);
    //######################################################################pragma endregion
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect.\n");
        return 1;
    } else {
        printf("Connected.\n");
        gpio_put(LED_PIN, 1);
        sleep_ms(500);
        gpio_put(LED_PIN, 0);
    }
    //######################################################################pragma endregion


    // Inicializa a comunicação I2C
    i2c_init(I2C_PORT, 100 * 4000); // 40kHz
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    scd30_init();
    start_mqtt_client();

    const char *topic_co2 = "air-monitor/co2";
    const char *topic_temp = "air-monitor/temp";
    const char *topic_rh = "air-monitor/rh";
    char payload[16] = "Hello, Pico!";
    while (true) {
        tight_loop_contents(); // Loop principal
        float co2, temp, rh;
        get_measurament(&co2, &temp, &rh);
        printf("\nfunction return %.2f %.2f %.2f\n", co2, temp, rh);

        
        if(global_mqtt_client && mqtt_client_is_connected(global_mqtt_client)) {
            sprintf(payload, "%.2f", co2);
            err_t err = mqtt_publish(global_mqtt_client, topic_co2, payload, strlen(payload), 1, 0, NULL, NULL);
            printf("Mensagem enviada: %s\n", payload);
            sprintf(payload, "%.2f", temp);
            err = mqtt_publish(global_mqtt_client, topic_temp, payload, strlen(payload), 1, 0, NULL, NULL);
            printf("Mensagem enviada: %s\n", payload);
            sprintf(payload, "%.2f", rh);
            err = mqtt_publish(global_mqtt_client, topic_rh, payload, strlen(payload), 1, 0, NULL, NULL);
            printf("Mensagem enviada: %s\n", payload);
        }
        sleep_ms(5000); // Espera 5 segundos
    }

    return 0;
}