
/////////////// SHIFT REGISTER ////////////////////////
int latchPin = A3;            //connect 595 ’ pin 12
int clockPin = A4;           //connect 595 ’s pin11 
int dataPin = A5;         
 



#include <PS2KeyAdvanced.h>
#include <Oscil.h> // oscillator template

#include <MozziGuts.h>
#include <Sample.h> // Sample template
#include <samples/drums/kit_1/KICK.h> // wavetable data
#include <samples/drums/kit_1/SNARE.h> //   wavetable data
#include <samples/drums/kit_1/CLAP.h> // wavetable data
#include <samples/drums/kit_1/HIHAT_CLOSED.h> // wavetable data
#include <samples/drums/kit_1/COW_BELL.h> // wavetable data
#include <samples/drums/kit_1/TOM.h> // wavetable data

#include <tables/triangle512_int8.h> // sine table for oscillator

#include <EventDelay.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>

#include <MIDI.h>
#include <EEPROM.h>

//MIDI_CREATE_DEFAULT_INSTANCE();
#define CONTROL_RATE 4096 // Hz, powers of 2 are most reliable

// use: Sample <table_size, update_rate> SampleName (wavetable)
Sample <KICK_NUM_CELLS, AUDIO_RATE>aKick(KICK_DATA);
Sample <SNARE_NUM_CELLS, AUDIO_RATE>aSnare(SNARE_DATA);
Sample <CLAP_NUM_CELLS, AUDIO_RATE>aClap(CLAP_DATA);
Sample <HIHAT_CLOSED_NUM_CELLS, AUDIO_RATE>aHihatClosed(HIHAT_CLOSED_DATA);
Sample <COW_BELL_NUM_CELLS, AUDIO_RATE>aCowBell(COW_BELL_DATA);
Sample <TOM_NUM_CELLS, AUDIO_RATE>aTom(TOM_DATA);

Oscil <TRIANGLE512_NUM_CELLS, AUDIO_RATE> aSin(TRIANGLE512_DATA);


#define DATAPIN 4
#define IRQPIN  3

PS2KeyAdvanced keyboard;

int masterOut;
int debugOut;

byte toggleTap2=1;
uint16_t sequenceTimerIndex;
uint16_t stutterTimerIndex;
byte stutterToggle;
byte resetStutter;

uint16_t oscilTimerIndex;
uint16_t oscilTimerCount=100;
uint16_t oscilOut;

uint16_t fxGlissTimerCount=300;
uint16_t fxGlissTimerIndex;
byte fxGlissToggle;
byte fxGlissDirection;
#define FX_GLISS_DIRECTION_UP       0
#define FX_GLISS_DIRECTION_DOWN     1


byte oscilToggle;

uint16_t recSeqTimerIndex;
byte recSequenceIndex;

uint16_t tapTempoTimerIndex;
byte toggleTap;
float freq = 500.f;

// for scheduling audio gain changes
EventDelay kTriggerDelay;
//EventDelay kTriggerDelayLedBlink;

byte sequenceIndex=0;
byte sequenceLength=15;
#define SEQ_MAX_LENGTH  16

byte toggle=1;
byte playStop=0;
int16_t ledData;
int16_t ledData2;
#define DEFAULT_TEMPO   120
uint16_t tempo =CONTROL_RATE*60.0/DEFAULT_TEMPO;
uint16_t stutterTempo;
uint16_t initSamples=AUDIO_RATE*2;

#define KICK  0
#define SNARE  1
#define CLAP   2
#define HIHAT_CLOSED  3
#define TOM     4
#define COW_BELL  5
#define STRETCH  6
#define DISTORTION  7


#define LENGTH  8
#define JUMP  9



//byte voiceKeys[]=
//byte sequencerIndexNotes[]={41,43,45,47, 48,50,52,53, 55,57,59,60, 62,64,65,67};
//byte sequencerIndexNotes[]={41,43,45,47, 53,55,57,59, 65,67,69,71, 77,79,81,83};

#define SCALE_MAX_LENGTH    11

#define VOICE_NUM   6

#define LAYER_1_16   0
#define LAYER_17_32   1

uint32_t one=1;
uint32_t  voiceSequence[VOICE_NUM];
uint32_t  voiceSequenceVol[VOICE_NUM];
uint32_t  voiceSequenceTemp[VOICE_NUM];
uint32_t  voiceSequenceVolTemp[VOICE_NUM];
byte stepsLayer;
byte reloadSequenceToggle;

