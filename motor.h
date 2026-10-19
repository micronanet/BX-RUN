#pragma once
#include "tipos.h"
#include <Preferences.h>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <algorithm>
using namespace fabgl;
VGAController DisplayController;Canvas canvas(&DisplayController);PS2Controller PS2;SoundGenerator soundGenerator;Preferences prefs;Keyboard*kb=nullptr;
bool wideView=false;bool refreshMargins=true;int stageChoice=0,carChoice=0;bool mapDirty=true;
int viewLeft(){return wideView?0:64;}int viewRight(){return wideView?639:575;}float viewScale(){return wideView?1.0f:.8f;}
Race race={};Record record={};float roadCentre[ROAD_SAMPLES],roadHeight[ROAD_SAMPLES];RoadRow roadRows[200];int horizon=72;uint32_t frameCount=0;char announcement[64]="";uint32_t soundClock=0;int musicBeat=0;float musicTime=0;
float limit(float x,float lo,float hi){return x<lo?lo:x>hi?hi:x;}
uint32_t rng(){race.seed=race.seed*1664525u+1013904223u;return race.seed;}
int randInt(int n){return n?int(rng()%unsigned(n)):0;}
void tone(int hz,int ms,int volume=12){if(!race.mute)soundGenerator.playSound(SquareWaveformGenerator(),hz,ms,volume);}
void announce(const char*msg,float seconds=2.0f){snprintf(announcement,sizeof(announcement),"%s",msg);race.messageTime=seconds;}
uint32_t recordCheck(const Record&r){return r.magic^r.score^r.bestMillis^0x42a19cb5u;}
const char*recordKey(){static char name[12];snprintf(name,sizeof(name),"car%d",carChoice);return name;}
void loadRecord(){Record r={};if(prefs.getBytesLength(recordKey())==sizeof(r)&&prefs.getBytes(recordKey(),&r,sizeof(r))==sizeof(r)&&r.magic==0x42585233&&r.check==recordCheck(r)&&r.score<100000000u&&r.bestMillis<3600000u)record=r;else record={0x42585233,0,0,0};}
void saveRecord(bool won){bool changed=false;if(uint32_t(race.score)>record.score){record.score=race.score;changed=true;}uint32_t t=uint32_t(race.elapsed*1000);if(won&&race.startStage==0&&(!record.bestMillis||t<record.bestMillis)){record.bestMillis=t;changed=true;}if(changed){record.magic=0x42585233;record.check=recordCheck(record);prefs.putBytes(recordKey(),&record,sizeof(record));}}
int stageAt(float z){return std::max(0,std::min(STAGES-1,int(z/STAGE_LENGTH)));}
int currentStage(){return stageAt(race.z);}
void initRoad(){for(int i=0;i<ROAD_SAMPLES;++i){float z=i*32.0f;roadCentre[i]=2400*sinf(z*.0017f)+1100*sinf(z*.0031f+.8f);float mountain=limit((z-6000)/400,0,1)*(1-limit((z-12400)/400,0,1));roadHeight[i]=(45+55*mountain)*sinf(z*.0018f)+24*sinf(z*.0038f+.5f);}}
float roadValue(float z,const float*table){float n=limit(z/32.0f,0,ROAD_SAMPLES-1.001f);int i=int(n);return table[i]+(table[i+1]-table[i])*(n-i);}
float centreAt(float z){return roadValue(z,roadCentre);}
float heightAt(float z){return roadValue(z,roadHeight);}
float tangentAt(float z){return (centreAt(z+16)-centreAt(z-16))/32;}
float curvatureAt(float z){return (tangentAt(z+24)-tangentAt(z-24))/48;}
Projection project(float distance,float lane=0){distance=std::max(42.0f,distance);float w=24000.0f*viewScale()/distance,p=w/900.0f;float bend=centreAt(race.z+distance)-centreAt(race.z)-tangentAt(race.z)*distance;float grade=(heightAt(race.z+16)-heightAt(race.z-16))/32;
 float relief=heightAt(race.z+distance)-heightAt(race.z)-grade*distance;return {320-race.x*w*.90f+bend*p+lane*w,horizon+8000/distance-8-relief*p*.35f,w,distance};}
void startRace(){bool mute=race.mute;race=Race{};race.mute=mute;race.seed=0xb016a1u;race.screen=COUNTDOWN;race.countdown=3;race.time=carChoice==1?55:45;race.car=carChoice;race.startStage=stageChoice;race.z=stageChoice*STAGE_LENGTH;while(race.checkpoint<CHECKPOINT_COUNT&&CHECKPOINTS[race.checkpoint]<=race.z)++race.checkpoint;refreshMargins=true;race.automatic=true;race.immune=4;musicBeat=0;musicTime=0;announcement[0]=0;
 for(int i=0;i<TRAFFIC_COUNT;++i)race.traffic[i]={(i<6+2*stageChoice?race.z+420.0f+i*430.0f:race.z+12000+i*1000.0f),(i%3-1)*.64f,44.0f+float(i%4)*6,uint8_t(i%4),false};}
