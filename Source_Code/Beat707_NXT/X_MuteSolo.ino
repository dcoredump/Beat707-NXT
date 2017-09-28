/*
 * 
 * Created by William Kalfelz @ Beat707 (c) 2017 - http://www.Beat707.com
 * 
 */

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void createScreenMuteSolo()
{
  leds[0] = leds[1] = leds[2] = 0;
  //
  resetSegments(0, 2);
  if (streamNextPattern || loadPatternNow) segments[2][7] = S_L;
  if (showBPMdot) segments[2][7] |= B10000000;
  //
  if (forceAccent)
  {
    leds[0] = patternBitsSelector;
    leds[1] = patternBitsSelector >> 8;   
    //
    segments[0][0] = S_P;
    if (nextPattern != currentPattern) printNumber(0, 1, nextPattern + 1); else printNumber(0, 1, currentPattern + 1);   
    segments[0][1] = S_T;
    if (streamNextPattern || loadPatternNow) segments[0][3] |= B10000000;
    //
    segments[2][0] = S_P;
    segments[2][2] = S_N;
    segments[2][4] = S_C;
    segments[2][5] = S_L;
    segments[2][6] = S_S;
  }
  else
  {
    if (seqPlaying) bitSet(leds[0], (seqPosition/4)); else bitSet(leds[0], 0);
    bitSet(leds[0], variation + 4);
    if (seqPlaying) { if (seqPosition < 8) bitSet(leds[1], seqPosition); else bitSet(leds[2], seqPosition - 8); }
    //
    for (byte x=0; x<8; x++)
    {
      for (byte i=0; i<3; i++)
      {
        if (i == 2 && x == 7) break;
        if (hasSoloTrack)
        {
          if (bitRead(configData.soloTrack, x + (i *8)) == 1) segments[i][x] = S_S; else segments[i][x] = S__;
          if (bitRead(configData.muteTrack, x + (i *8)) == 0) segments[i][x] |= B10000000;
        }
        else
        {
          if (bitRead(configData.muteTrack, x + (i *8)) == 1) segments[i][x] = S__; else segments[i][x] = S_N;
        }
      }
    }
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void checkInterfaceMuteSolo()
{
  for (byte x=0; x<8; x++)
  {
    for (byte i=0; i<3; i++)
    {
      if (buttonEvent[i][x] >= kButtonClicked)
      {
        if (forceAccent)
        {
          if (i == 2 && x == 7 && buttonEvent[i][x] >= kButtonRelease) 
          {
            forceAccent = false;
            if (currentPattern != nextPattern) loadPattern(nextPattern);
          }
          else if (i == 2 && (x == 0 || x == 1))
          {
            if (nextPattern > 0) nextPattern--;
          }
          else if (i == 2 && (x == 2 || x == 3))
          {
            if (nextPattern < PATTERNS) nextPattern++;
          }
          else if (i == 2 && (x > 3))
          {
            forceAccent = false;
            if (currentPattern != nextPattern) loadPattern(nextPattern);
            curRightScreen = kRightSteps;
          }
          else if (i <= 1)
          {
            byte leButton = x + (i * 8);
            checkIfDataNeedsSaving();
            if (leButton < 8) patternBitsSelector &= 0xFF00; else patternBitsSelector &= 0xFF;
            bitSet(patternBitsSelector, leButton);
            //
            nextPattern = 0;
            for (byte x = 0; x < 8; x++)
            {
              if (bitRead(patternBitsSelector, x + 8)) nextPattern += x;
              if (bitRead(patternBitsSelector, x)) nextPattern += (x * 8);
            }
          }
        }
        else
        {
          if (i == 2 && x == 7) // Play Stop //
          {
            if (buttonEvent[i][x] == kButtonClicked) { if (seqPlaying) stopSequencer(); else startSequencer(); }
            else if (buttonEvent[i][x] == kButtonHold) forceAccent = true;
            else if (buttonEvent[i][x] >= kButtonRelease) forceAccent = false;
              //curRightScreen = kRightSteps;
          }
          else
          {
            somethingChangedConfig = true;
            if (buttonEvent[i][x] == kButtonClicked)
            {
              if (bitRead(configData.muteTrack, x + (i *8)) == 1) bitClear(configData.muteTrack, x + (i *8)); else bitSet(configData.muteTrack, x + (i *8));
            }
            else if (buttonEvent[i][x] == kButtonHold)
            {
              if (bitRead(configData.soloTrack, x + (i *8)) == 1) bitClear(configData.soloTrack, x + (i *8)); else bitSet(configData.soloTrack, x + (i *8));
              checkSoloTracks();
            }
          }
        }
        //
        buttonEvent[i][x] = 0;
        updateScreen = true;
      }
    }
  }  
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void checkSoloTracks()
{
  hasSoloTrack = false;
  for (byte x=0; x<(NOTE_TRACKS + DRUM_TRACKS - 1); x++)
  {
    if (bitRead(configData.soloTrack, x) == 1) 
    {
      hasSoloTrack = true;
      break;
    }
  }
}
