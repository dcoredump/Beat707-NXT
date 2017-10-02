/*
 * 
 * Created by William Kalfelz @ Beat707 (c) 2017 - http://www.Beat707.com
 * 
 */

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void showMenuCopyPaste()
{
  // 12341234 12341234
  // PAT COPY PST INIT
  //
  segments[1][0] = S_P;
  segments[1][1] = S_A;
  segments[1][2] = S_T;
  //
  segments[1][4] = S_C;
  segments[1][5] = S_O;
  segments[1][6] = S_P;
  segments[1][7] = S_Y;
  //
  segments[2][0] = S_P;
  segments[2][1] = S_S;
  segments[2][2] = S_T;
  //
  segments[2][4] = S_I;
  segments[2][5] = S_n;
  segments[2][6] = S_I;
  segments[2][7] = S_T;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void processMenuCopyPaste(byte button)
{
  if (button >= 4 && button <= 7) // Copy
  {
    uint16_t pagePos = 16 + (SONGS * ((64 * 16) + 16));
    eraseSector(pagePos);
    if (!flash.writeAnything(pagePos, (uint8_t) 0, patternData)) showErrorMsg(flash.error());
    pagePos++;
    if (!flash.writeAnything(pagePos, (uint8_t) 0, stepsData)) showErrorMsg(flash.error());
    
  }
  else if (button >= 8 && button <= 11) // Paste
  {
    uint16_t pagePos = 16 + (SONGS * ((64 * 16) + 16));
    if (!flash.readAnything(pagePos, (uint8_t) 0, patternData)) showErrorMsg(flash.error());
    pagePos++;
    if (!flash.readAnything(pagePos, (uint8_t) 0, stepsData)) showErrorMsg(flash.error());
    somethingChangedPattern = true;
  }
  else if (button >= 12 && button <= 15) // Init
  {
    uint16_t pagePos = 16 + (SONGS * ((64 * 16) + 16)) + 16;
    if (!flash.readAnything(pagePos, (uint8_t) 0, patternData)) showErrorMsg(flash.error());
    pagePos++;
    if (!flash.readAnything(pagePos, (uint8_t) 0, stepsData)) showErrorMsg(flash.error());
    somethingChangedPattern = true;
  }
  //
  curRightScreen = kRightSteps;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void showMenu()
{
  switch(menuPosition)
  {
    case menuMidiCC: 
            segments[2][0] = S_N;
            segments[2][1] = S_N;
            segments[2][2] = S_C;
            segments[2][3] = S_H;
            //
            if (curTrack != 15) printNumber(2, 5, configData.trackMidiCH[curTrack] + 1);
              else printDashDash(2, 6);
    break;
    //
    case menuNote:
            segments[1][4] = S_N;
            segments[1][5] = S_O;
            segments[1][6] = S_T;
            segments[1][7] = S_E;
            //
            if (curTrack < (DRUM_TRACKS - 1))
            {
              printNumber(2, 0, configData.trackNote[curTrack] + 1);
              printMIDInote(configData.trackNote[curTrack], 2, 4, 7);
            }
            else printDashDash(2, 6);
    break;
    //
    case menuAccent1: 
    case menuAccent2:
    case menuAccent3: 
            segments[1][0] = S_A;
            segments[1][1] = S_C;
            segments[1][2] = S_C;
            segments[1][3] = S_E;
            segments[1][4] = S_N;
            segments[1][5] = S_T;
            //
            segments[2][0] = S_U;
            segments[2][1] = (char)pgm_read_word(&numbers[menuPosition-menuAccent1+1]);
            //
            printNumber(2, 4, configData.accentValues[menuPosition-menuAccent1]);
    break;
    //
    case menuProc:
            segments[1][0] = S_P;
            segments[1][1] = S_R;
            segments[1][2] = S_O;
            segments[1][3] = S_C;
            //
            if (curTrack != 15) 
            {
              if (patternData.trackProcessor[curTrack] == fadeIn) 
              {
                segments[2][0] = S_F;
                segments[2][1] = S_A;
                segments[2][2] = S_d;
                segments[2][3] = S_E;
                //
                segments[2][5] = S_I;
                segments[2][6] = S_N;
              }
              else if (patternData.trackProcessor[curTrack] == fadeOut) 
              {
                segments[2][0] = S_F;
                segments[2][1] = S_A;
                segments[2][2] = S_d;
                segments[2][3] = S_E;
                //
                segments[2][5] = S_O;
                segments[2][6] = S_U;
                segments[2][7] = S_T;
              }
              else if (patternData.trackProcessor[curTrack] == fadeInD) 
              {
                segments[2][0] = S_F;
                segments[2][1] = S_A;
                segments[2][2] = S_d;
                segments[2][3] = S_E;
                //
                segments[2][5] = S_I;
                segments[2][6] = S_N;
                segments[2][7] = S_d;
              }
              else if (patternData.trackProcessor[curTrack] == fadeOutD) 
              {
                segments[2][0] = S_F;
                segments[2][1] = S_A;
                segments[2][2] = S_d;
                segments[2][3] = S_E;
                segments[2][4] = S_O;
                segments[2][5] = S_U;
                segments[2][6] = S_T;
                segments[2][7] = S_d;
              }
              else if (patternData.trackProcessor[curTrack] == fadeInOut) 
              {
                segments[2][0] = S_F;
                segments[2][1] = S_d;
                //
                segments[2][3] = S_I;
                segments[2][4] = S_N;
                segments[2][5] = S_O;
                segments[2][6] = S_U;
                segments[2][7] = S_T;
              }
              else if (patternData.trackProcessor[curTrack] == fadeOutIn) 
              {
                segments[2][0] = S_F;
                segments[2][1] = S_d;
                //
                segments[2][3] = S_O;
                segments[2][4] = S_U;
                segments[2][5] = S_T;
                segments[2][6] = S_I;
                segments[2][7] = S_N;
              }
              else if (patternData.trackProcessor[curTrack] == randomVel)
              {
                segments[2][0] = S_R;
                segments[2][1] = S_A;
                segments[2][2] = S_N;
                segments[2][3] = S_d;
                segments[2][4] = S_O;
                segments[2][5] = S_N;
                segments[2][6] = S_N;
              }
              else printDashDash(2, 6);
            }
            else printDashDash(2, 6);
    break;
    //
    case menuEcho:
    case menuEchoTrack:
    case menuEchoTicks:
    case menuEchoSpace:
    case menuEchoAttackDecay:
    case menuEchoType:
            segments[1][4] = S_E;
            segments[1][5] = S_C;
            segments[1][6] = S_H;
            segments[1][7] = S_O;
            //
            if (menuPosition == menuEcho)
            {
              printNumber(2, 5, echoEdit+1);
            }
            else if (menuPosition == menuEchoTrack)
            {
              segments[2][0] = S_T;
              segments[2][1] = S_R;
              segments[2][2] = S_A;
              segments[2][3] = S_C;
              if (patternData.echoConfig[echoEdit].track == 0) printDashDash(2, 6); else printNumber(2, 5, patternData.echoConfig[echoEdit].track);
            }
            else if (menuPosition == menuEchoTicks)
            {
              segments[2][0] = S_T;
              segments[2][1] = S_I;
              segments[2][2] = S_C;
              segments[2][3] = S_S;
              if (patternData.echoConfig[echoEdit].track == 0) printDashDash(2, 6); else printNumber(2, 5, patternData.echoConfig[echoEdit].ticks);
            }
            else if (menuPosition == menuEchoSpace)
            {
              segments[2][0] = S_S;
              segments[2][1] = S_P;
              segments[2][2] = S_S;
              segments[2][3] = S_C;
              if (patternData.echoConfig[echoEdit].track == 0) printDashDash(2, 6); else printNumber(2, 5, patternData.echoConfig[echoEdit].space);
            }
            else if (menuPosition == menuEchoAttackDecay)
            {
              if (patternData.echoConfig[echoEdit].attackDecay > 0)
              {
                segments[2][0] = S_A;
                segments[2][1] = S_T;
                segments[2][2] = S_T;
                segments[2][3] = S_A;
                if (patternData.echoConfig[echoEdit].track == 0) printDashDash(2, 6); else printNumber(2, 5, patternData.echoConfig[echoEdit].attackDecay);
              }
              else if (patternData.echoConfig[echoEdit].attackDecay < 0)
              {
                segments[2][0] = S_d;
                segments[2][1] = S_E;
                segments[2][2] = S_C;
                segments[2][3] = S_Y;
                if (patternData.echoConfig[echoEdit].track == 0) printDashDash(2, 6); else printNumber(2, 5, -1 * patternData.echoConfig[echoEdit].attackDecay);
              }
              else 
              {
                segments[2][0] = S_A;
                segments[2][1] = S_DASH;
                segments[2][2] = S_d;
                printDashDash(2, 6);
              }
            }
            else if (menuPosition == menuEchoType)
            {
              if (patternData.echoConfig[echoEdit].track == 0) 
              {
                segments[2][0] = S_T;
                segments[2][1] = S_Y;
                segments[2][2] = S_P;
                segments[2][3] = S_E;
                //
                printDashDash(2, 6);
              }
              else if (patternData.echoConfig[echoEdit].type == 0)
              {
                segments[2][0] = S_A;
                segments[2][1] = S_L;
                segments[2][2] = S_L;
                //  
                segments[2][4] = S_N;
                segments[2][5] = S_O;
                segments[2][6] = S_T;
                segments[2][7] = S_E;
              }
              else if (patternData.echoConfig[echoEdit].type == 1)
              {
                segments[2][0] = S_T;
                segments[2][1] = S_O;
                segments[2][2] = S_P;
                //  
                segments[2][4] = S_N;
                segments[2][5] = S_O;
                segments[2][6] = S_T;
                segments[2][7] = S_E;
              }
              else if (patternData.echoConfig[echoEdit].type == 2)
              {
                segments[2][0] = S_L;
                segments[2][1] = S_O;
                segments[2][2] = 0x3c;
                segments[2][3] = 0x1e;
                //  
                segments[2][4] = S_N;
                segments[2][5] = S_O;
                segments[2][6] = S_T;
                segments[2][7] = S_E;
              }              
            }
    break;
    //
    case menuVariationsABCD:
            segments[2][0] = S_U;
            segments[2][1] = S_A;
            segments[2][2] = S_R;
            //
            segments[2][4] = S_A;
            if (patternData.totalVariations >= 2) segments[2][5] = S_b;
            if (patternData.totalVariations >= 3) segments[2][6] = S_C;
            if (patternData.totalVariations >= 4) segments[2][7] = S_d;
    break;
    //
    case menuSyncOut:
            segments[1][0] = S_S;
            segments[1][1] = S_Y;
            segments[1][2] = S_N;
            segments[1][3] = S_C;
            segments[1][4] = S_O;
            segments[1][5] = S_U;
            segments[1][6] = S_T;
            //
            if (configData.seqSyncOut)
            {
              segments[2][6] = S_O;
              segments[2][7] = S_N;
            }
            else
            {
              segments[2][5] = S_O;
              segments[2][6] = S_F;
              segments[2][7] = S_F;
            }
    break;
    //
    case menuSysex:
            segments[1][0] = S_S;
            segments[1][1] = S_Y;
            segments[1][2] = S_S;
            segments[1][3] = S_E;
            segments[1][4] = S_X;
            //
            if (songSysExDump == -2)
            {
              printDashDash(1, 6);
            }
            else if (songSysExDump == -1)
            {
              segments[1][5] = S_A;
              segments[1][6] = S_L;
              segments[1][7] = S_L;
            }
            else
            {
              printNumber(1, 5, songSysExDump + 1);
              segments[1][4] = S_X;
            }
            //
            if (sysExDump == 0)
            {
              segments[2][0] = S_d;
              segments[2][1] = S_U;
              segments[2][2] = S_N;
              segments[2][3] = S_N;
              segments[2][4] = S_P;
            }
            else if (sysExDump == 1)
            {
              segments[2][0] = S_R;
              segments[2][1] = S_E;
              segments[2][2] = S_C;
              segments[2][3] = S_E;
              segments[2][4] = S_I;
              segments[2][5] = S_U;
              segments[2][6] = S_E;
            }
    break;
    //
    case menuSong: 
            segments[2][0] = S_S;
            segments[2][1] = S_O;
            segments[2][2] = S_N;
            segments[2][3] = S_G;
            //
            printNumber(2, 5, currentSong + 1);
    break;
    //
    case menuInit:
            segments[1][4] = S_I;
            segments[1][5] = S_N;
            segments[1][6] = S_I;
            segments[1][7] = S_T;
            //
            if (initMode == 0)
            {
              segments[2][4] = S_N;
              segments[2][5] = S_O;
              segments[2][6] = S_N;
              segments[2][7] = S_E;
            }
            else if (initMode == 1)
            {
              segments[2][4] = S_S;
              segments[2][5] = S_O;
              segments[2][6] = S_N;
              segments[2][7] = S_G;
            }
            else
            {
              segments[2][5] = S_A;
              segments[2][6] = S_L;
              segments[2][7] = S_L;              
            }
    break;
    //
    case menuToSongMode:
            segments[1][1] = S_T;
            segments[1][2] = S_O;
            //
            segments[1][4] = S_S;
            segments[1][5] = S_O;
            segments[1][6] = S_N;
            segments[1][7] = S_G;
            //
            segments[2][0] = S_N;
            segments[2][1] = S_N;
            segments[2][2] = S_O;
            segments[2][3] = S_d;
            segments[2][4] = S_E;
    break;
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void processMenuOK()
{
  switch(menuPosition)
  {
    case menuSysex:
      if (seqPlaying) printStopSequencer();
      else
      {
        if (sysExDump == 0) sysexDump(songSysExDump); else sysexReceive(songSysExDump);
      }
    break;
    //
    case menuInit:
      if (seqPlaying) printStopSequencer();
      else
      {
        if (initMode == 1)
        {
          stopSequencer();
          reset();
          int porc = 0;
          initSong(currentSong, true, porc, true);
          loadSong(currentSong);
          checkMenuClose();
        }
        else if (initMode == 2)
        {
          stopSequencer();
          reset();
          currentSong = 0;
          flashInit(true);
          checkMenuClose();
        }
        else checkMenuClose();
      }
    break;
    // 
    case menuToSongMode:
      if (seqPlaying) printStopSequencer();
      else
      {
        checkIfDataNeedsSaving();
        //
        currentMode = songMode;
        songPosition = 0;
      }
    break;
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void processMenu(char value)
{
  switch(menuPosition)
  {
    case menuMidiCC:
      if (value > 0 && configData.trackMidiCH[curTrack] < 15) configData.trackMidiCH[curTrack]++;
      else if (value < 0 && configData.trackMidiCH[curTrack] > 0) configData.trackMidiCH[curTrack]--;
      somethingChangedConfig = true;
    break;
    //
    case menuNote:
      if (curTrack < DRUM_TRACKS)
      {
        if (value > 0 && configData.trackNote[curTrack] < 127) configData.trackNote[curTrack]++;
          else if (value < 0 && configData.trackNote[curTrack] > 0) configData.trackNote[curTrack]--;
        somethingChangedConfig = true;
      }
    break;
    //
    case menuAccent1:
    case menuAccent2:
    case menuAccent3:
        if (value > 0 && configData.accentValues[menuPosition-menuAccent1] < 127) configData.accentValues[menuPosition-menuAccent1]++;
          else if (value < 1 && configData.accentValues[menuPosition-menuAccent1] > 0) configData.accentValues[menuPosition-menuAccent1]--;
        somethingChangedConfig = true;
    break;
    //
    case menuProc:
        if (value > 0 && patternData.trackProcessor[curTrack] < 7) patternData.trackProcessor[curTrack]++;
          else if (value < 1 && patternData.trackProcessor[curTrack] > 0) patternData.trackProcessor[curTrack]--;
        somethingChangedPattern = true;
    break;
    //
    case menuEcho:
        if (value > 0 && echoEdit < (ECHOS-1)) echoEdit++;
          else if (value < 1 && echoEdit > 0) echoEdit--;
        somethingChangedPattern = true;
    break;
    //
    case menuEchoTrack:
        if (value > 0 && patternData.echoConfig[echoEdit].track < (DRUM_TRACKS-1)) patternData.echoConfig[echoEdit].track++;
          else if (value < 1 && patternData.echoConfig[echoEdit].track > 0) patternData.echoConfig[echoEdit].track--;
        somethingChangedPattern = true;
    break;
    //
    case menuEchoTicks:
        if (value > 0 && patternData.echoConfig[echoEdit].ticks < 127) patternData.echoConfig[echoEdit].ticks++;
          else if (value < 1 && patternData.echoConfig[echoEdit].ticks > 0) patternData.echoConfig[echoEdit].ticks--;
        somethingChangedPattern = true;
    break;
    //
    case menuEchoSpace:
        if (value > 0 && patternData.echoConfig[echoEdit].space < 127) patternData.echoConfig[echoEdit].space++;
          else if (value < 1 && patternData.echoConfig[echoEdit].space > 0) patternData.echoConfig[echoEdit].space--;
        somethingChangedPattern = true;
    break;
    //
    case menuEchoAttackDecay:
        if (value > 0 && patternData.echoConfig[echoEdit].attackDecay < 127) patternData.echoConfig[echoEdit].attackDecay++;
          else if (value < 1 && patternData.echoConfig[echoEdit].attackDecay > -127) patternData.echoConfig[echoEdit].attackDecay--;
        somethingChangedPattern = true;
    break;  
    //
    case menuEchoType:
        if (value > 0 && patternData.echoConfig[echoEdit].type <= 1) patternData.echoConfig[echoEdit].type++;
          else if (value < 1 && patternData.echoConfig[echoEdit].type > 0) patternData.echoConfig[echoEdit].type--;
        somethingChangedPattern = true;
    break;  
    //
    case menuVariationsABCD:
        if (value > 0 && patternData.totalVariations < 4) patternData.totalVariations++;
          else if (value < 1 && patternData.totalVariations > 1) patternData.totalVariations--;
        somethingChangedPattern = true;
    break;
    //
    case menuSyncOut:
      configData.seqSyncOut = !configData.seqSyncOut;
      somethingChangedConfig = true;
    break;
    //
    case menuSysex:
        if (value > 0 && songSysExDump < (SONGS-1)) songSysExDump++;
          else if (value < 1 && songSysExDump > -2) songSysExDump--;
    break;
    //
    case menuSong:
      if (seqPlaying) printStopSequencer();
      else
      {
        checkIfDataNeedsSaving();
        //
        if (value > 0 && currentSong < (SONGS-1)) currentSong++;
          else if (value < 1 && currentSong > 0) currentSong--;
        //
        loadSong(currentSong);
        changedSong = true;
      }
    break;
    //
    case menuInit:
      if (value > 0 && initMode < 2) initMode++;
      else if (value < 1 && initMode > 0) initMode--;
    break;    
    //
    case menuToSongMode: 
        processMenuOK();
    break;
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void checkMenuClose()
{
  if (curRightScreen == kRightMenuCopyPaste || curRightScreen == kRightMenu) curRightScreen = kRightSteps;
}
