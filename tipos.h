#pragma once
#include <fabgl.h>
#include <cstdint>
const int SCREEN_W=640,SCREEN_H=200,TRAFFIC_COUNT=24,MAX_ACTORS=25,SCALES=10,ROAD_SAMPLES=1260;
const int STAGES=6,CAR_COUNT=5,PROP_FRAMES=240;
const float CAMERA_GROUND=3600.0f,CAMERA_FOCAL=18000.0f,PLAYER_DEPTH=44.0f,WORLD_SPEED=.95f;
const float STAGE_LENGTH=6400.0f,FINISH_Z=38400.0f;
const int CHECKPOINT_COUNT=17;
const float CHECKPOINTS[CHECKPOINT_COUNT]={2100,4200,6400,8500,10600,12800,14900,17000,19200,21300,23400,25600,27700,29800,32000,34100,36200};
const char*const STAGE_NAMES[STAGES]={"COSTA AZUL","ALPES","DESERTO","FLORESTA","METROPOLE","POR DO SOL"};
enum RaceScreen {TITLE,TITLE_EXIT,CAR_SELECT,NAME_ENTRY,HALL_OF_FAME,COUNTDOWN,RACING,FINISHED,GAMEOVER,DIFFICULTY_SELECT};
struct Input {bool gas,brake,left,right;};
struct Traffic {float z,lane,speed;uint8_t type;bool passed;float targetLane,changeWait;};
struct Record {uint32_t magic,score,bestMillis,check;};
struct Race {float oilTime;int lastOil,oilDirection;float lateralVelocity;int difficulty;float wheelPhase,wheelSlip,brakeSkid,understeer,spin,spinStress,spinCooldown;int spinDirection;float rpm,shiftTime;int gear;float z,speed,x,time,elapsed,crash,immune,countdown,messageTime,offroad;int score,checkpoint,passed,startStage,car;bool paused,mute,automatic;RaceScreen screen;uint32_t seed;Traffic traffic[TRAFFIC_COUNT];};
struct Projection {float x,y,w,dist;};
struct RoadRow {float x,w,dist;};
struct Actor {float distance;int frame,x,y;};

const char*const CAR_NAMES[CAR_COUNT]={"BX 16 VALVE","2CV","C3 PLURIEL","CITROEN SM","XANTIA ACTIVA 3.0 V6"};
const int CAR_MAX[CAR_COUNT]={218,115,160,220,230};
const int CAR_FIRST_FRAME[CAR_COUNT]={0,3,6,12,15};

// Specific versions; speeds are km/h, not the previous arcade limits.
const char*const CAR_ENGINE[5]={"XU9J4 / 1905 CM3 / 16V","602 CM3 / BICILINDRICO BOXER","TU3JP / 1360 CM3 / 8V","MASERATI V6 / 2670 CM3","ES9J4 / 2946 CM3 / V6 24V"};
const char*const CAR_POWER[5]={"160 CV / INJECAO / 5 MARCHAS","29 CV / AR / 4 MARCHAS","75 CV / INJECAO / 5 MARCHAS","170 CV / CARBURADORES / 5 MARCHAS","194 CV / INJECAO / 5 MARCHAS"};
const char*const CAR_SUSP[5]={"SUSPENSAO HIDROPNEUMATICA","MOLAS HORIZONTAIS INTERLIGADAS","MOLAS / FRENTE MACPHERSON","HIDROPNEUMATICA / DIRAVI","HIDRACTIVA II + ACTIVA (SC.CAR)"};
const char*const CAR_EXTRA[5]={"ALTURA CONSTANTE / ABS","TRACAO DIANTEIRA / CAPOTA DE LONA","DESCAPOTAVEL / TRACAO DIANTEIRA","DIRECAO DE ASSISTENCIA VARIAVEL","ANTI-ROLAMENTO POR COMPUTADOR"};
const char*const CAR_VERSION[5]={"16 VALVE 160 CV","2CV 6 / 602 CM3","1.4i 75 / TU3JP","2.7 V6 / 1970","3.0 V6 ACTIVA / BVM5"};
const char*const CAR_SHORT[5]={"BX","2CV","PLURIEL","SM","ACTIVA"};
