# PlatformIO project for Arduino Mega 2560

Проект содержит два независимых окружения PlatformIO для одной платы:

- `blink` управляет встроенным светодиодом и использует исходники из `src/blink`;
- `serial_status` раз в секунду отправляет сообщение в последовательный порт и использует исходники из `src/serial_status`.

Параметр `build_src_filter` в `platformio.ini` исключает общий каталог исходников и затем подключает только каталог выбранного окружения. Поэтому исходники двух программ не смешиваются при сборке.

## Структура

```text
.
├── platformio.ini
└── src
    ├── blink
    │   └── main.cpp
    └── serial_status
        └── main.cpp
```

## Сборка

Собрать окружения по отдельности:

```bash
pio run -e blink
pio run -e serial_status
```

Собрать все окружения, указанные в `default_envs`:

```bash
pio run
```

Для просмотра вывода окружения `serial_status` после прошивки:

```bash
pio device monitor -e serial_status
```
