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

## Stack utilizada

- C++ (Arduino), [Mqtt](https://mqtt.org/)

## Projeto
Esta aplicação faz parte de um projeto onde trabalhei com mais três pessoas: [Gabriel](https://github.com/mogba) , [Paloma](https://github.com/Paloma-Marian) e [Ricardo](https://github.com/ricardo-14).

Este dispensador possui um completo sistema onde é possível fazer a sua configuração e gerenciamento de alarmes.<br>
O front-end pode ser [acessado aqui](https://github.com/mogba/pillio)<br>
O back-end pode ser [acessado aqui](https://github.com/AdryanR/api-pilli-o)

Para exemplificar as funções, prints do front-end:
<p align="center">
  <img
    width="25%"
    alt="sistema-menu"
    src="https://user-images.githubusercontent.com/56984939/175789879-7a5b88ee-ef67-45d5-9a8e-725dedd27fea.png"
  >
  <img
    width="25%"
    alt="sistema-home"
    src="https://user-images.githubusercontent.com/56984939/175789890-57a6b942-bfe5-49fc-9fce-2063fb71211b.png"
  >
  <img
    width="25%"
    alt="app-edit-alarm"
    src="https://user-images.githubusercontent.com/56984939/175789908-1a444535-4eae-4da8-bb2e-7659db9ba412.png"
  >
</p>

## Como usar
```
IDE Arduino
```
