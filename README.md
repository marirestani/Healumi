# Healumi
Objetos Inteligentes Conectados - Projeto de IoT de monitoramento e ajuste do nível de luminosidade em ambientes internos, focado em minimizar o impacto da luz artificial no ciclo circadiano.

O sistema utiliza um ESP32 para ler a intensidade da luz ambiente e, com base na hora do dia (obtida via NTP), ajusta a cor e o brilho de um atuador via protocolo MQTT.

## i. Descrição do Funcionamento e Uso

O sistema foi prototipado no Wokwi. O ESP32 (cliente MQTT) se conecta ao broker HiveMQ. A cada 5 segundos, ele:
1.  Lê a hora atual (NTP) e a intensidade da luz (LDR).
2.  **Lógica:** Se o ambiente estiver escuro, ele decide a cor ideal (luz fria de dia, luz quente à noite).
3.  **Publica** o comando (R,G,B) no tópico `healumi/light/set`.
4.  Ele também **assina** (subscribe) esse mesmo tópico, recebendo o comando de volta.
5.  Ao receber o comando, a função de callback aciona o atuador (NeoPixel) com a cor e brilho corretos.

## ii. Software Desenvolvido

Todo o software está contido no arquivo `sketch.ino` (escrito em C++/Arduino). Os arquivos de simulação são:
* `diagram.json`: Descreve as conexões de hardware virtuais.
* `wokwi.toml`: Define a configuração de compilação e as bibliotecas (`PubSubClient`, `NTPClient`, `Adafruit_NeoPixel`).

## iii. Hardware Utilizado

| Componente | Função |
| :--- | :--- |
| **ESP32 DevKitC V4** | Microcontrolador e módulo Wi-Fi. |
| **Módulo LDR** | Sensor para medir a intensidade da luz ambiente. |
| **LED NeoPixel (WS2812)** | Atuador, simula a lâmpada inteligente (cor e brilho). |
| **Resistor (470Ω)** | Proteção da linha de dados do atuador. |

## iv. Interfaces e Protocolos

* **Wi-Fi (TCP/IP):** Usado pelo ESP32 para se conectar à internet.
* **MQTT (Message Queuing Telemetry Transport):** Protocolo principal de comunicação IoT. O ESP32 atua como cliente, publicando dados do sensor e comandos de atuação no broker público `broker.hivemq.com`.
* **NTP (Network Time Protocol):** Usado para buscar a hora mundial em `pool.ntp.org` e aplicar a lógica circadiana.
