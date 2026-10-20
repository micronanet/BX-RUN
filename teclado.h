#pragma once
bool key(VirtualKey k){if(!kb)return false;if(k>=VK_a&&k<=VK_z)return kb->isVKDown(k)||kb->isVKDown(static_cast<VirtualKey>(VK_A+k-VK_a));if(k>=VK_0&&k<=VK_9)return kb->isVKDown(k)||kb->isVKDown(static_cast<VirtualKey>(VK_KP_0+k-VK_0));if(k==VK_RETURN)return kb->isVKDown(VK_RETURN)||kb->isVKDown(VK_KP_ENTER);return kb->isVKDown(k);}
VirtualKey watched[64];bool wasDown[64]={};int watchCount=0;bool helpOpen=false;
bool pressed(VirtualKey k){int i=0;while(i<watchCount&&watched[i]!=k)++i;if(i==watchCount){if(watchCount==64)return false;watched[watchCount++]=k;}bool down=key(k),out=down&&!wasDown[i];wasDown[i]=down;return out;}
bool nameFresh=true;
void enterName(){race.screen=NAME_ENTRY;nameFresh=true;}
void editPilot(char c){if(nameFresh){pilotName[0]=0;nameFresh=false;}size_t n=strlen(pilotName);if(n<12){pilotName[n]=c;pilotName[n+1]=0;++menuRevision;}}
Input controls(){bool enter=pressed(VK_RETURN),esc=pressed(VK_ESCAPE);
 // Text entry owns all letter keys; A/M/P/I must be usable in a name.
 bool letters[26],numbers[10];for(int i=0;i<26;++i)letters[i]=pressed(static_cast<VirtualKey>(VK_a+i));for(int i=0;i<10;++i)numbers[i]=pressed(static_cast<VirtualKey>(VK_0+i));bool space=pressed(VK_SPACE),back=pressed(VK_BACKSPACE),hyphen=pressed(VK_MINUS);
 bool left=pressed(VK_LEFT),right=pressed(VK_RIGHT),up=pressed(VK_UP),down=pressed(VK_DOWN);
 if(race.screen==NAME_ENTRY){for(int i=0;i<26;++i)if(letters[i])editPilot(char('A'+i));for(int i=0;i<10;++i)if(numbers[i])editPilot(char('0'+i));if(space)editPilot(' ');if(hyphen)editPilot('-');if(back){nameFresh=false;size_t n=strlen(pilotName);if(n)pilotName[n-1]=0;++menuRevision;}if(esc)race.screen=DIFFICULTY_SELECT;else if(enter){if(!validPilot(pilotName))snprintf(pilotName,sizeof(pilotName),"PILOTO");startRace();}return {};}
 bool pause=letters['p'-'a'],mute=letters['m'-'a'],gearDown=letters['q'-'a'],gearUp=letters['w'-'a'],automatic=letters[0],help=letters['i'-'a'],detail=letters['f'-'a'],view=letters['v'-'a'];
 if(view){wideView=!wideView;refreshMargins=true;}if(detail)liteMode=!liteMode;if(mute)race.mute=!race.mute;if(helpOpen){if(help||esc||enter)helpOpen=false;return {}; }if(help){helpOpen=true;return {};}
 if(letters['n'-'a']&&(race.screen==RACING||race.screen==COUNTDOWN))nextRadio();
 if(race.screen==TITLE_EXIT){if(esc)race.screen=TITLE;return {};}
 if(race.screen==DIFFICULTY_SELECT){if(left||right||up||down){difficultyChoice=(difficultyChoice+(right||down?1:4))%5;++menuRevision;}if(enter)enterName();if(esc)race.screen=CAR_SELECT;}
 else if(race.screen==CAR_SELECT){if(left||right||up||down){carChoice=(carChoice+(right||down?1:CAR_COUNT-1))%CAR_COUNT;loadRecord();}if(enter)race.screen=DIFFICULTY_SELECT;if(esc)race.screen=TITLE;}
 else if(race.screen==TITLE){if(left||right)stageChoice=(stageChoice+(right?1:STAGES-1))%STAGES;if(enter){titleExitTime=0;race.screen=TITLE_EXIT;tone(120,100);}if(letters['h'-'a'])race.screen=HALL_OF_FAME;}
 else if(race.screen==HALL_OF_FAME){if(left||right){carChoice=(carChoice+(right?1:CAR_COUNT-1))%CAR_COUNT;loadRecord();}if(up||down)stageChoice=(stageChoice+(down?1:STAGES-1))%STAGES;if(enter){loadRecord();race.screen=CAR_SELECT;}if(esc)race.screen=TITLE;}
 else if(race.screen==GAMEOVER||race.screen==FINISHED){if(enter)race.screen=CAR_SELECT;if(esc)race.screen=TITLE;}
 else{if(esc)race.screen=TITLE;if(pause)race.paused=!race.paused;if(!race.paused){if(gearDown!=gearUp)changeGear(gearUp?1:-1,true);if(automatic)race.automatic=true;}}
 return {key(VK_UP)||key(VK_SPACE),key(VK_DOWN),key(VK_LEFT),key(VK_RIGHT)};}
void renderHelp(){hideActors();rect(0,0,639,199,192);centred(13,"BX RUN / GUIA DE CONDUCAO",rgb(3,3,1));const char*lines[]={"CIMA ou ESPACO: acelerar / BAIXO: travar","ESQUERDA / DIREITA: virar","A caixa e automatica ao iniciar a corrida.","Q: reduzir / W: subir mudanca / A: voltar a automatico","P: pausa M: som F: detalhe V: janela/panoramico ESC: menu","N: mudar radio / 4 temas originais ao estilo dos anos 80","Tempo de checkpoint ajustado a velocidade de cada carro.","Evita o oleo: perda de aderencia e risco de piao.","Hall of Fame no SD: nome, carro, partida, pontos e tempo.","Titulo: ENTER arranca o BX / H consulta a classificacao.","I, ENTER ou ESC: voltar"};for(int i=0;i<11;++i)centred(38+i*13,lines[i],i==10?rgb(1,3,3):255);}
