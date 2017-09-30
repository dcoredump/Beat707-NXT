/*
 * 
 * Created by William Kalfelz @ Beat707 (c) 2017 - http://www.Beat707.com
 * 
 */

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void send4BitsBytes(byte value, byte nbits)
{
  if (nbits == 4)
  {
    Serial.write(((value & B11110000) >> 4) & B00001111);
    Serial.write((value >> 4) & B00001111);
  }
  else
  {
    Serial.write(value);
  }
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
byte hasValueOver7Bits(void * data, byte nsize)
{
  for (int xpp = 0; xpp < nsize; xpp++) 
  { 
    if ((((byte*)data)[xpp]) > 0x7f) 
    {
      Serial.write(0x24);
      return 4; 
    }
  }
  //
  Serial.write(0x28);
  return 8;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool isPatternEmpty()
{
  for (byte x=0; x<STEPS; x++)
  {
    for (byte y=0; y<(DRUM_TRACKS-1); y++) { if (stepsData[x].steps[y] != 0) return false; }
    for (byte y=0; y<4; y++) { if (stepsData[x].stepsDouble[y] != 0) return false; }
    for (byte y=0; y<NOTE_TRACKS; y++) 
    { 
      if (stepsData[x].noteSteps[y][0] != 0) return false; 
      if (stepsData[x].noteSteps[y][1] != 0) return false; 
      if (stepsData[x].noteSteps[y][2] != 0) return false; 
      if (stepsData[x].noteSteps[y][3] != 0) return false; 
      if (stepsData[x].noteStepsExtras[y][0] != 0) return false; 
      if (stepsData[x].noteStepsExtras[y][1] != 0) return false; 
    }
  }
  //
  return true;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool isSongDataEmpty()
{
  for (byte x=0; x<SONG_POSITIONS; x++) { if (songData.pattern[0][x] != 0) return false; }
  //
  return true;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool isEntireSongEmpty()
{
  if (!isSongDataEmpty()) return false;
  //
  for (byte xd = 0; xd < PATTERNS; xd++)
  {
    loadPattern(xd);
    if (!isPatternEmpty()) return false;
  }
  return true;
}

int porc = 0;
byte porcDivider = 1;
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void updatePorc(byte value)
{
  porc += value;
  if (porcDivider == 1) showWaitMsg(porc*3); else showWaitMsg(porc/porcDivider);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void sysexDump(char song)
{
  if (song == -2) return;
  ////
  stopTimer();
  showWaitMsg(-1);
  porc = 0;
  byte prevSong = currentSong;
  byte prevPattern = currentPattern;
  byte nSongs = SONGS;
  byte songSelect = 0;
  porcDivider = 20;
  if (song >= 0) 
  {
    songSelect = song;
    nSongs = 1;
    porcDivider = 1;
  }
  //
  Serial.write(0xF0); // Sysex Start Code //
  Serial.write(0x01); // Sysex ID Code //
  Serial.write(0x02);
  Serial.write(0x28);
  if (song == -1) Serial.write(0x08); else Serial.write(0x04);
  //
  for (byte x = 0; x < nSongs; x++) // songs
  {
    loadSong(songSelect);
    songSelect++;
    //
    if (isEntireSongEmpty())
    {
      Serial.write(0x10);
      updatePorc(64);
    }
    else
    {
      Serial.write(0x12);
      byte nbits = hasValueOver7Bits(&configData, sizeof(configData));
      for (int xd = 0; xd < sizeof(configData); xd++) { send4BitsBytes(((byte*)&configData)[xd], nbits); }
      //
      if (isSongDataEmpty())
      {
        Serial.write(0x14);
      }
      else
      {
        Serial.write(0x16);
        byte nbits = hasValueOver7Bits(&songData, sizeof(songData));
        for (int xd = 0; xd < sizeof(songData); xd++) { send4BitsBytes(((byte*)&songData)[xd], nbits); }
      }
      //    
      for (byte xd = 0; xd < PATTERNS; xd++)
      {
        loadPattern(xd);
        if (isPatternEmpty())
        {
          Serial.write(0x18);
        }
        else
        {
          Serial.write(0x22);
          byte nbits = hasValueOver7Bits(&patternData, sizeof(patternData));
          for (int xpp = 0; xpp < sizeof(patternData); xpp++) { send4BitsBytes(((byte*)&patternData)[xpp], nbits); }
          nbits = hasValueOver7Bits(&stepsData, sizeof(stepsData));
          for (int xpp = 0; xpp < sizeof(stepsData); xpp++) { send4BitsBytes(((byte*)&stepsData)[xpp], nbits); }
        }
        //
        updatePorc(1);
      }
    }
  }
  //
  Serial.write(0x24); // End Of Message //
  Serial.write(0xF7); // Sysex End Code //
  loadSong(prevSong);
  loadPattern(currentPattern);
  showWaitMsg(-1);
  startTimer();
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void sysexReceive(char song)
{
  
}

