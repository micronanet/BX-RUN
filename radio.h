#pragma once
// Original compositions, compact monophonic arrangements for the FabGL square wave.
// Each beat alternates melody, bass and chord notes; no sampled audio in flash/RAM.
const int RADIO_TRACKS=4;
const char*const RADIO_NAMES[RADIO_TRACKS]={"NEON HIGHWAY / HARD ROCK","LAST SUMMER / POWER BALLAD","MIDNIGHT DRIVE / SYNTH ROCK","COASTLINE / SOFT ROCK"};
const uint16_t RADIO_LEAD[4][32]={
 {330,0,392,440,494,440,392,330,294,330,392,0,440,392,330,294,330,392,494,587,659,587,494,440,392,440,494,392,330,294,247,0},
 {392,0,440,494,587,0,494,440,392,330,294,0,330,392,440,0,494,0,587,659,587,494,440,392,440,0,392,330,294,330,392,0},
 {440,523,659,523,587,698,880,698,392,494,587,494,330,415,494,415,440,659,784,659,587,880,698,587,523,659,784,659,494,415,330,0},
 {349,440,523,0,587,523,440,392,330,392,494,587,523,0,440,392,349,440,523,698,659,587,523,440,392,440,523,392,349,330,349,0}};
const uint16_t RADIO_BASS[4][4]={{82,65,98,73},{98,73,82,65},{110,87,98,82},{87,65,73,98}};
const float RADIO_STEP[4]={.115f,.18f,.125f,.15f};
int radioTrack=0,radioTick=0;float radioClock=0;
void resetRadio(){radioTick=0;radioClock=0;}
void nextRadio(){refreshMargins=true;radioTrack=(radioTrack+1)%RADIO_TRACKS;resetRadio();char s[64];snprintf(s,sizeof(s),"RADIO %d: %s",radioTrack+1,RADIO_NAMES[radioTrack]);announce(s,3.0f);}
void playRadio(float dt){radioClock+=dt;float step=RADIO_STEP[radioTrack];if(radioClock<step)return;radioClock=fmodf(radioClock,step);int beat=(radioTick/2)%32,bar=beat/8;int hz;
 if((radioTick&1)==0)hz=RADIO_LEAD[radioTrack][beat];
 else{int root=RADIO_BASS[radioTrack][bar];hz=beat%4==3?root*3:root*(beat%2?2:1);}
 ++radioTick;if(hz)tone(hz,int(step*880),((radioTick&1)?8:5));}
