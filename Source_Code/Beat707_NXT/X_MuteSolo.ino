/*
 * 
 * Created by William Kalfelz @ Beat707 (c) 2017 - http://www.Beat707.com
 * 
 */

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void createScreenMuteSolo()
{
  leds[0] = leds[1] = leds[2] = 0;
  if (seqPlaying) bitSet(leds[0], (seqPosition/4)); else bitSet(leds[0], 0);
  bitSet(leds[0], variation + 4);
  if (seqPlaying) { if (seqPosition < 8) bitSet(leds[1], seqPosition); else bitSet(leds[2], seqPosition - 8); }
  //
  resetSegments(0, 2);
  if (showBPMdot) segments[2][7] |= B10000000;
  //
  for (byte x=0; x<8; x++)
  {
    for (byte i=0; i<3; i++)
    {
      if (i == 2 && x == 7) break;
      if (hasSoloTrack)
      {
        if (bitRead(patternData.soloTrack, x + (i *8)) == 1) segments[i][x] = S_S; else segments[i][x] = S__;
      }
      else
      {
        if (bitRead(patternData.muteTrack, x + (i *8)) == 1) segments[i][x] = S__; else segments[i][x] = S_N;
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
        if (i == 2 && x == 7) // Play Stop //
        {
          if (buttonEvent[i][x] == kButtonClicked)
          {
            if (seqPlaying) stopSequencer(); else startSequencer();
          }
          else
          {
            curRightScreen = kRightSteps;
          }
        }
        else
        {
          somethingChangedPattern = true;
          if (buttonEvent[i][x] == kButtonClicked)
          {
            if (bitRead(patternData.muteTrack, x + (i *8)) == 1) bitClear(patternData.muteTrack, x + (i *8)); else bitSet(patternData.muteTrack, x + (i *8));
          }
          else if (buttonEvent[i][x] == kButtonHold)
          {
            if (bitRead(patternData.soloTrack, x + (i *8)) == 1) bitClear(patternData.soloTrack, x + (i *8)); else bitSet(patternData.soloTrack, x + (i *8));
            checkSoloTracks();
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
    if (bitRead(patternData.soloTrack, x) == 1) 
    {
      hasSoloTrack = true;
      break;
    }
  }
}
