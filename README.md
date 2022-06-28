# Dispenser - Pilli-o

Prótótipo de dispenser de medicamentos automátizado para idosos utilizando sistema embarcado, com o <b>microcontrolador ESP8266</b>.

<p align="center">
  <img
    width="35%"
    alt="Dispenser prototype with circular shape"
    src="https://user-images.githubusercontent.com/56984939/175790549-d4c8776e-41c4-46e5-bc72-2e3f99bc8b58.png"
  >
</p>

O dispensador tem 15 compartimentos utilizáveis para colocar medicamentos. Possuindo um alarme sonoro quando chegar a hora do remédio e na tampa um mecanismo que registra quando um idoso retira o medicamento do aparelho, para monitorar se "tomou" ou não o medicamento.

## Funcionalidades

- Requisições HTTP
- Comunicação MQTT
- Captive Portal (com scan de redes próximas)
- Wi-FI

Umas das funcionalidades é a rede Wi-FI que o dispenser gera com um captive portal, para escolher a rede no qual vai ser conectado.

<p align="center">
  <img
    width="20%"
    alt="Captive portal gerado pelo dispenser"
    src="https://user-images.githubusercontent.com/92650594/176282171-33bf97c8-f6d0-4b97-b263-948cb4191b66.jpeg"
  >
  <img
    width="20%"
    alt="Captive portal gerado pelo dispenser"
    src="https://user-images.githubusercontent.com/92650594/176282166-94d9987a-bf58-441e-99ad-3a0096843b1d.jpeg"
  >
  <img
    width="20%"
    alt="Captive portal gerado pelo dispenser"
    src="https://user-images.githubusercontent.com/92650594/176282155-76dd6458-c1f4-4bdd-a1eb-631ff98f0e92.jpeg"
  >
</p>

## Stack utilizada

- C++ (Arduino), [Mqtt](https://mqtt.org/)

## Projeto
Esta aplicação faz parte de um projeto onde trabalhei com mais três pessoas: [Gabriel](https://github.com/mogba) , [Paloma](https://github.com/Paloma-Marian) e [Ricardo](https://github.com/ricardo-14).

Este dispensador possui um completo sistema onde é possível fazer a sua configuração e gerenciamento de alarmes.<br>
<b>O front-end pode ser [acessado aqui](https://github.com/mogba/pillio)</b><br>
<b>O back-end pode ser [acessado aqui](https://github.com/AdryanR/api-pilli-o)</b>


## Como usar
```
IDE Arduino
```
