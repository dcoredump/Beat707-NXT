/*
 * 
 * Created by William Kalfelz @ Beat707 (c) 2017 - http://www.Beat707.com
 * 
 * Flash Page Size = 256 bytes
 * Capacity =  8388608 bytes
 * Number of Pages = 32768
 * 
 * Config: 1 page
 * Song: 1 page
 * Pattern Size: 2 pages
 * 
 * Page 0 is just the B707 header
 * Page 1 starts the data
 *                      Pg1    Config
 *                      Pg2    Song
 *                      Pg3    Pattern 01
 *                      Pg4    Pattern 01 (continued)
 *                                  ~
 *                             Pattern 64
 * Total of 130 pages per song
 * The problem is that the W25Q64 IC only erases pages in groups of 16. So we end up with only 2048 pages.
 * So we need a full 16 pages for the song data + config, them another 16 pages per pattern. Total of 1040 pages per song.
 * 
 */

#define FLASH_CHIPSIZE MB64
#define FLASH_VERSION '2'

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void flashInit(bool force)
{
  totalFlashErrors = 0;
  if (!flash.begin(FLASH_CHIPSIZE)) { showErrorMsg(127); }
  delay(80);
  if (!flash.readAnything(0, (uint8_t) 0, flashHeader)) showErrorMsg(flash.error());
  //
  byte lastSong = flashHeader[6];
  if (flashHeader[0] != 'B' || flashHeader[1] != '7' || flashHeader[2] != '0' || flashHeader[3] != '7' || flashHeader[4] != 'V' || flashHeader[5] != FLASH_VERSION) showErrorMsg(99);
  //
  #if INIT_FLASH_MEMORY
    force = true;
  #endif
  //
  if (force)
  {
    stopTimer();
    showWaitMsg(-1);
    delay(2000);
    lastSong = 0;
    #if INIT_ENTIRE_FLASH
      bool sectorErase = false;
      if (!flash.eraseChip()) sectorErase = true;
    #else
      bool sectorErase = true;
    #endif
    int porc = 0;
    //
    // Start Saving Songs/Patterns //
    for (byte x = 0; x < SONGS; x++) // songs
    {
      initSong(x, sectorErase, porc, false);
    }
    // Reset the Init Area //
    uint16_t pagePos = 16 + (SONGS * ((64 * 16) + 16)) + 16;
    if (sectorErase) eraseSector(pagePos);
    if (!flash.writeAnything(pagePos, (uint8_t) 0, patternData)) showErrorMsg(flash.error());
    pagePos++;
    if (!flash.writeAnything(pagePos, (uint8_t) 0, stepsData)) showErrorMsg(flash.error());
    //
    // Reset the Copy/Paste Area //
    pagePos = 16 + (SONGS * ((64 * 16) + 16));
    if (sectorErase) eraseSector(pagePos);
    if (!flash.writeAnything(pagePos, (uint8_t) 0, patternData)) showErrorMsg(flash.error());
    pagePos++;
    if (!flash.writeAnything(pagePos, (uint8_t) 0, stepsData)) showErrorMsg(flash.error());
    //
    // Finish //
    showWaitMsg(-1);
    saveHeader(lastSong, sectorErase);
    //
    #if DEBUG_SERIAL
      Serial.println(flashHeader);
    #endif  
    //    
    if (!flash.readAnything(0, (uint8_t) 0, flashHeader)) showErrorMsg(flash.error());
    if (flashHeader[0] != 'B' || flashHeader[1] != '7' || flashHeader[2] != '0' || flashHeader[3] != '7' || flashHeader[4] != 'V' || flashHeader[5] != FLASH_VERSION) showErrorMsg(98);
    //
    if (totalFlashErrors > 0)
    {
      showErrorMsg(totalFlashErrors, true);
      delay(4000);
      totalFlashErrors = 0;
    }
    //
    startTimer();
  }
  //
  loadSong(lastSong);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void saveHeader(byte theSong, bool sectorErase)
{
  createFlashHeader(theSong);
  if (sectorErase) eraseSector(0);
  if (!flash.writeAnything(0, (uint8_t) 0, flashHeader)) showErrorMsg(flash.error());
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void createFlashHeader(byte lastSong)
{
  flashHeader[0] = 'B';
  flashHeader[1] = '7';
  flashHeader[2] = '0';
  flashHeader[3] = '7';
  flashHeader[4] = 'V';
  flashHeader[5] = FLASH_VERSION;
  flashHeader[6] = lastSong;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void initSong(byte song, bool sectorErase, int &porc, bool songOnly)
{
  uint16_t pagePos = 16 + (song * ((64 * 16) + 16));
  //
  if (sectorErase) eraseSector(pagePos);
  if (!flash.writeAnything(pagePos, (uint8_t) 0, configData)) showErrorMsg(flash.error());
  pagePos++;
  if (!flash.writeAnything(pagePos, (uint8_t) 0, songData)) showErrorMsg(flash.error());
  pagePos += 15;
  //
  for (byte p = 0; p < PATTERNS; p ++)
  {
    if (sectorErase) eraseSector(pagePos);
    if (!flash.writeAnything(pagePos, (uint8_t) 0, patternData)) showErrorMsg(flash.error());
    pagePos++;
    if (!flash.writeAnything(pagePos, (uint8_t) 0, stepsData)) showErrorMsg(flash.error());
    pagePos += 15;
    porc++;
    if (songOnly) showWaitMsg(porc * 2); else showWaitMsg(byte(porc / 20));
  } 
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loadPattern(byte pattern, bool force = false)
{
  checkIfDataNeedsSaving();
  //
  if (!seqPlaying || force)
  {
    ignoreButtons = true;
    //
    uint16_t pagePos = 16 + (currentSong * ((64 * 16) + 16)) + (pattern * 16) + 16;
    if (!flash.readAnything(pagePos, (uint8_t) 0, patternData)) showErrorMsg(flash.error());
    pagePos++;
    if (!flash.readAnything(pagePos, (uint8_t) 0, stepsData)) showErrorMsg(flash.error());
    currentPattern = pattern;
    patternBitsSelector = 0;
    bitSet(patternBitsSelector, 8 + (currentPattern - ((currentPattern / 8) * 8)));
    bitSet(patternBitsSelector, (currentPattern / 8));
    checkSoloTracks();
    //
    ignoreButtons = false;
  }
  else
  {
    streamNextPattern = true;
    ignoreButtons = true;
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void checkPatternStream()
{
  if (loadPatternNow)
  {
    loadPattern(nextPattern, true);
    loadPatternNow = streamNextPattern = ignoreButtons = false;
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loadSong(byte song)
{
  currentPattern = 0;
  currentSong = song;
  loadPattern(0);
  //
  uint16_t pagePos = 16 + (currentSong * ((64 * 16) + 16));
  if (!flash.readAnything(pagePos, (uint8_t) 0, configData)) showErrorMsg(flash.error()); 
  pagePos++;
  if (!flash.readAnything(pagePos, (uint8_t) 0, songData)) showErrorMsg(flash.error());
}

