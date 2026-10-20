#pragma once
// Reference road gearsets, not competition optional gearboxes.
// See FONTES_CAIXAS.txt: ratios, final drives, rolling circumference and provenance.
struct Gearbox {uint8_t count;float ratio[5],finalDrive,circumference;int idle,upRPM,redline;float torque,mass;};
const Gearbox GEARBOX[CAR_COUNT]={
 {5,{38.0f/13,37.0f/20,32.0f/25,31.0f/32,28.0f/37},62.0f/14,1.853f,900,6400,7200,177,1070},
 {4,{21.5f/4.125f,10.96f/4.125f,7.37f/4.125f,5.43f/4.125f,0},4.125f,1.8f,850,5600,6200,39,600},
 {5,{41.0f/12,38.0f/21,41.0f/32,39.0f/40,33.0f/43},59.0f/13,1.90f,850,5300,6250,118,1158},
 {5,{38.0f/13,33.0f/17,33.0f/25,32.0f/33,28.0f/37},35.0f/8,2.052f,900,5900,6500,233,1450},
 {5,{39.0f/12,41.0f/23,37.0f/31,35.0f/40,33.0f/47},69.0f/16,1.92f,850,5900,6600,267,1470}
};
float kmhPer1000(int car,int gear){const Gearbox&g=GEARBOX[car];gear=std::max(1,std::min(int(g.count),gear));return 60*g.circumference/(g.ratio[gear-1]*g.finalDrive);}
float wheelRPM(int gear){return race.speed*1000/kmhPer1000(race.car,gear);}
float engineRPM(){return std::max(float(GEARBOX[race.car].idle),wheelRPM(race.gear));}
bool changeGear(int direction,bool manual){const Gearbox&g=GEARBOX[race.car];if(manual)race.automatic=false;int next=race.gear+(direction>0?1:-1);if(race.shiftTime>0||next<1||next>g.count)return false;if(direction<0&&wheelRPM(next)>g.redline){announce("REDUCAO RECUSADA: RPM DEMASIADO ALTAS",1);return false;}race.gear=next;race.shiftTime=.20f;race.rpm=engineRPM();return true;}
void automaticGear(const Input&in){if(!race.automatic||race.shiftTime>0)return;const Gearbox&g=GEARBOX[race.car];float rpm=engineRPM();if(in.gas&&rpm>=g.upRPM&&race.gear<g.count)changeGear(1,false);else if(race.gear>1&&(rpm<1700||(in.gas&&rpm<2600&&wheelRPM(race.gear-1)<g.upRPM*.80f)))changeGear(-1,false);}
float driveAcceleration(){const Gearbox&g=GEARBOX[race.car];float rpm=engineRPM();if(rpm>=g.redline)return -18;float curve=rpm<4000?.58f+.42f*rpm/4000:1-.16f*(rpm-4000)/std::max(1,g.redline-4000);float force=g.torque*curve*g.ratio[race.gear-1]*g.finalDrive*.86f/(g.circumference/6.2831853f);float ratio=race.speed/CAR_MAX[race.car];return std::max(.2f,force/g.mass*3.6f*(1-.65f*ratio*ratio));}
