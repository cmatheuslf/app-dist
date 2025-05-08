# Monitor Remoto de Qualidade do Ar

## Sobre o Projeto

Este projeto consiste em um sistema de monitoramento da qualidade do ar ambiente para acompanhamento de pacientes com dificuldades respiratórias, tanto de forma local quanto remota. O sistema coleta dados de CO2, temperatura e umidade, exibindo-os em um display OLED e enviando-os para um servidor MQTT para monitoramento em tempo real.

Com a crescente preocupação com doenças respiratórias desde a pandemia de 2022, tornou-se necessário o acompanhamento mais rigoroso das condições ambientais que podem afetar o bem-estar respiratório das pessoas.

## Funcionalidades

- Coleta dados de qualidade do ar (CO2, temperatura e umidade) utilizando um sensor SCD30
- Exibe os dados coletados em um display OLED SSD1306
- Envia os dados para um servidor MQTT para monitoramento remoto
- Implementa alertas visuais (LED) quando os níveis excedem limites seguros:
  - CO2 acima de 1000 PPM
  - Temperatura acima de 40°C 
  - Umidade relativa acima de 80% ou abaixo de 40%

## Arquitetura do Sistema

### Hardware
- Raspberry Pi Pico (RP2040)
- Sensor SCD30 (CO2, temperatura e umidade)
- Display OLED SSD1306
- LED para alertas visuais

### Software
- Firmware em C para o Raspberry Pi Pico
- Conexão Wi-Fi para comunicação em rede
- Protocolo MQTT para transmissão de dados
- Sistema Web para visualização dos dados (Frontend/Backend)

## Fluxo de Funcionamento

### Sistema Embarcado
1. Inicialização do sistema (configurações de I2C, PWM, Wi-Fi)
2. Coleta de dados do sensor SCD30
3. Verificação da qualidade do ar (comparação com limites seguros)
4. Envio dos dados para o servidor MQTT
5. Exibição no display OLED
6. Ativação de alertas quando necessário

### Sistema Web
1. Subscrição aos tópicos MQTT
2. Recebimento das publicações
3. Armazenamento dos dados
4. Exibição em tempo real através de gráficos e valores numéricos

## Instalação e Configuração

### Requisitos
- Ambiente de desenvolvimento para Raspberry Pi Pico
- Servidor MQTT (como Mosquitto)
- Componentes de hardware conforme o esquemático

### Configuração Wi-Fi e MQTT
Altere as seguintes variáveis no código:
```c
char WIFI_SSID[] = "SeuSSID";
char WIFI_PASSWORD[] = "SuaSenha";

// Na função start_mqtt_client():
IP4_ADDR(&broker_ip, 192, 168, 1, 105); // Altere para o IP do seu servidor MQTT
```

## Esquemático do Hardware

O projeto utiliza dois barramentos I2C:
- I2C0: Sensor SCD30 (pinos 0 e 1)
- I2C1: Display OLED (pinos 14 e 15)

O LED de alerta está conectado ao pino 13 e controlado via PWM.

## Contribuições

Contribuições são bem-vindas! Sinta-se à vontade para fazer um fork do projeto, implementar melhorias e enviar pull requests.

## Licença

Este projeto está licenciado sob a licença MIT.

## Autor

Carlos Matheus de Lima Ferreira - cmatheuslf@alu.ufc.br

## Referências

- Karnati, H. (2023). IoT-Based Air Quality Monitoring System with Machine Learning for Accurate and Real-time Data Analysis.
- DE VITO, S. et al. (2024). Future Low-Cost Urban Air Quality Monitoring Networks: Insights from the EU's AirHeritage Project.
- GARCÍA, M. R. et al. (2023). Review of low-cost sensors for indoor air quality: Features and applications.
- ARAÚJO, T. et al. (2023). Calibration Assessment of Low-Cost Carbon Dioxide Sensors Using the Extremely Randomized Trees Algorithm.
