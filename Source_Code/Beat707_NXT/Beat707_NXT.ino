/*
 * 
 * Created by William Kalfelz @ Beat707 (c) 2017 - http://www.Beat707.com
 * 
 */

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "X_Lib_Flash.h"

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define DRUM_TRACKS 16
#define NOTE_TRACKS 8
#define SONG_POSITIONS 99
#define ECHOS 8
#define SONGS 30
#define STEPS 16
#define INIT_FLASH_MEMORY 0
#define SHOW_FREE_RAM 0
#define DEBUG_SERIAL 0

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
enum
{
  fadeIn = 1, fadeOut, fadeInOut, fadeOutIn, fadeInD, fadeOutD, randomVel,
  kButtonNone = 0, kButtonClicked, kButtonHold, kButtonRelease, kButtonReleaseNothingClicked,
  kRightSteps = 0, kRightABCDVariations, kRightTrackSelection, kRightPatternSelection, kRightMenuCopyPaste, kRightMenu,
  midiNoteOn = 0x90, midiNoteOff = 0x80, midiCC = 0xB0, midiChannels=16,
  patternMode = 0, songMode,
  accentTrack = (DRUM_TRACKS-1),
  echoTypeOnAllNotes = 0, echoTypeForceMaxVelocity, echoTypeForceLowVelocity,
  procFadeMin = 4, lastMenu = 16, initMenu = 15,
  kLeftMain = 0
};

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SPIFlash flash;
byte segments[3][8];
byte leds[3];
byte buttons[3]; // raw button values
byte buttonEvent[3][8];
byte buttonEventWasHolding[3] = {0, 0, 0};
byte buttonIsHolding[8];
byte buttonDownTime[3][8]; // time of the button press in ms
byte variation = 0; // ABCD Variations
char forceVariation = -1;
byte curTrack = 0;
bool updateScreen = true;
byte curLeftScreen = 0;
byte curRightScreen = 0;
byte currentPattern = 0;
byte nextPattern = 0;
uint16_t patternBitsSelector;
byte currentSong = 0;
bool mirror = false;
bool somethingClicked = false;
bool somethingHappened = false;
bool forceAccent = false;
bool showBPMdot = false;
byte prevVel = 1;
bool seqPlaying = false;
byte seqPosition = 0;
byte seqCounter = 0;
byte ppq24Counter = 0;
byte midiClockBeats = 0;
bool somethingChangedPattern = false; // if true saves the pattern (steps, double steps, midi processor and variations)
bool somethingChangedConfig = false; // if true saves the config data (BPM, track Note #, track MIDI CC, SeqSyncOut)
bool somethingChangedSong = false;
bool streamNextPattern = false;
bool loadPatternNow = false;
byte ignoreButtons = false;
byte lastVelocity = B00000011;
bool editingNote = false;
byte editStep = 0;
byte editVariation = 0;
byte prevPlayedNote[NOTE_TRACKS];
byte menuPosition = 0;
byte currentMode = patternMode;
byte songPosition = 0;
byte initMode = 0;
byte echoCounter[ECHOS][2];
byte echoVelocity[ECHOS];
byte echoEdit = 0;
bool changedSong = false;
char flashHeader[8];
//
struct WECHO
{
  byte track, ticks, space, type;
  char attackDecay;
  //
  void init()
  {
    track = type = 0;
    ticks = 24;
    space = 14;
    attackDecay = 5;
  }
};
struct WSTEPS
{
  byte steps[DRUM_TRACKS]; // steps are stored as 8 bits variations in AABBCCDD format. 2 bits per step per variation. 0~3
  uint16_t stepsDouble[4]; // 16 drum tracks, 4 bits each for the ABCD variations
  byte noteSteps[NOTE_TRACKS][4];
  byte noteStepsExtras[NOTE_TRACKS][2]; // 2 bits packed as ABCD: [0] = Velocity (2 bits) ABCD, [1] = Glide/Double ABCD (2 bits)
  //
  void init()
  {
    memset(steps, 0, sizeof(steps));
    memset(stepsDouble, 0, sizeof(stepsDouble));
    memset(noteSteps, 0, sizeof(noteSteps));
    memset(noteStepsExtras, 0, sizeof(noteStepsExtras));
  }
};
struct WPATTERN // 
{
  byte totalVariations;
  byte trackProcessor[DRUM_TRACKS+NOTE_TRACKS]; 
  byte lastNote[NOTE_TRACKS];
  WECHO echoConfig[ECHOS];
  //
  void init()
  {
    for (byte xe=0; xe < ECHOS; xe++) { echoConfig[xe].init(); }
    memset(trackProcessor, 0, sizeof(trackProcessor));
    memset(lastNote, 0, sizeof(lastNote));
    totalVariations = 4;
  }
};
//
struct WCONFIG
{
  char header = 'C';
  byte trackNote[DRUM_TRACKS] { 36, 40, 42, 44, 46, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58 };
  byte trackMidiCH[DRUM_TRACKS+NOTE_TRACKS]; // 0~15 
  byte accentValues[3] = { 80, 100, 127 };
  byte BPM = 120;
  boolean seqSyncOut = false;
};
//
struct WSONG
{
  byte pattern[2][SONG_POSITIONS]; // Pattern Number and Options: ABCD var and repeat 0 to 15 (4 bits each)
  char loopTo;
  void init()
  {
    memset(pattern, 0, sizeof(pattern));
    loopTo = -1; // No Loop //
    for (byte x=0; x < SONG_POSITIONS; x++) { pattern[1][x] = 4 << 4; }
  }
};
//
WSTEPS stepsData[STEPS];
WPATTERN patternData;
WCONFIG configData;
WSONG songData;

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup() 
{
  startMIDIinterface(false);
  initTM1638();
  reset();
  #if SHOW_FREE_RAM
    freeMemory();
    sendScreen();
    delay(2000);
  #endif 
  flashInit(false);
  //
  startTimer();
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void reset()
{
  createFlashHeader(currentSong);
  memset(segments, 0, sizeof(segments));
  memset(leds, 0, sizeof(leds));
  memset(buttons, 0, sizeof(buttons));  
  memset(buttonEvent, 0, sizeof(buttonEvent));
  memset(buttonDownTime, 0, sizeof(buttonDownTime));
  memset(configData.trackMidiCH, 9, sizeof(configData.trackMidiCH));
  memset(prevPlayedNote, 0, sizeof(prevPlayedNote));
  memset(echoCounter, 0, sizeof(echoCounter));
  memset(echoVelocity, 0, sizeof(echoVelocity));
  for (byte x = 0; x < 8; x++) { configData.trackMidiCH[DRUM_TRACKS + x] = 1 + x; }
  bitSet(patternBitsSelector,0);
  bitSet(patternBitsSelector,8);
  //
  songData.init();
  for (byte x=0; x < 4; x++) 
  { 
    stepsData[0 + (x*4)].init(); 
    stepsData[1 + (x*4)].init(); 
    stepsData[2 + (x*4)].init(); 
    stepsData[3 + (x*4)].init(); 
    //
    stepsData[0 + (x*4)].steps[15] = B11111111;
    stepsData[1 + (x*4)].steps[15] = B10101010;
    stepsData[2 + (x*4)].steps[15] = B01010101;
    stepsData[3 + (x*4)].steps[15] = B10101010;
    
  }
  patternData.init();
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() 
{
  checkPatternStream();
  readButtons();
  if (currentMode == patternMode) checkInterface(); else songInterface();
  //
  if (updateScreen)
  {
    updateScreen = false;
    if (currentMode == patternMode) createScreen(); else createSongScreen();
    sendScreen();
  }
}
