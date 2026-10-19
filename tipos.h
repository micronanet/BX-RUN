#pragma once
#include <fabgl.h>
#include <cstdint>
const int SCREEN_W=640,SCREEN_H=200,TRAFFIC_COUNT=16,MAX_ACTORS=25,SCALES=10,ROAD_SAMPLES=1260;
const int STAGES=6,CAR_COUNT=5,PROP_FRAMES=240;
const float STAGE_LENGTH=6400.0f,FINISH_Z=38400.0f;
const int CHECKPOINT_COUNT=17;
const float CHECKPOINTS[CHECKPOINT_COUNT]={2100,4200,6400,8500,10600,12800,14900,17000,19200,21300,23400,25600,27700,29800,32000,34100,36200};
const char*const STAGE_NAMES[STAGES]={"COSTA AZUL","ALPES","DESERTO","FLORESTA","METROPOLE","POR DO SOL"};
enum RaceScreen {TITLE,CAR_SELECT,COUNTDOWN,RACING,FINISHED,GAMEOVER};
struct Input {bool gas,brake,left,right;};
struct Traffic {float z,lane,speed;uint8_t type;bool passed;};
struct Record {uint32_t magic,score,bestMillis,check;};
struct Race {float z,speed,x,time,elapsed,crash,immune,countdown,messageTime,offroad;int score,checkpoint,passed,startStage,car;bool paused,mute,automatic,high;RaceScreen screen;uint32_t seed;Traffic traffic[TRAFFIC_COUNT];};
struct Projection {float x,y,w,dist;};
struct RoadRow {float x,w,dist;};
struct Actor {float distance;int frame,x,y;};

const char*const CAR_NAMES[CAR_COUNT]={"BX 16 VALVE","2CV","C3 PLURIEL","CITROEN SM","XANTIA ACTIVA 3.0 V6"};
const int CAR_LOW[CAR_COUNT]={150,115,140,150,155};
const int CAR_MAX[CAR_COUNT]={280,185,235,275,280};
const int CAR_FIRST_FRAME[CAR_COUNT]={0,3,6,12,15};