byte      voiceVolume[VOICE_NUM];
byte      voiceMute[VOICE_NUM];


bool lastSelectedVol=!lastSelectedVol;
bool sequenceNext;

/////// VOICE SELECT ///////
#define VOICE_KICK_KEY        90            // z
#define VOICE_SNARE_KEY       88            // x
#define VOICE_CLAP_KEY        67            // c
#define VOICE_HIHAT_KEY       86            // v
#define VOICE_COWBELL_KEY     78            // n
#define VOICE_TOM_KEY         66            // m

#define LENGTH_KEY            48            // 0
#define JUMP_STEP_KEY         285            // tab
#define RESET_KEY             49            // 1
#define STEPS_1_16__KEY             60            // '
#define STEPS_17_32__KEY             95            // ¡


#define PP_KEY                70            // f
#define FF_KEY                71            // g
#define A_TYPE_KEY            72            // h
#define B_TYPE_KEY            74            // j

#define STUTTER_5_ON_KEY           35            // 3 press
#define STUTTER_5_OFF_KEY           32803            // 3 release

#define STUTTER_6_ON_KEY           34            // 2 press
#define STUTTER_6_OFF_KEY           32802            // 2 release

#define STUTTER_8_ON_KEY           33            // 1 press
#define STUTTER_8_OFF_KEY           32801            // 1 release

#define STUTTER_16_ON_KEY           32            // 0 press
#define STUTTER_16_OFF_KEY           32800           // 0 release

#define CLEAR_ALL_KEY         284           // retroceso
#define VOICE_CLEAR_KEY       282           // supr
#define RANDOM_VOICE_KEY      274           // fin
#define MUTE_KEY              276           // av pag

#define START_STOP            287           // espacio

/////// TEMPO //////////////
#define TAP_TEMPO_KEY       286           // entrar
#define UP_KEY              279           // arriba
#define DOWN_KEY            280           // abajo

///////// PLAY MODE ///////////
#define MODE_PLAY_KEY   281                  // insert 
#define MODE_REC_8TH_KEY   273                   // inicio
#define MODE_REC_16TH_KEY   275                   // repag
#define MODE_EDIT_KEY   0                  // 


///////// PRESETS ///////////
#define SAVE_PRESET_KEY_1   8545            // ctrl + F1
#define SAVE_PRESET_KEY_2   8546            //
#define SAVE_PRESET_KEY_3   8547            //
#define SAVE_PRESET_KEY_4   8548            //

#define SAVE_PRESET_KEY_5   8549            //
#define SAVE_PRESET_KEY_6   8550            //
#define SAVE_PRESET_KEY_7   8551            //
#define SAVE_PRESET_KEY_8   8552            //

#define SAVE_PRESET_KEY_9   8553            //
#define SAVE_PRESET_KEY_10  8554            //
#define SAVE_PRESET_KEY_11  8555            //
#define SAVE_PRESET_KEY_12  8556            // ctrl + F12

#define LOAD_PRESET_KEY_1   353           // F1
#define LOAD_PRESET_KEY_2   354           // ...
#define LOAD_PRESET_KEY_3   355           //
#define LOAD_PRESET_KEY_4   356           //

#define LOAD_PRESET_KEY_5   357           //
#define LOAD_PRESET_KEY_6   358           //
#define LOAD_PRESET_KEY_7   359           //
#define LOAD_PRESET_KEY_8   360           //

#define LOAD_PRESET_KEY_9   361           //
#define LOAD_PRESET_KEY_10  362           //
#define LOAD_PRESET_KEY_11  363           //
#define LOAD_PRESET_KEY_12  364           // F12

#define FX_SRATE_DOWN_ON       75            //  k
#define FX_SRATE_DOWN_OFF      32843         // 
#define FX_SRATE_UP_ON        76            //  l
#define FX_SRATE_UP_OFF       32844         //  
#define FX_DISTORTION_ON    91            //  ñ
#define FX_DISTORTION_OFF   32859         //  
#define FX_BIT_CRUSH_ON     58            // ´
#define FX_BIT_CRUSH_OFF    32826         //

