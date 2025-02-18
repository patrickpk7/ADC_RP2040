# Projeto EmbarcaTech: Controle de Display e LEDs RGB com Joystick no Raspberry Pi Pico

Este projeto utiliza um **Raspberry Pi Pico** para controlar um **display OLED SSD1306** e **LEDs RGB** com base no movimento de um **joystick analógico**. Além disso, há interação com botões para alterar o estado do sistema.

## 📌 Funcionalidades
- **Leitura do Joystick**: Captura os valores dos eixos X e Y através do conversor ADC.
- **Controle de LEDs RGB**: Ajusta a intensidade dos LEDs vermelho e azul conforme o movimento do joystick.
- **Interação com Botões**:
  - Pressionar o botão do joystick altera o estilo da borda do display e altera o estado do Led VERDE.
  - Pressionar o botão A ativa/desativa o controle dos LEDs RGB.
- **Exibição Gráfica**: Um quadrado se move na tela OLED de acordo com o movimento do joystick.
- **Debounce nos botões** para evitar múltiplos acionamentos involuntários.

---

## 🛠️ Hardware Utilizado
- Raspberry Pi Pico
- Display OLED SSD1306 (I2C)
- Joystick analógico
- LEDs RGB
- Resistores de pull-up para os botões (caso necessário)

### 📌 Pinos Utilizados
| Componente        | GPIO       |
|------------------|-----------|
| Joystick Eixo Y  | 26        |
| Joystick Eixo X  | 27        |
| Botão Joystick   | 22        |
| Botão A          | 5         |
| LED Azul         | 12        |
| LED Verde        | 11        |
| LED Vermelho     | 13        |
| I2C SDA         | 14        |
| I2C SCL         | 15        |

---

## 📦 Como Executar
### 🔧 Pré-requisitos
1. Instale o **SDK do Raspberry Pi Pico** e o **CMake**.
2. Conecte o Raspberry Pi Pico ao seu computador.

### 📜 Compilação e Upload
Execute os seguintes comandos no terminal:
```sh
mkdir build
cd build
cmake ..
make
```
Após a compilação bem-sucedida, envie o código para o Raspberry Pi Pico via **BOOTSEL Mode** copiando o arquivo `.uf2` gerado para a unidade montada do Pico.

---

## 🖥️ Como Funciona
1. **Joystick**: Controla o movimento de um quadrado no display e a intensidade dos LEDs RGB.
2. **Botão do Joystick**: Alterna entre dois estilos de borda no display e liga/desliga o Led VERDE.
3. **Botão A**: Ativa ou desativa o controle dos LEDs RGB pelo joystick.
4. **Debounce**: Impede acionamentos acidentais dos botões.

O código usa **PWM** para controlar o brilho dos LEDs e **I2C** para comunicação com o display OLED.

---

## 🔄 Fluxo do Programa
1. **Inicialização**:
   - Configuração dos **GPIOs** para botões e LEDs.
   - Configuração do **I2C** para comunicação com o display OLED.
   - Inicialização do **ADC** para leitura do joystick.
   - Configuração do **PWM** para os LEDs RGB.
2. **Loop Principal**:
   - Leitura do **joystick** e mapeamento dos valores do ADC.
   - Controle do **display OLED** para exibir a posição do quadrado.
   - Atualização dos **LEDs RGB** conforme o movimento do joystick.
   - Gerenciamento dos **botões** para alternar estados.

---

##  Link para vídeo demonstração
 - https://drive.google.com/file/d/1YLnh5G6DiHwIgnFpUP4b9n3npJJZMK-X/view?usp=sharing

---

🚀 **Desenvolvido para aplicações embarcadas no Raspberry Pi Pico!**

