# PlatformIO project for Arduino Mega 2560

Проект содержит три независимых окружения PlatformIO для одной платы:

- `blink` управляет встроенным светодиодом и использует исходники из `src/blink`;
- `serial_status` раз в секунду отправляет сообщение в последовательный порт и использует исходники из `src/serial_status`;
- `motor_reg` формирует управляющий сервосигнал для ESC на пине `D2` и использует исходники из `src/motor_reg`. Перед использованием обязательно прочитайте [инструкцию по безопасному подключению](docs/motor_reg/README.md).

Параметр `build_src_filter` в `platformio.ini` исключает общий каталог исходников и затем подключает только каталог выбранного окружения. Поэтому исходники разных программ не смешиваются при сборке.

## Структура

```text
.
├── docs
│   └── motor_reg
│       └── README.md
├── platformio.ini
└── src
    ├── blink
    │   └── main.cpp
    ├── motor_reg
    │   └── main.cpp
    └── serial_status
        └── main.cpp
```

## Сборка

Собрать окружения по отдельности:

```bash
pio run -e blink
pio run -e serial_status
pio run -e motor_reg
```

Собрать все окружения, указанные в `default_envs`:

```bash
pio run
```

Для просмотра вывода окружений с последовательной диагностикой после прошивки:

```bash
pio device monitor -e serial_status
pio device monitor -e motor_reg
```
