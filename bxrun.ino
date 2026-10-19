// BX RUN - keyboard-only coastal arcade racer for ESP32 + FabGL.
// All custom types and functions live in headers: safe with INO prototype generation.
#include "tipos.h"
#include "motor.h"
#include "ecra.h"
#include "teclado.h"
void setup(){PS2.begin(PS2Preset::KeyboardPort0,KbdMode::GenerateVirtualKeys);DisplayController.begin();DisplayController.setResolution(VGA_640x200_70Hz);prefs.begin("bxrun3",false);initRoad();loadRecord();initGraphics();race.screen=TITLE;beginRaster();renderTitle();endRaster();}
void loop(){static uint32_t lastFrame=0,fpsSince=0;static int fpsFrames=0;static float accumulator=0;static int previousScreen=-1,previousChoice=-1,previousCar=-1;static bool previousHelp=false,previousPause=false,previousLite=false,previousMute=false,previousWide=false;
 uint32_t now=millis();if(now-lastFrame<(wideView?33u:25u))return;float elapsed=std::min(.1f,(now-lastFrame)/1000.0f);lastFrame=now;++frameCount;kb=PS2.keyboard();Input input=controls();
 if(!helpOpen&&(race.screen==RACING||race.screen==COUNTDOWN)){accumulator+=elapsed;int steps=0;while(accumulator>=1.0f/60&&steps<6){updateRace(1.0f/60,input);accumulator-=1.0f/60;++steps;}updateSound(elapsed);}else accumulator=0;
 bool active=!helpOpen&&!race.paused&&(race.screen==RACING||race.screen==COUNTDOWN);
 bool changed=previousScreen!=int(race.screen)||previousHelp!=helpOpen||previousPause!=race.paused||previousLite!=liteMode||previousMute!=race.mute||previousWide!=wideView||previousChoice!=stageChoice||previousCar!=carChoice;
 if(changed)refreshMargins=true;
 if(active&&changed){fpsSince=now;fpsFrames=0;}
 if(active||changed){beginRaster();if(helpOpen)renderHelp();else renderGame(input);endRaster();uint32_t end=millis();++fpsFrames;if(end-fpsSince>=1000){measuredFPS=int(fpsFrames*1000u/(end-fpsSince));fpsFrames=0;fpsSince=end;}}
 previousScreen=int(race.screen);previousHelp=helpOpen;previousPause=race.paused;previousLite=liteMode;previousMute=race.mute;previousWide=wideView;previousChoice=stageChoice;previousCar=carChoice;
}
