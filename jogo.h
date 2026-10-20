#pragma once
#include "tipos.h"
#include "motor.h"
#include "ecra.h"
#include "teclado.h"

// Implementation lives in a header, outside Arduino INO prototype scanning.
#if defined(__XTENSA__) && defined(ARDUINO_ARCH_ESP32)
#include <esp_system.h>
#include <esp_heap_caps.h>
#define BXRUN_BOOT_LOG(s) Serial.println(s)
#else
#define BXRUN_BOOT_LOG(s) ((void)0)
#endif
bool gameReady=false;
void setupGame(){
#if defined(__XTENSA__) && defined(ARDUINO_ARCH_ESP32)
 Serial.begin(115200);
 Serial.printf("\nBX RUN 6.5 reset=%d heap=%u DMA-largest=%u\n",int(esp_reset_reason()),unsigned(ESP.getFreeHeap()),unsigned(heap_caps_get_largest_free_block(MALLOC_CAP_DMA)));
#endif
 BXRUN_BOOT_LOG("VGA: allocation 640x200");
 DisplayController.begin();DisplayController.setResolution(VGA_640x200_70Hz);
#ifdef BXRUN_NATIVE_RASTER
 if(DisplayController.getViewPortWidth()!=640||DisplayController.getViewPortHeight()!=200){BXRUN_BOOT_LOG("ERROR: incomplete VGA viewport");return;}
 for(int y=0;y<200;++y){auto row=DisplayController.getScanline(y);if(!row||(reinterpret_cast<uintptr_t>(row)&3)){BXRUN_BOOT_LOG("ERROR: invalid VGA scanline");return;}}
#endif
 BXRUN_BOOT_LOG("VGA: ready");
 initRoad();initGraphics();race.screen=TITLE;beginRaster();renderTitle();endRaster();
 BXRUN_BOOT_LOG("First image: ready; starting keyboard");
 PS2.begin(PS2Preset::KeyboardPort0,KbdMode::GenerateVirtualKeys);
 BXRUN_BOOT_LOG("Keyboard: ready; reading preferences");
 prefs.begin("bxrun4",false);loadRecord();
 BXRUN_BOOT_LOG("SD: loading optional hall (check sd_config.h wiring)");loadHall();
 BXRUN_BOOT_LOG(hallMounted?"SD: ready":"SD: unavailable or pins rejected; continuing without SD");
 gameReady=true;BXRUN_BOOT_LOG("Game: ready");
}
void loopGame(){
 if(!gameReady){
#if defined(__XTENSA__) && defined(ARDUINO_ARCH_ESP32)
 delay(20);
#endif
 return;
 }
 static uint32_t lastFrame=0,fpsSince=0;
 static int fpsFrames=0;
 static float accumulator=0;
 static int previousScreen=-1,previousChoice=-1,previousCar=-1;
 static uint32_t previousRevision=0;
 static bool previousHelp=false,previousPause=false,previousLite=false,previousMute=false,previousWide=false;
 uint32_t now=millis();if(now-lastFrame<(wideView?33u:25u))return;float elapsed=std::min(.1f,(now-lastFrame)/1000.0f);lastFrame=now;++frameCount;kb=PS2.keyboard();Input input=controls();
 if(!helpOpen&&race.screen==TITLE_EXIT){titleExitTime+=elapsed;if(frameCount%4==0)tone(titleExitTime<INTRO_LIFT_TIME?85:120+int((titleExitTime-INTRO_LIFT_TIME)*240),30,titleExitTime<INTRO_LIFT_TIME?3:8);if(titleExitTime>=INTRO_LIFT_TIME+INTRO_DRIVE_TIME)race.screen=CAR_SELECT;}
 if(!helpOpen&&(race.screen==RACING||race.screen==COUNTDOWN)){accumulator+=elapsed;int steps=0;while(accumulator>=1.0f/60&&steps<6){updateRace(1.0f/60,input);accumulator-=1.0f/60;++steps;}updateSound(elapsed);}else accumulator=0;
 bool active=!helpOpen&&((!race.paused&&(race.screen==RACING||race.screen==COUNTDOWN))||race.screen==TITLE_EXIT);
 bool changed=previousRevision!=menuRevision||previousScreen!=int(race.screen)||previousHelp!=helpOpen||previousPause!=race.paused||previousLite!=liteMode||previousMute!=race.mute||previousWide!=wideView||previousChoice!=stageChoice||previousCar!=carChoice;
 if(changed)refreshMargins=true;
 if(active&&changed){fpsSince=now;fpsFrames=0;}
 if(active||changed){beginRaster();if(helpOpen)renderHelp();else renderGame(input);endRaster();uint32_t end=millis();++fpsFrames;if(end-fpsSince>=1000){measuredFPS=int(fpsFrames*1000u/(end-fpsSince));fpsFrames=0;fpsSince=end;}}
 previousRevision=menuRevision;previousScreen=int(race.screen);previousHelp=helpOpen;previousPause=race.paused;previousLite=liteMode;previousMute=race.mute;previousWide=wideView;previousChoice=stageChoice;previousCar=carChoice;
}
