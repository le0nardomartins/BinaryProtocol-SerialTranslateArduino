# FirmwareDrawix

Firmware Arduino para controle de 3 eixos (X, Y, Z) via protocolo serial binário simples.

O projeto recebe frames pela porta serial, valida checksum, decodifica direção e quantidade de passos por eixo, e aciona pinos `STEP`/`DIR` para movimentação.

## Estrutura do projeto

- `exemplo/exemplo.ino`: sketch principal (setup, loop, execução de movimento e logs).
- `exemplo/BinaryProtocol.h`: interface da classe de protocolo.
- `exemplo/BinaryProtocol.cpp`: implementação de leitura, sincronização e decodificação de frame.

## Como funciona

1. O firmware aguarda bytes na serial.
2. Quando encontra o byte de sincronismo (`0xAA`), começa a montar um frame de 9 bytes.
3. Ao completar o frame, calcula checksum (XOR dos bytes 1 a 7) e compara com o byte final.
4. Se válido, converte os campos para um comando de movimento:
   - tipo (`G0`/`G1`),
   - direção de cada eixo (`X`, `Y`, `Z`),
   - passos de cada eixo (`stepX`, `stepY`, `stepZ`).
5. Executa os passos eixo por eixo (implementação simples sequencial).

## Formato do frame binário (9 bytes)

| Byte | Campo         | Descrição |
|------|---------------|-----------|
| 0    | `SYNC`        | Sempre `0xAA` |
| 1    | `FLAGS`       | Bits de controle |
| 2-3  | `stepX`       | `uint16_t` little-endian |
| 4-5  | `stepY`       | `uint16_t` little-endian |
| 6-7  | `stepZ`       | `uint16_t` little-endian |
| 8    | `CHECKSUM`    | XOR dos bytes 1..7 |

### Bits de `FLAGS`

- Bit 0: tipo de movimento (`1 = G1`, `0 = G0`)
- Bit 1: direção X (`1 = +`, `0 = -`)
- Bit 2: direção Y (`1 = +`, `0 = -`)
- Bit 3: direção Z (`1 = +`, `0 = -`)
- Bits 4..7: reservados para uso futuro

## Mapeamento de pinos

- Eixo X: `STEP = 2`, `DIR = 5`
- Eixo Y: `STEP = 3`, `DIR = 6`
- Eixo Z: `STEP = 4`, `DIR = 7`

## Serial

- Baud rate: `115200`
- Mensagens no monitor serial:
  - `READY`: firmware inicializado
  - linha com dados decodificados (`VALID`, `TYPE`, `DIR*`, `STEP*`, `FLAGS`, `CHK`)
  - `OK`: comando processado

## Como compilar e enviar

1. Abra `exemplo/exemplo.ino` na Arduino IDE.
2. Selecione a placa e porta corretas.
3. Compile e faça upload.
4. Abra o Monitor Serial em `115200` para acompanhar logs.

## Observações

- A execução atual dos eixos e sequencial (X, depois Y, depois Z).  
  Para movimento coordenado, pode-se implementar interpolação simultânea (ex.: Bresenham).
- O tempo de pulso esta em `delayMicroseconds(300)` para `HIGH` e `LOW`, podendo ser ajustado conforme driver/motor.
