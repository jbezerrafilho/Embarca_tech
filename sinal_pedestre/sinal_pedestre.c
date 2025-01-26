#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"

// Definições dos pinos dos LEDs e botão
#define LED_R_PIN 13
#define LED_G_PIN 11
#define LED_B_PIN 12
#define BTN_A_PIN 5

// Configuração dos pinos I2C para o OLED
const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

// Variáveis globais
int A_state = 0; // Indica se o botão foi pressionado

// Inicialização do display OLED
struct render_area frame_area;
uint8_t ssd[ssd1306_buffer_length];

// Funções do semáforo
void SinalAberto() {
    gpio_put(LED_R_PIN, 0);
    gpio_put(LED_G_PIN, 1);
    gpio_put(LED_B_PIN, 0);

    // Exibir mensagem no display
    memset(ssd, 0, ssd1306_buffer_length);
    ssd1306_draw_string(ssd, 0, 0, "SINAL ABERTO");
    ssd1306_draw_string(ssd, 0, 16, "ATRAVESSAR");
    ssd1306_draw_string(ssd, 0, 32, "COM CUIDADO");
    render_on_display(ssd, &frame_area);
}

void SinalAtencao() {
    gpio_put(LED_R_PIN, 1);
    gpio_put(LED_G_PIN, 1);
    gpio_put(LED_B_PIN, 0);

    // Exibir mensagem no display
    memset(ssd, 0, ssd1306_buffer_length);
    ssd1306_draw_string(ssd, 0, 0, "SINAL DE ATENCAO");
    ssd1306_draw_string(ssd, 0, 16, "PREPARE-SE");
    render_on_display(ssd, &frame_area);
}

void SinalFechado() {
    gpio_put(LED_R_PIN, 1);
    gpio_put(LED_G_PIN, 0);
    gpio_put(LED_B_PIN, 0);

    // Exibir mensagem no display
    memset(ssd, 0, ssd1306_buffer_length);
    ssd1306_draw_string(ssd, 0, 0, "SINAL FECHADO");
    ssd1306_draw_string(ssd, 0, 16, "AGUARDE");
    render_on_display(ssd, &frame_area);
}

// Função para aguardar com leitura do botão
int WaitWithRead(int timeMS) {
    for (int i = 0; i < timeMS; i += 100) {
        A_state = !gpio_get(BTN_A_PIN);
        if (A_state == 1) {
            return 1;
        }
        sleep_ms(100);
    }
    return 0;
}

int main() {
    stdio_init_all();

    // Configuração dos LEDs
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_init(LED_G_PIN);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);

    // Configuração do botão
    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);

    // Inicialização do display OLED
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init();

    // Configuração da área de renderização
    frame_area = (struct render_area) {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);

    // Loop principal do semáforo
    while (true) {
        // Sinal fechado para pedestre
        SinalFechado();
        // Espera com leitura do botäo
        A_state = WaitWithRead(10000);   
        
        // Se alguém apertar o botão o sinal fica verde por 8s
        if(A_state){              
            SinalAberto();
            sleep_ms(8000);
            SinalAtencao();
            sleep_ms(5000);

        // Ninguém apertou o botão
        }else{                          
            SinalAberto();
            sleep_ms(8000);                       
            SinalAtencao();
            sleep_ms(2000);     
           
        }
           
    }

    return 0;
}