void endRace(bool won){if(won){race.score+=int(race.time)*100;race.screen=FINISHED;announce("CHEGASTE A META!",5);tone(1046,220);}else{race.screen=GAMEOVER;announce("TEMPO ESGOTADO",5);tone(110,300);}saveRecord(won);}
void crashCar(){if(race.immune>0)return;race.speed*=.36f;race.crash=1.1f;race.immune=2.5f;announce("COLISAO!",1);tone(85,130,22);}
void updateRace(float dt,const Input&in){dt=limit(dt,0,.1f);if(race.paused)return;
 if(race.screen==COUNTDOWN){float old=race.countdown;race.countdown-=dt;if(int(ceilf(old))!=int(ceilf(race.countdown)))tone(race.countdown<=0?1046:523,110);if(race.countdown<=0){race.screen=RACING;announce("VAMOS!",1.2f);}return;}
 if(race.screen!=RACING)return;race.elapsed+=dt;race.time=std::max(0.0f,race.time-dt);race.immune=std::max(0.0f,race.immune-dt);race.crash=std::max(0.0f,race.crash-dt);race.messageTime=std::max(0.0f,race.messageTime-dt);
 if(race.automatic){if(!race.high&&race.speed>CAR_LOW[race.car]*.86f)race.high=true;else if(race.high&&race.speed<CAR_LOW[race.car]*.63f)race.high=false;}
 float cap=float(race.high?CAR_MAX[race.car]:CAR_LOW[race.car]),accel=race.high?(race.car==1?48.0f:66.0f):105.0f;
 if(race.crash>0)race.speed-=65*dt;else if(in.brake)race.speed-=150*dt;else if(in.gas)race.speed+=accel*dt;else race.speed-=24*dt;
 race.speed=limit(race.speed,0,cap);float steer=(in.right?1.0f:0)-(in.left?1.0f:0);if(race.crash<=0){race.x+=steer*(.30f+race.speed/115.0f)*dt*(race.speed>4?1:0);float velocity=race.speed*.56f;race.x-=curvatureAt(race.z)*velocity*velocity*.00265f*dt;}
 race.x=limit(race.x,-2.1f,2.1f);if(fabsf(race.x)>1.02f){race.speed=std::max(0.0f,race.speed-100*dt);race.offroad+=dt;}else race.offroad=0;
 float oldZ=race.z;race.z+=race.speed*.56f*dt;race.score+=int(race.z/10)-int(oldZ/10);
 for(int i=0;i<6+2*currentStage();++i){Traffic&t=race.traffic[i];if(t.z-race.z>3500)t.z=race.z+700+(i%3)*260;float before=t.z-oldZ-65;t.z+=t.speed*dt;float gap=t.z-race.z-65;if(fabsf(gap)<14&&fabsf(race.x-t.lane)<(race.car==1?.21f:.25f)&&race.speed>35)crashCar();if(!t.passed&&before>=0&&gap<0){t.passed=true;++race.passed;race.score+=250;}
 if(gap< -150){t.z=race.z+1500+randInt(650);t.lane=(randInt(3)-1)*.64f;t.type=randInt(4);t.passed=false;}}
 while(race.checkpoint<CHECKPOINT_COUNT&&race.z>=CHECKPOINTS[race.checkpoint]){float crossed=CHECKPOINTS[race.checkpoint++];int bonus=race.car==1?33:25;race.time+=bonus;race.score+=1000;if(int(crossed)%6400==0){char label[64];snprintf(label,sizeof(label),"ETAPA %d / %s / +%d SEG",currentStage()+1,STAGE_NAMES[currentStage()],bonus);announce(label,3);}else{char label[64];snprintf(label,sizeof(label),"CHECKPOINT! +%d SEGUNDOS",bonus);announce(label,2.5f);}tone(880,180,18);}
 if(race.z>=FINISH_Z){race.z=FINISH_Z;endRace(true);}else if(race.time<=0)endRace(false);}
const int melody[32]={330,392,440,523,494,440,392,330,294,349,392,440,392,349,294,262,330,392,523,659,587,523,494,440,392,440,494,587,523,440,392,330};
void updateSound(float dt){if(race.paused||race.mute||race.screen!=RACING)return;musicTime+=dt;if(musicTime>=.22f){musicTime-=.22f;tone(melody[musicBeat++%32],95,7);}if(frameCount%5==0&&race.speed>15)tone(85+int(race.speed)*2+(race.high?0:60),28,5);}
