// Este projeto utiliza um Raspberry Pi Pico para controlar um display OLED SSD1306 e LEDs RGB com base no movimento 
// de um joystick analógico. Além disso, há interação com botões para alterar o estado do sistema.


// Bibliotecas para funcionamento correto do código
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "hardware/pwm.h"

#define I2C_PORT i2c1 // Definição ta porta i2c na placa pico w
#define I2C_SDA 14 // Definição do SDA para GPIO 14
#define I2C_SCL 15// Definição do SCL para GPIO 15
#define endereco 0x3C // Endereço para Display
#define Y_PIN 26  // GPIO para eixo Y
#define X_PIN 27  // GPIO para eixo X
#define JOYSTICK_CLICK 22 // GPIO para botão do Joystick
#define Botao_A 5 // GPIO para botão A
#define LED_AZUL 12 // GPIO 12 para led azul
#define LED_VERDE 11 // GPIO 11 para led verde
#define LED_VERMELHO 13 // GPIO 13 para led vermelho

// Definição dos parametros do PWM
#define WRAP 4095
#define DIV_CLK 30.52f


// Variaveis para leds e borda do display
bool led_estado = 0;
bool leds_rgb_setup = 1;
bool borda_ativa = 0;

absolute_time_t last_interrupt_time = 0;


ssd1306_t ssd; // Inicializa a estrutura do display

// Configuração do PWM
void pwm_setup(uint gpio, uint wrap, float clkdiv) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, wrap);
    pwm_config_set_clkdiv(&config, clkdiv);
    pwm_init(slice_num, &config, true);
}


// MOVIMENTOS DO EIXO Y DO JOYSTICK
int32_t VR_Y(uint16_t valor_y) {
    int16_t valor_eixo_y;
    if (valor_y < 2047) {
        valor_eixo_y = 2047 - valor_y;
    } else if (valor_y > 2047) {
        valor_eixo_y = valor_y - 2047;
    } else {
        valor_eixo_y = 0;
    }
    return valor_eixo_y;
}

// MOVIMENTOS DO EIXO X DO JOYSTICK
int32_t VR_X(uint16_t valor_x) {
    int16_t valor_eixo_x;
    if (valor_x < 2047) {
        valor_eixo_x = 2047 - valor_x;
    } else if (valor_x > 2047) {
        valor_eixo_x = valor_x - 2047;
    } else{
        valor_eixo_x = 0;
    }
    return valor_eixo_x;
}


// Interrupção para botão do joystick e botão A com DEBOUNCE ativo
void gpio_irq_handler(uint gpio, uint32_t events)
{   
    //Debounce
    absolute_time_t now = get_absolute_time();
    if (absolute_time_diff_us(last_interrupt_time, now) < 250000) // 250ms debounce
        return; // ignora interrupções ate o intervalo for concluido

    last_interrupt_time = now; // atualiza 
    
    // Se o botao do joystick for pressionado, muda o estado do Led Verde e da Borda no display
    if (gpio == JOYSTICK_CLICK){
        led_estado = !led_estado;
        gpio_put(LED_VERDE, led_estado);
        borda_ativa = !borda_ativa;

    } else if (gpio == Botao_A) { // Se o botão A for pressionado, muda o estado dos LEDS RGBS
        leds_rgb_setup = !leds_rgb_setup;
        pwm_set_gpio_level(LED_VERMELHO, leds_rgb_setup ? VR_X(adc_read()) : 0);
        pwm_set_gpio_level(LED_AZUL, leds_rgb_setup ? VR_Y(adc_read()) : 0);
    }
        
}


