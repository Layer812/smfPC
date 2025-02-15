// midi Player for Cardputer
// 2025 Layer8

#include "M5Cardputer.h"
#include <SdFat.h>
#include <MD_MIDIFile.h>
#include "M5UnitSynth.h"

M5UnitSynth synth;

#define SERIAL_RATE 31250

// SD chip select pin for SPI comms.
// Default SD chip select is the SPI SS pin (10 on Uno, 53 on Mega).
const uint8_t SD_SELECT = SS;

const uint16_t WAIT_DELAY = 2000; // ms
#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))

// The files in the tune list should be located on the SD card 
// or an error will occur opening the file and the next in the 
// list will be opened (skips errors).
const char *tuneList[] = 
{
//  "tfeo.mid",  // simplest and shortest file
  "zolt.mid",
};

SDFAT	SD;
MD_MIDIFile SMF;

// for filer.cpp
#define VERSION 0.2

#define STATNUM 4
#define STATMAX 10
#define STATCLR 0
#define STATLOAD 1
#define STATPLAY 2
#define STATALL 3

#define DISPMAX 7
#define LISTMAX 256   // max files in a directory
#define PATHMAX 256   // filename max include pathname

#define TYPE_SMF  2
#define TYPE_SDIR  1
#define TYPE_UDIR  0

#define DIRMAX 10   // directory depth max

struct fl{
  uint8_t filename[PATHMAX];
  uint8_t type;
};
#define BUFF_SIZE 2048

bool menuflag = false;
struct fl filelist[LISTMAX];
int filenum;
int dirnum = 0;
uint8_t cdir[PATHMAX] = "/";
uint8_t dirs[DIRMAX][PATHMAX] = {"/", "", "", "", ""};
int sel = 0, disp = 0;
int pcmvol;

bool playall;
bool playend;
bool playloop;
bool loopflag;
int  vol;



const char statstr[STATNUM][STATMAX] = {"         ", "loading ", "playing ", "playall  "}; 


void disptitle(int stat){
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.fillRect(0, 0, 240, 18, BLACK);
  M5Cardputer.Display.setTextColor(OLIVE);
  M5Cardputer.Display.printf("smfPC %.1f ", VERSION);
  M5Cardputer.Display.setTextColor(GREEN);
  M5Cardputer.Display.printf("%s", statstr[stat % STATNUM]);
  M5Cardputer.Display.drawLine(0,17,240,17,OLIVE);
}

void dispmenu(){
  M5Cardputer.Lcd.fillRect(20,18,200,100,BLUE);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(0, 28);
  M5.Lcd.println("   m: menu");
  M5.Lcd.println("   a: playall");
  M5.Lcd.println("   \" \": play/stop");
}

void hitkey(){
  M5Cardputer.update();
  if (M5Cardputer.Keyboard.isChange()) {
    if (M5Cardputer.Keyboard.isKeyPressed(' ')){
      playend = true;
      playall = false;
     // printf("space pressed \n");
    }
    if (M5Cardputer.Keyboard.isKeyPressed('=')){
      vol += 20;
      if(vol > 255)
        vol = 255;
     // printf("plus pressed \n");
      M5.Speaker.setVolume(vol);
    }
    if (M5Cardputer.Keyboard.isKeyPressed('-')){
      vol -= 20;
      if(vol < 0)
        vol = 0;
      //printf("minus pressed \n");
      M5.Speaker.setVolume(vol);
    }
    if (M5Cardputer.Keyboard.isKeyPressed('a')){
      playall = !playall;
      loopflag = false;
      disptitle(playall? STATALL: STATCLR);
      //printf("a pressed \n");
    }
    if (M5Cardputer.Keyboard.isKeyPressed('m')){
      menuflag = !menuflag;
      if(menuflag)
        dispmenu();
      //printf("m pressed \n");
    }
    if (M5Cardputer.Keyboard.isKeyPressed('l')){
      loopflag = !loopflag;
      //printf("p pressed \n");
    }
  }
}

bool ismdxfile(const char *fileName) {
  int i; char exttmp[5];
  exttmp[4] = 0;
  const char *ext = strrchr(fileName, '.');
  if(ext == NULL)
    return false;
  for(i = 0; i < 4; i++)
    exttmp[i] = tolower(ext[i]);

  return (strcmp(".mid", exttmp) == 0);
}

