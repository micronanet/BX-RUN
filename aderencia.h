#pragma once
// Deliberately bounded arcade tyre model. These are gameplay settings, not measured car data.
const float TYRE_GRIP[CAR_COUNT]={1.60f,.46f,.90f,1.65f,2.05f};
const float SPIN_DURATION=1.35f;
const int MAX_TYRE_PARTICLES=24;
struct TyreParticle {float x,y,vx,vy,life,age;uint8_t dust;};
TyreParticle tyreParticles[MAX_TYRE_PARTICLES]={};
float particleClock=0,tyreSoundClock=0;uint32_t effectSeed=0x16b00bu;
uint32_t effectRandom(){effectSeed=effectSeed*1664525u+1013904223u;return effectSeed;}
void resetTyres(){memset(tyreParticles,0,sizeof(tyreParticles));particleClock=tyreSoundClock=0;effectSeed=0x16b00bu;}
void startSpin(float curve){if(race.car!=1&&race.car!=2)return;race.spin=SPIN_DURATION;race.spinCooldown=7;race.spinStress=0;race.spinDirection=curve>=0?-1:1;race.speed*=.72f;race.immune=std::max(race.immune,SPIN_DURATION+.3f);announce("PIAO! ALIVIA O ACELERADOR",1.8f);}
void tyreGrip(float dt,const Input&in){
 race.spinCooldown=std::max(0.0f,race.spinCooldown-dt);
 if(race.spin>0){race.spin=std::max(0.0f,race.spin-dt);race.understeer=1;race.spinStress=0;return;}
 float curve=curvatureAt(race.z),steer=float(in.right)-float(in.left);
 float demand=race.speed*race.speed*(fabsf(curve)*.003f+fabsf(steer)*.000010f);
 float target=limit((demand/(TYRE_GRIP[race.car]*DIFFICULTY_GRIP[race.difficulty])-.85f)*.85f,0,1);
 if(race.oilTime>0&&race.speed>15)target=std::max(target,.38f+.12f*race.difficulty);
 if(fabsf(race.x)>1.02f)target=std::min(.3f,target); // Recovery from the shoulder remains available.
 float previous=race.understeer;race.understeer+=(target-race.understeer)*std::min(1.0f,dt*5);
 if(previous<.35f&&race.understeer>=.35f&&race.messageTime<=0)announce("SUBVIRAGEM! ABRANDA",1.2f);
 bool small=race.car==1||race.car==2;
 if(small&&race.spinCooldown<=0&&race.crash<=0&&fabsf(race.x)<1.02f&&race.speed>CAR_MAX[race.car]*(.98f-.05f*race.difficulty)&&fabsf(curve)>.009f&&race.understeer>.68f){race.spinStress+=dt*(in.brake?2.6f:1.0f);if(race.spinStress>(1.65f-.3f*race.difficulty))startSpin(curve);}else race.spinStress=std::max(0.0f,race.spinStress-dt*2);
}
void emitTyreParticle(bool dust,bool front,int side){for(int i=0;i<MAX_TYRE_PARTICLES;++i)if(tyreParticles[i].life<=0){uint32_t r=effectRandom();float width=race.car==1?37.0f:54.0f;tyreParticles[i]={side*(width+float(r%6)),front?-16.0f:-3.0f,side*(18.0f+float((r>>4)%16)),-10.0f-float((r>>8)%12),dust?.48f:.62f,0,uint8_t(dust)};return;}}
void updateTyreEffects(float dt,const Input&in,float oldSpeed,float oldRPM){
 for(int i=0;i<MAX_TYRE_PARTICLES;++i){TyreParticle&p=tyreParticles[i];if(p.life>0){p.life=std::max(0.0f,p.life-dt);p.age+=dt;p.x+=p.vx*dt;p.y+=p.vy*dt;}}
 bool powered=in.gas&&!in.brake&&race.shiftTime<=0&&race.crash<=0&&race.spin<=0;
 float launch=powered&&race.gear<=2?limit((32-race.speed)/32,0,1)*.85f:0;
 float rapidRPM=powered&&race.gear<=2&&race.speed<90?limit((engineRPM()-oldRPM)/std::max(.001f,dt)/5000,0,.8f):0;
 float slip=std::max(launch,rapidRPM);
 race.wheelSlip+=(slip-race.wheelSlip)*std::min(1.0f,dt*(slip>race.wheelSlip?12:5));
 race.brakeSkid=in.brake&&oldSpeed>65&&race.spin<=0?limit((oldSpeed-55)/90,.12f,1):0;
 race.rpm=std::min(float(GEARBOX[race.car].redline),engineRPM()+race.wheelSlip*1800);
 race.wheelPhase=fmodf(race.wheelPhase+(race.speed*.025f+race.wheelSlip*12)*dt,2.0f);
 bool dust=fabsf(race.x)>1.02f;
 bool emit=(dust&&race.speed>7)||race.wheelSlip>.18f||race.brakeSkid>.05f||race.understeer>.30f||race.spin>0;
 if(emit){particleClock+=dt;if(particleClock>=.055f){particleClock=fmodf(particleClock,.055f);bool front=race.brakeSkid>.1f||race.spin>0?(int(race.elapsed*18)&1)!=0:true;emitTyreParticle(dust,front,-1);emitTyreParticle(dust,front,1);}}else particleClock=0;
}
