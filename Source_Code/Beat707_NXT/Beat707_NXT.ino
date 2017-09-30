/*
 * 
 * Beat707 NXT - Works with 3 x TM1638 boards connected in paralel
 * Created by William Kalfelz @ Beat707 (c) 2017 - http://www.Beat707.com
 * 
 * https://github.com/Beat707/Beat707-NXT
 * 
 * Version 1.0.0 - Sep 28 2017
 * 
 */

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "X_Lib_Flash.h"

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define DRUM_TRACKS 16        // This can't go above 16
#define NOTE_TRACKS 8         // This can't go above 8
#define SONG_POSITIONS 64     // Depends on RAM left, can't go above 99
#define PATTERNS 64           // This can't go above 64
#define ECHOS 6               // Depends on RAM left, usually can't go above 9
#define SONGS 30              // 30 songs should be the MAX that the Winbond chip can take
#define STEPS 16              // This can't go above 16
#define INIT_FLASH_MEMORY 0   // Warning, this will erase the entire flash contents
#define INIT_ENTIRE_FLASH 1   // When selected with the above, the entire flash chip will be erased first. This takes around 20 seconds or less to perform before we can write data to the flash chip. If this is not set, the code will erase the flash chip by sector, which takes a bit more time to perform.
#define SHOW_FREE_RAM 0       // Will show how much RAM is left on the display during initiation
#define DEBUG_SERIAL 0        // Sets the serial output to 9600 bauds and sends some debuging information out.
#define MIDI_OVER_USB 0       // When set will use 38400 bauds for the Serial interface

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
enum
{
  fadeIn = 1, fadeOut, fadeInOut, fadeOutIn, fadeInD, fadeOutD, randomVel,
  kButtonNone = 0, kButtonClicked, kButtonHold, kButtonRelease, kButtonReleaseNothingClicked,
  kRightSteps = 0, kRightABCDVariations, kRightTrackSelection, kRightPatternSelection, kRightMenuCopyPaste, kRightMenu, kMuteSoloMenu,
  midiNoteOn = 0x90, midiNoteOff = 0x80, midiCC = 0xB0, midiChannels=16,
  patternMode = 0, songMode,
  accentTrack = (DRUM_TRACKS-1),
  echoTypeOnAllNotes = 0, echoTypeForceMaxVelocity, echoTypeForceLowVelocity,
  procFadeMin = 4, 
  menuFirst = 0, menuMidiCC = 0, menuNote, menuAccent1, menuAccent2, menuAccent3, menuProc, menuEcho, menuEchoTrack, menuEchoTicks,
  menuEchoSpace, menuEchoAttackDecay, menuEchoType, menuVariationsABCD, menuSyncOut, menuSong, menuSysex, menuInit, menuToSongMode, lastMenu = menuToSongMode, 
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
bool changedSong = false;
char flashHeader[8];
char songSysExDump = -2;
byte sysExDump = 0;
bool hasSoloTrack = false;
uint32_t prevMuteTrack = 0;
uint32_t prevSoloTrack = 0xFFFFFFFF;
byte totalFlashErrors = 0;
//
byte echoCounter[ECHOS][2];
byte echoVelocity[ECHOS];
byte echoTrack[ECHOS];
char echoAttackDecay[ECHOS];
byte echoSpace[ECHOS];
byte echoEdit = 0;
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
  bool hasInit = true; // Used by the SysEx code
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
  byte trackNote[DRUM_TRACKS] { 36, 40, 42, 44, 46, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 0 };
  byte trackMidiCH[DRUM_TRACKS+NOTE_TRACKS]; // 0~15 
  byte accentValues[3] = { 80, 100, 127 };
  byte BPM = 120;
  boolean seqSyncOut = false;
  uint32_t muteTrack;
  uint32_t soloTrack; 
};
//
struct WSONG
{
  byte pattern[2][SONG_POSITIONS]; // Pattern Number and Options: ABCD var and repeat 0 to 15 (4 bits each)
  char loopTo;
  bool hasInit = true; // Used by the SysEx code
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
  startMIDIinterface();
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
  resetSegments(0, 2);
  memset(echoSpace, 0, sizeof(echoSpace));
  memset(echoAttackDecay, 0, sizeof(echoAttackDecay));
  memset(echoTrack, 0, sizeof(echoTrack));
  memset(leds, 0, sizeof(leds));
  memset(buttons, 0, sizeof(buttons));  
  memset(buttonEvent, 0, sizeof(buttonEvent));
  memset(buttonDownTime, 0, sizeof(buttonDownTime));
  memset(configData.trackMidiCH, 9, sizeof(configData.trackMidiCH));
  memset(prevPlayedNote, 0, sizeof(prevPlayedNote));
  memset(echoCounter, 0, sizeof(echoCounter));
  memset(echoVelocity, 0, sizeof(echoVelocity));
  for (byte x = 0; x < 8; x++) { configData.trackMidiCH[DRUM_TRACKS + x] = x; }
  bitSet(patternBitsSelector,0);
  bitSet(patternBitsSelector,8);
  configData.muteTrack = configData.soloTrack = 0;
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
