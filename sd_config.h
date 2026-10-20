#pragma once
// Common VGA32/PICO SD wiring. Verify against YOUR board schematic.
// GPIO16/17 are reserved for PSRAM on WROVER: never use them for SD.
#ifndef BXRUN_SD_ENABLED
#define BXRUN_SD_ENABLED 1
#endif
// Adapt these four values to the wiring of YOUR SD socket.
#ifndef BXRUN_SD_MISO
#define BXRUN_SD_MISO 2
#endif
#ifndef BXRUN_SD_MOSI
#define BXRUN_SD_MOSI 12
#endif
#ifndef BXRUN_SD_SCK
#define BXRUN_SD_SCK 14
#endif
#ifndef BXRUN_SD_CS
#define BXRUN_SD_CS 13
#endif