int makemdxlist() {
  int i = 0;
 // printf("ck1\n");
   //printf("ck6 %s\n", cdir);
  char fn[PATHMAX];
  File root = SD.open((const char *)cdir);
  if (!root.isDirectory()) {
    return 0;
  }
  //printf("ck7 %d\n", dirnum);
  memset(filelist, 0, sizeof(struct fl) * LISTMAX);
  if(dirnum > 0){
    strcpy((char *)filelist[0].filename, (char *)dirs[dirnum - 1]);
    filelist[0].type = TYPE_UDIR;
    i++;
  }
  File file = root.openNextFile();
  while (file) {
 //   printf("ck8 %s\n", file.name());
    if (i == LISTMAX)
      break;
//    sprintf((char *)filelist[i].filename, "/%.*s", PATHMAX - 2, file.name());
//    printf("ck4 %d %s\n", i, filelist[i]);
    file.getName(fn, PATHMAX);
    if (ismdxfile((const char *)fn)){
      memcpy(filelist[i].filename, fn, PATHMAX);
      filelist[i].type = TYPE_SMF;
      i++;
    }
    if(file.isDirectory()){
      memcpy(filelist[i].filename, fn, PATHMAX);
      filelist[i].type = TYPE_SDIR;
      i++;
    }
//    printf("ck5 %d %s\n", i, filelist[i]);
    file = root.openNextFile();
//    printf("ck6\n");
  }
  return i;
}

void dispfiles(int disp, int sel, bool start){
  int i;
  menuflag = false;
  M5Cardputer.Display.fillRect(0,18, 240,135, BLACK);
  M5Cardputer.Display.setCursor(0, 18);

//  M5Cardputer.Display.fillScreen(BLACK);
  for (i = 0; i < DISPMAX; i++){
    if(filelist[i + disp].type == TYPE_SDIR || filelist[i + disp].type == TYPE_UDIR)
      M5Cardputer.Display.setTextColor(LIGHTGREY);  
    if((i + disp)== sel)
      M5Cardputer.Display.setTextColor(BLACK, start? YELLOW: GREEN);
    M5Cardputer.Display.printf("%.*s\n", 20, filelist[i + disp].filename);
    M5Cardputer.Display.setTextColor(WHITE);
  }
}


int selectfile(){
    dispfiles(disp, sel, false);
    if(playall){
      if(sel < (filenum - 1)){
        sel++;
        disp++;
        dispfiles(disp, sel, true);
        return sel;
      }else{
        playall = false;
        sel = disp = 0;
        dispfiles(disp, sel, false);
      }
    }
    while(1){
      M5Cardputer.update();
      if (M5Cardputer.Keyboard.isChange()) {
          if (M5Cardputer.Keyboard.isKeyPressed(';')){
            if(sel != 0){
              if(sel == disp)
                disp--;
              sel--;
            }
            dispfiles(disp, sel, false);
          }
          if (M5Cardputer.Keyboard.isKeyPressed('.')){
            if(sel < (DISPMAX - 1)){
              sel++;
            }else{
              if(sel < (filenum - 1)){
                sel++;
                disp++;
              }
            }
            dispfiles(disp, sel, false);
          }
          if (M5Cardputer.Keyboard.isKeyPressed(',')){
            if(sel - DISPMAX > 0){
              sel -= DISPMAX;
              disp = sel;
            }else{
              sel = disp = 0;
            }
            dispfiles(disp, sel, false);
          }
          if (M5Cardputer.Keyboard.isKeyPressed('/')){
             if(sel < filenum - DISPMAX){
              sel += DISPMAX;
              disp += DISPMAX;
            }
            dispfiles(disp, sel, false);
          }
          if (M5Cardputer.Keyboard.isKeyPressed(' ')){
            dispfiles(disp, sel, true);
            return sel;
          }
          if (M5Cardputer.Keyboard.isKeyPressed('=')){
            vol += 20;
            if(vol > 255)
              vol = 255;
            M5.Speaker.setVolume(vol);
          }
          if (M5Cardputer.Keyboard.isKeyPressed('-')){
            vol -= 20;
            if(vol < 0)
              vol = 0;
            M5.Speaker.setVolume(vol);
          }       
          if (M5Cardputer.Keyboard.isKeyPressed('m')){
            dispmenu();
            //printf("m pressed \n");
          }
          if (M5Cardputer.Keyboard.isKeyPressed('a')){
            if(filelist[sel].type != TYPE_SMF)
              continue;
            playall = true;
            loopflag = false;
            disptitle(playall? STATALL: STATCLR);
            return sel;
          }
      }
    }
}

bool cnvfile(struct fl *srct, uint8_t *dst){
  bool ret = true;
  //printf("ck4 %d\n", srct->type);
  switch(srct->type){
    case TYPE_SMF:
      sprintf((char *)dst, "%s%s%s", cdir, (dirnum > 0)?"/":"",(char *)srct->filename);
//      printf("chk spri1: %s, %d\n", (char *)dst, strlen((char *)dst));
      break;
    case TYPE_SDIR:
      if(dirnum == DIRMAX-1)
        break;
      dirnum++;   
      sprintf((char *)dirs[dirnum], "%s%s%s", (char *)cdir, (dirnum > 1)?"/":"",(char *)srct->filename);
//      printf("chk spri3: %s, %d, %d\n", (char *)dirs[dirnum], strlen((char *)dirs[dirnum]), dirnum);
      strcpy((char *)cdir, (char *)dirs[dirnum]);
      sel = disp = 0;
      ret = false;
      break;
    case TYPE_UDIR:
      dirnum--;   
      strcpy((char *)cdir, (char *)dirs[dirnum]);
//      printf("chk spri4: %s, %d, %d\n", (char *)dirs[dirnum], strlen((char *)dirs[dirnum]), dirnum);
      sel = disp = 0;
      ret = false;
      break;
  }
//  printf("ck5 %s %s %d\n", cdir, dst, ret);
  return ret;
}

