# Healumi
Objetos Inteligentes Conectados - Projeto de IoT de monitoramento e ajuste do nível de luminosidade em ambientes internos, focado em minimizar o impacto da luz artificial no ciclo circadiano. O sistema se adapta tanto à hora do dia (NTP) quanto à luz ambiente (LDR), mantendo a iluminação do atuador em um perfil ideal.

O sistema utiliza um ESP32 para ler a intensidade da luz ambiente e, com base na hora do dia (obtida via NTP), ajusta a cor e o brilho de um atuador via protocolo MQTT.

## Descrição do Funcionamento e Uso

O Healumi é composto pelo módulo sensor de luminosidade LDR KY-018, a placa microcontroladora ESP32 DevKitC, e uma lâmpada LED Smart Wi-Fi como atuador. Para fins de prototipação virtual do Healumi, a lâmpada LED Smart Wi-Fi foi substituída pelo LED RGB WS2812 como atuador.

**Funcionamento:**
1.  O ESP32 se conecta ao Wi-Fi, inicializa o cliente NTP, e se conecta ao broker MQTT HiveMQ.
2.  A cada 5 segundos, o ESP32 lê a hora atual (NTP) e a intensidade da luz ambiente (LDR).
3.  O ESP32 publica o valor captado do sensor LDR no tópico healumi/sensor/ldr_value.
4.  O ESP32 executa a lógica circadiana:
    - Se o ambiente estiver claro (ldrValue < 2500), ele envia o comando para desligar o LED.
    - Se o ambiente estiver escuro (ldrValue > 2500) e o período for diurno, ele envia o comando de luz fria.
    - Se o ambiente estiver escuro (ldrValue > 2500) e o período for noturno, ele envia o comando de luz quente.
5.  O comando definido é publicado no tópico `healumi/light/set`.
6.  O ESP32 assina esse mesmo tópico, recebendo o comando previamente publicado.
7.  Ao receber o comando, a função de callback aciona o LED RGB WS2812 com a cor e brilho corretos.

**Reprodução:**
1. Abra esse projeto, com os arquivos libraries.txt, diagram.json e sketch.ino disponibilizados, no Wokwi.
2. Inicie a simulação.
3. Clique no módulo sensor LDR e move o slider "ILLUMINATION (LUX)" para acompanhar o comportamento do Healumi pelo Monitor Serial, e visualizar as adaptações do atuador.

## Software Desenvolvido

O software está contido no arquivo `sketch.ino` (escrito em C++). Os arquivos de simulação são:
* `diagram.json`: Descreve as conexões de hardware virtuais.
* `libraries.txt`: Define as bibliotecas (`PubSubClient`, `NTPClient`, `Adafruit_NeoPixel`).

## Hardware Utilizado

| Componente | Função |
| :--- | :--- |
| **ESP32 DevKitC** | Microcontrolador e módulo Wi-Fi. |
| **Módulo LDR KY-018** | Sensor para medir a intensidade da luz ambiente. |
| **LED NeoPixel (WS2812)** | Atuador, simula a lâmpada inteligente (cor e brilho). |
| **Resistor (470Ω)** | Proteção da linha de dados do atuador (apenas no protótipo). |

## Interfaces, Protocolos e Módulo de Comunicação

O próprio ESP32 DevKitC atua como o módulo de comunicação, utilizando seu rádio Wi-Fi 802.11 b/g/n para acesso à internet (TCP/IP).

* **TCP/IP:** Protocolo de transporte que garante a entrega das mensagens MQTT e NTP pela internet.
* **MQTT (Message Queuing Telemetry Transport):** Protocolo principal de comunicação IoT. O ESP32 atua como cliente, publicando dados do sensor e comandos de atuação no broker público `broker.hivemq.com`.
* **NTP (Network Time Protocol):** Protocolo de aplicação usado para consultar pool.ntp.org e obter a hora exata para a lógica circadiana.