// Principal
int main()
{
  stdio_init_all(); // Inicialização serial
  sleep_ms(1000); // Delay para verificar se iniciou corretamente
    
  // Inicializa e direciona o botão do Joystick  
  gpio_init(JOYSTICK_CLICK);
  gpio_set_dir(JOYSTICK_CLICK, GPIO_IN);
  gpio_pull_up(JOYSTICK_CLICK); // Ativa pullup no joystick
  gpio_set_irq_enabled_with_callback(JOYSTICK_CLICK, GPIO_IRQ_EDGE_FALL, true, gpio_irq_handler); // Chama interrupção para o botao joystick

  // Inicializa e direciona o botão A 
  gpio_init(Botao_A);
  gpio_set_dir(Botao_A, GPIO_IN);
  gpio_pull_up(Botao_A);
  gpio_set_irq_enabled_with_callback(Botao_A, GPIO_IRQ_EDGE_FALL, true, gpio_irq_handler); // Chama a interrupção para botao A


  pwm_setup(LED_AZUL, WRAP, DIV_CLK); // PWM para Led Azul
  pwm_setup(LED_VERMELHO, WRAP, DIV_CLK); // PWM para Led Vemrelho
  gpio_init(LED_VERDE); // Inicia o Led Verde
  gpio_set_dir(LED_VERDE, GPIO_OUT); // Direciona o led Verde

  adc_init(); // Inicia o ADC
  adc_gpio_init(Y_PIN); // Inicia o GPIO do eixo Y como ADC
  adc_gpio_init(X_PIN);  // Inicia o GPIO do eixo X como ADC
  
  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Define o GPIO como função I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Define o GPIO como função I2C
  gpio_pull_up(I2C_SDA); // Pull up na linha de dados
  gpio_pull_up(I2C_SCL); // Pull up na linha de clock
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
  ssd1306_config(&ssd); // Configura o display
  ssd1306_send_data(&ssd); // Envia os dados para o display

  // Limpa o display. O display inicia com todos os pixels apagados.
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  // Variaveis para valor do eixo X e Y
  uint16_t adc_value_x;
  uint16_t adc_value_y;  
  
  uint quadrado = 8; // Variavel para o quadrado no Display
  uint x_central = (WIDTH - quadrado) / 2; // Variavel para captar a central do eixo X
  uint y_central = (HEIGHT - quadrado) / 2; // Variavel para captar a central do eixo Y
  
  while (true)
  {
    adc_select_input(0); // Seleciona o ADC para eixo y. O pino 26 como entrada analógica
    adc_value_y = adc_read(); // Lê o valor do eixo Y
    adc_select_input(1); // Seleciona o ADC para eixo x. O pino 27 como entrada analógica
    adc_value_x = adc_read(); // Lê o valor do eixo X    
    
    // Se o Joystick estiver solto, desliga os LEDS RGB
    if(adc_value_x >= 2060 - 100 && adc_value_x <= 2060 + 100 && adc_value_y >= 2060 - 100 && adc_value_y <= 2060 + 100) {
        pwm_set_gpio_level(LED_AZUL, 0);
        pwm_set_gpio_level(LED_VERMELHO, 0);

    } else if(leds_rgb_setup) { // Se estiver em movimento, a intensidade do led é ativada via PWM
       
        pwm_set_gpio_level(LED_AZUL, VR_Y(adc_value_y)); // Intensidade do LED AZUL de acordo o valor do eixo Y
        pwm_set_gpio_level(LED_VERMELHO, VR_X(adc_value_x)); // Intensidade do LED VERMELHO de acordo o valor do eixo X
    }

    // Imprime os valores do eixo X e eixo Y
    printf("VR_X: %d \n VR_Y: %d \n", adc_value_x, adc_value_y);
    
    // Converte os valores analogicos do joystick para coordenadas no DISPLAY
    int p_x = x_central + (( (int)adc_value_x - 2048) * x_central)  /2048; // Quadrado para esquerda e direita
    int p_y = y_central + (( 2048 - (int)adc_value_y ) * y_central) / 2048; // Quadrado para cima e para baixo


    // Altera o desenho da borda
    if (borda_ativa == 0) {
        ssd1306_rect(&ssd, 3, 3, 122, 60, true, false);
    } else {
        ssd1306_rect(&ssd, 3, 3, 122, 58, true, false);
        ssd1306_rect(&ssd, 5, 5, 118, 54, true, false);
    }

    ssd1306_rect(&ssd, p_y, p_x, quadrado, quadrado, true, true);  // Desenha o quadrado preenchido na posição correspondente
    ssd1306_send_data(&ssd); // Atualiza display
    ssd1306_fill(&ssd, false);

    

    


    sleep_ms(100); // Para evitar sobrecarga
  }
}