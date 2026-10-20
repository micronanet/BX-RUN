#pragma once
// Deterministic, spaced hazards: physics and rendering use the same coordinates.
float oilSpacing(){return 2400.0f-race.difficulty*280.0f;}
uint32_t oilHash(int index){uint32_t n=uint32_t(index+1)*2654435761u;n^=n>>13;return n;}
float oilZ(int index){return (index+1)*oilSpacing()+float(oilHash(index)%300);}
float oilLane(int index){return (int(oilHash(index)%3)-1)*.64f;}
void checkOil(float oldZ){race.oilTime=std::max(0.0f,race.oilTime);int cell=std::max(0,int((race.z+PLAYER_DEPTH)/oilSpacing())-2);
 for(int i=cell;i<cell+3;++i){float z=oilZ(i);if(i==race.lastOil||z>=FINISH_Z-200)continue;
  if(oldZ+PLAYER_DEPTH<=z+18&&race.z+PLAYER_DEPTH>=z-18&&fabsf(race.x-oilLane(i))<.27f){race.lastOil=i;race.oilTime=1.0f+.3f*race.difficulty;race.oilDirection=(oilHash(i)&8)?1:-1;announce("OLEO! ALIVIA E CORRIGE A DIRECAO",2);if(race.difficulty>=2&&race.speed>CAR_MAX[race.car]*.8f&&fabsf(curvatureAt(race.z))>.008f&&race.spinCooldown<=0)startSpin(curvatureAt(race.z));}}
}