#define FX_STUTTER_A_ON       77            //  m
#define FX_STUTTER_A_OFF       32845        //  
#define FX_STUTTER_B_ON       59            //  ,
#define FX_STUTTER_B_OFF       32827        //  
#define FX_STUTTER_C_ON       61            //  .
#define FX_STUTTER_C_OFF       32829        //  
#define FX_STUTTER_D_ON       62            //  -
#define FX_STUTTER_D_OFF       32830        //  


#define FX_STUTTER_A_RATIO       16        //  
#define FX_STUTTER_B_RATIO       4        //  
#define FX_STUTTER_C_RATIO       3        //  
#define FX_STUTTER_D_RATIO       2        //  

#define FX_GLISS_DOWN_ON                94        //  +
#define FX_GLISS_DOWN_OFF                32862        //  +

#define FX_GLISS_UP_ON                93        //  `
#define FX_GLISS_UP_OFF                32861        //  `


#define LASER_MIN_COUNT 5

byte fxSrateDown;
byte fxSrateUp;
byte fxDistortion;
byte fxBitCrush;
byte fxSrateDownToggle;
byte fxSrateUpToggle;


byte voiceSelect;

byte sequenceIndexKeys[]={81,50,87,51, 69,52,82,53, 84,54,89,55, 85,56,73,57};
byte voiceIndex[]={VOICE_KICK_KEY, VOICE_SNARE_KEY, VOICE_CLAP_KEY, VOICE_HIHAT_KEY, VOICE_TOM_KEY, VOICE_COWBELL_KEY};
byte scaleOnIndexKeys[]={32,42,33,34,35,36,37,38,39,40,41};
uint16_t scaleOffIndexKeys[]={32800,32810,32801,32802,32803,32804,32805,32806,32807,32808,32809};
byte scale[]={0,3,5,8,10,12,15,17,20,22,24};
byte noteOffset;
byte pressedKeysIndex;
byte pressedKeys[10];
byte noteSelected;

uint16_t prevKey;

byte voiceMode;

#define MODE_PLAY     0
#define MODE_REC_8TH      1
#define MODE_REC_16TH      2
#define MODE_EDIT     3









#define PRESET_SIZE 52    // preset size en bytes


byte lpfToggle;

byte i;
byte ledBlinkFlag;
byte ledToggle;
byte toggleScatter;



int pot1prev;
int pot1;
int potProcesado;


const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average
int averagePrev = 0;                // the average



  int asig1;
  int asig2;
  int asig3;
  int asig4;
  int asig5;
  int asig6;
  int asig7;




void setup()
{
  //delay(500);
  keyboard.begin(DATAPIN, IRQPIN);
  Serial.begin(9600);


  aKick.setFreq((float) KICK_SAMPLERATE / (float) KICK_NUM_CELLS); // play at the speed it was recorded at
  aSnare.setFreq((float) SNARE_SAMPLERATE / (float) SNARE_NUM_CELLS); // play at the speed it was recorded at
  aClap.setFreq((float) CLAP_SAMPLERATE / (float) CLAP_NUM_CELLS); // play at the speed it was recorded at
  aHihatClosed.setFreq((float) HIHAT_CLOSED_SAMPLERATE / (float) HIHAT_CLOSED_NUM_CELLS); // play at the speed it was recorded at
  aCowBell.setFreq((float) COW_BELL_SAMPLERATE / (float) COW_BELL_NUM_CELLS); // play at the speed it was recorded at
  aTom.setFreq((float) TOM_SAMPLERATE / (float) TOM_NUM_CELLS); // play at the speed it was recorded at

  //kTriggerDelay.set(120); // countdown ms, within resolution of CONTROL_RATE
  //kTriggerDelayTapTempo.set(1000); // countdown ms, within resolution of CONTROL_RATE
  //kTriggerDelayLedBlink.set(10); // countdown ms, within resolution of CONTROL_RATE


//  MIDI.setHandleNoteOn(handleNoteOn);  // Put only the name of the function
//  MIDI.setHandleNoteOff(handleNoteOff);
//  MIDI.setHandleControlChange(handleControlChange);
  //MIDI.begin(MIDI_CHANNEL_OMNI);

  pinMode(latchPin, OUTPUT); 
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT); 
  startMozzi(CONTROL_RATE); // :)

/*
uint32_t test=0;
for (int i = 0; i < 32; ++i)
{
  test |= uint32_t(1)<<i;
  printBits(test);
  delay(100);
//Serial.println(test, BIN);
}
*/

}



void loop()
{
  audioHook();


  

  

}
