#pragma once
#include "sd_config.h"
#include <cstdlib>
// SD is optional in desktop emulators. Native ESP32 always enables it.
#if defined(ARDUINO_ARCH_ESP32) || defined(BXRUN_SD_TEST)
#include <SPI.h>
#include <SD.h>
#define BXRUN_HAS_SD 1
SPIClass hallSPI(HSPI);
#endif
struct HallEntry {char name[13];uint32_t score,millis;uint8_t car,start,won;uint8_t difficulty=2;};
HallEntry hall[CAR_COUNT*STAGES][10]={};uint8_t hallCount[CAR_COUNT*STAGES]={};
char pilotName[13]="PILOTO";uint32_t menuRevision=0;float titleExitTime=0;
bool hallMounted=false,hallSaved=false,hallSubmitted=false,hallWriteError=false;
int hallGroup(int car,int start){return car*STAGES+start;}
bool validPilot(const char*s){size_t n=strlen(s);if(!n||n>12)return false;bool letter=false;for(size_t i=0;i<n;++i){char c=s[i];if(!((c>='A'&&c<='Z')||(c>='0'&&c<='9')||c==' '||c=='-'))return false;if(c!=' '&&c!='-')letter=true;}return letter;}
void addHall(const HallEntry&e){int g=hallGroup(e.car,e.start),n=hallCount[g],at=0;while(at<n){const HallEntry&r=hall[g][at];if(e.score>r.score||(e.score==r.score&&(e.won>r.won||(e.won==r.won&&e.millis<r.millis))))break;++at;}if(at>=10)return;for(int i=std::min(n,9);i>at;--i)hall[g][i]=hall[g][i-1];hall[g][at]=e;hallCount[g]=std::min(n+1,10);}
uint32_t hallHash(const char*s){uint32_t h=2166136261u;while(*s)h=(h^uint8_t(*s++))*16777619u;return h;}
void hallPayload(const HallEntry&e,char*b,size_t size){snprintf(b,size,"2;%s;%u;%u;%lu;%lu;%u;%u",e.name,unsigned(e.car),unsigned(e.start),(unsigned long)e.score,(unsigned long)e.millis,unsigned(e.won),unsigned(e.difficulty));}
bool parseHall(char*line,HallEntry&e){char*last=strrchr(line,';');if(!last)return false;*last++=0;char*end=nullptr;unsigned long check=strtoul(last,&end,16);if(!*last||*end||strlen(last)!=8||uint32_t(check)!=hallHash(line))return false;
 unsigned ver,car,start,won;unsigned long score,ms;int used=0;char name[13]={};if(sscanf(line,"%u;%12[^;];%u;%u;%lu;%lu;%u%n",&ver,name,&car,&start,&score,&ms,&won,&used)!=7||(ver!=1&&ver!=2)||car>=CAR_COUNT||start>=STAGES||won>1||score>=100000000ul||ms>3600000ul||!validPilot(name))return false;unsigned difficulty=2;if(ver==1){if(line[used])return false;}else{int extra=0;if(sscanf(line+used,";%u%n",&difficulty,&extra)!=1||line[used+extra]||difficulty>4)return false;}e=HallEntry{};e.difficulty=difficulty;snprintf(e.name,sizeof(e.name),"%s",name);e.score=score;e.millis=ms;e.car=car;e.start=start;e.won=won;return true;}
bool mountHall(){
#ifdef BXRUN_HAS_SD
 if(!BXRUN_SD_ENABLED)return false;
 // Reject reserved flash/PSRAM pins before SPI changes their function.
 const int pins[]={BXRUN_SD_SCK,BXRUN_SD_MISO,BXRUN_SD_MOSI,BXRUN_SD_CS};
 for(int pin:pins)if((pin>=6&&pin<=11)||pin==16||pin==17)return false;
 if(hallMounted)return true;hallSPI.begin(BXRUN_SD_SCK,BXRUN_SD_MISO,BXRUN_SD_MOSI,BXRUN_SD_CS);hallMounted=SD.begin(BXRUN_SD_CS,hallSPI,4000000);return hallMounted;
#else
 return false;
#endif
}
void loadHall(){
#ifdef BXRUN_HAS_SD
 if(!mountHall())return;File f=SD.open("/BX_RUN/hall.csv",FILE_READ);if(!f)return;char line[128];size_t n=0;bool overflow=false;unsigned bytes=0;while(f.available()){
#if defined(__XTENSA__) && defined(ARDUINO_ARCH_ESP32)
 if((++bytes&1023)==0)delay(1);
#else
 (void)bytes;
#endif
 int ch=f.read();if(ch<0)break;if(ch=='\n'){line[n]=0;HallEntry e;if(!overflow&&parseHall(line,e))addHall(e);n=0;overflow=false;}else if(ch!='\r'){if(n<sizeof(line)-1)line[n++]=char(ch);else overflow=true;}}f.close(); // Truncated final lines are deliberately ignored.
#endif
}
void submitHall(bool won){if(hallSubmitted)return;hallSubmitted=true;HallEntry e={};snprintf(e.name,sizeof(e.name),"%s",validPilot(pilotName)?pilotName:"PILOTO");e.car=race.car;e.start=race.startStage;e.won=won;e.difficulty=race.difficulty;e.score=std::max(0,race.score);e.millis=uint32_t(race.elapsed*1000);addHall(e);hallSaved=false;hallWriteError=false;
#ifdef BXRUN_HAS_SD
 if(!mountHall())return;if(!SD.exists("/BX_RUN")&&!SD.mkdir("/BX_RUN")){hallWriteError=true;return;}
 File f=SD.open("/BX_RUN/hall.csv",FILE_APPEND);if(!f){hallWriteError=true;return;}char payload[100],row[120];hallPayload(e,payload,sizeof(payload));int n=snprintf(row,sizeof(row),"\n%s;%08lx\n",payload,(unsigned long)hallHash(payload)); // Leading newline isolates an interrupted previous write.
 size_t wrote=f.write((const uint8_t*)row,n);f.flush();f.close();hallSaved=wrote==size_t(n);
 if(hallSaved){File verify=SD.open("/BX_RUN/hall.csv",FILE_READ);hallSaved=verify&&verify.size()>=size_t(n)&&verify.seek(verify.size()-n);for(int i=0;hallSaved&&i<n;++i)if(verify.read()!=uint8_t(row[i]))hallSaved=false;verify.close();}hallWriteError=!hallSaved;
#endif
}