void filerinit(){

}

void midiCallback(midi_event *pev)
// Called by the MIDIFile library when a file event needs to be processed
// thru the midi communications interface.
// This callback is set up in the setup() function.
{
  if ((pev->data[0] >= 0x80) && (pev->data[0] <= 0xe0))
  {
    Serial2.write(pev->data[0] | pev->channel);
    Serial2.write(&pev->data[1], pev->size-1);
  }
  else
    Serial.write(pev->data, pev->size);
}

void sysexCallback(sysex_event *pev)
// Called by the MIDIFile library when a system Exclusive (sysex) file event needs 
// to be processed through the midi communications interface. Most sysex events cannot 
// really be processed, so we just ignore it here.
// This callback is set up in the setup() function.
{

}

void midiSilence(void)
// Turn everything off on every channel.
// Some midi files are badly behaved and leave notes hanging, so between songs turn
// off all the notes and sound
{
  midi_event ev;

  // All sound off
  // When All Sound Off is received all oscillators will turn off, and their volume
  // envelopes are set to zero as soon as possible.
  ev.size = 0;
  ev.data[ev.size++] = 0xb0;
  ev.data[ev.size++] = 120;
  ev.data[ev.size++] = 0;

  for (ev.channel = 0; ev.channel < 16; ev.channel++)
    midiCallback(&ev);
}

void setup(void)
{
  //  disableLoopWDT();
  auto cfg = M5.config();
  M5Cardputer.begin(cfg);
  int textsize = M5Cardputer.Display.height() / 60;
  if (textsize == 0) {
    textsize = 1;
  }
  M5Cardputer.Display.setTextSize(textsize); 
  Serial.begin(115200);
  Serial2.begin(SERIAL_RATE, SERIAL_8N1, 1, 2);

  // Initialize SD
  if (!SD.begin(SD_SELECT, SPI_FULL_SPEED))
  {
    while (true) ;
  }

  // Initialize MIDIFile
  SMF.begin(&SD);
  SMF.setMidiHandler(midiCallback);
  SMF.setSysexHandler(sysexCallback);

  synth.begin(&Serial2, UNIT_SYNTH_BAUD, 1, 2);
}

void tickMetronome(void)
// flash a LED to the beat
{
  static uint32_t lastBeatTime = 0;
  static boolean  inBeat = false;
  uint16_t  beatTime;

  beatTime = 60000/SMF.getTempo();    // msec/beat = ((60sec/min)*(1000 ms/sec))/(beats/min)
  if (!inBeat)
  {
    if ((millis() - lastBeatTime) >= beatTime)
    {
      lastBeatTime = millis();
      inBeat = true;
    }
  }
  else
  {
    if ((millis() - lastBeatTime) >= 100)	// keep the flash on for 100ms only
    {
      inBeat = false;
    }
  }
}

void loop(void)
{
  static enum { S_IDLE, S_PLAYING, S_END, S_WAIT_BETWEEN } state = S_IDLE;
  static uint16_t currTune = ARRAY_SIZE(tuneList);
  static uint32_t timeStart;
  int i;
  uint8_t fname[PATHMAX] ;

while(1){  
  disptitle(STATCLR);
  filenum = makemdxlist();
  i = selectfile();
  disptitle(STATLOAD);
	if(!cnvfile(&filelist[i], fname)) //chance
  	continue;
 state = S_IDLE;
 playend = false;
 while(state != S_END){
  hitkey();
  switch (state)
  {
  case S_IDLE:    // now idle, set up the next tune
    {
      int err;
      // use the next file name and play it
      err = SMF.load((const char *)fname);
      if (err != MD_MIDIFile::E_OK)
      {
        timeStart = millis();
        state = S_WAIT_BETWEEN;
      }
      else
      {
        state = S_PLAYING;
        disptitle(STATPLAY);
      }
    }
    break;

  case S_PLAYING: // play the file
    if (!SMF.isEOF())
    {
      if (SMF.getNextEvent())
        tickMetronome();
    }
    else
      playend = true;

    if(playend){
      SMF.close();
      midiSilence();
      timeStart = millis();
      state = S_WAIT_BETWEEN;      
    }
    break;
  case S_WAIT_BETWEEN:    // signal finished with a dignified pause
    if (millis() - timeStart >= WAIT_DELAY)
      state = S_END;
    break;
  default:
    state = S_IDLE;
    break;
  }
 }
}
}