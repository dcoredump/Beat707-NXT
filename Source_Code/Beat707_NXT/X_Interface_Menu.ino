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
    if (!flash.eraseSector(uint16_t(16 + (SONGS * ((64 * 16) + 16))), 0)) showErrorMsg(flash.error());
    if (!flash.writeAnything(uint16_t(16 + (SONGS * ((64 * 16) + 16))), (uint8_t) 0, stepsData)) showErrorMsg(flash.error());
  }
  else if (button >= 8 && button <= 11) // Paste
  {
    if (!flash.readAnything(uint16_t(16 + (SONGS * ((64 * 16) + 16))), (uint8_t) 0, stepsData)) showErrorMsg(flash.error());
    somethingChangedPattern = true;
  }
  else if (button >= 12 && button <= 15) // Init
  {
    if (!flash.readAnything(uint16_t(16 + (SONGS * ((64 * 16) + 16))) + 16, (uint8_t) 0, stepsData)) showErrorMsg(flash.error());
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
    case 0: segments[2][0] = S_N;
            segments[2][1] = S_N;
            segments[2][2] = S_C;
            segments[2][3] = S_H;
            //
            if (curTrack != 15) printNumber(2, 5, configData.trackMidiCH[curTrack] + 1);
              else printDashDash(2, 6);
    break;
    //
    case 1: segments[1][4] = S_N;
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
    case 2: 
    case 3:
    case 4: segments[1][0] = S_A;
            segments[1][1] = S_C;
            segments[1][2] = S_C;
            segments[1][3] = S_E;
            segments[1][4] = S_N;
            segments[1][5] = S_T;
            //
            segments[2][0] = S_U;
            segments[2][1] = numbers[menuPosition-2+1];
            //
            printNumber(2, 4, configData.accentValues[menuPosition-2]);
    break;
    //
    case 5: segments[1][0] = S_P;
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
    case 6: segments[2][0] = S_U;
            segments[2][1] = S_A;
            segments[2][2] = S_R;
            //
            segments[2][4] = S_A;
            if (patternData.totalVariations >= 2) segments[2][5] = S_b;
            if (patternData.totalVariations >= 3) segments[2][6] = S_C;
            if (patternData.totalVariations >= 4) segments[2][7] = S_d;
    break;
    //
    case 7: segments[1][0] = S_S;
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
    case 8: segments[2][0] = S_S;
            segments[2][1] = S_O;
            segments[2][2] = S_N;
            segments[2][3] = S_G;
            //
            printNumber(2, 5, currentSong + 1);
    break;
    //
    case 9: segments[1][1] = S_T;
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
void processMenu(char value)
{
  switch(menuPosition)
  {
    case 0:
      if (value > 0 && configData.trackMidiCH[curTrack] < 15) configData.trackMidiCH[curTrack]++;
      else if (value < 0 && configData.trackMidiCH[curTrack] > 0) configData.trackMidiCH[curTrack]--;
      somethingChangedConfig = true;
    break;
    //
    case 1:
      if (curTrack < DRUM_TRACKS)
      {
        if (value > 0 && configData.trackNote[curTrack] < 127) configData.trackNote[curTrack]++;
          else if (value < 0 && configData.trackNote[curTrack] > 0) configData.trackNote[curTrack]--;
        somethingChangedConfig = true;
      }
    break;
    //
    case 2:
    case 3:
    case 4:
        if (value > 0 && configData.accentValues[menuPosition-2] < 127) configData.accentValues[menuPosition-2]++;
          else if (value < 1 && configData.accentValues[menuPosition-2] > 0) configData.accentValues[menuPosition-2]--;
        somethingChangedConfig = true;
    break;
    //
    case 5:
        if (value > 0 && patternData.trackProcessor[curTrack] < 7) patternData.trackProcessor[curTrack]++;
          else if (value < 1 && patternData.trackProcessor[curTrack] > 0) patternData.trackProcessor[curTrack]--;
        somethingChangedPattern = true;
    break;
    //
    case 6:
        if (value > 0 && patternData.totalVariations < 4) patternData.totalVariations++;
          else if (value < 1 && patternData.totalVariations > 1) patternData.totalVariations--;
        somethingChangedPattern = true;
    break;
    //
    case 7:
      configData.seqSyncOut = !configData.seqSyncOut;
      somethingChangedConfig = true;
    break;
    //
    case 8:
      if (!seqPlaying)
      {
        checkIfDataNeedsSaving();
        //
        if (value > 0 && currentSong < (SONGS-1)) currentSong++;
          else if (value < 1 && currentSong > 0) currentSong--;
        //
        loadSong(currentSong);
      }
    break;
    //
    case 9:
      if (!seqPlaying)
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
void checkMenuClose()
{
  if (curRightScreen == kRightMenuCopyPaste || curRightScreen == kRightMenu) curRightScreen = kRightSteps;
}
