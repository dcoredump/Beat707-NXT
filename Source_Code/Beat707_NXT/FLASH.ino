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
#define FLASH_VERSION '1'

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void flashInit()
{
  if (!flash.begin(FLASH_CHIPSIZE)) { showErrorMsg(1); }
  //
  char header[6];
  if (!flash.readAnything(0, (uint8_t) 0, header)) showErrorMsg(flash.error());
  if (header[0] != 'B' || header[1] != '7' || header[2] != '0' || header[3] != '7' || header[4] != 'V' || header[5] != FLASH_VERSION)
  {
    showWaitMsg(-1);
    bool sectorErase = false;
    if (!flash.eraseChip()) sectorErase = true;
    int porc = 0;
    //
    // Start Saving Songs/Patterns //
    uint16_t pagePos = 16;
    for (byte x = 0; x < SONGS; x++) // songs
    {
      if (sectorErase && !flash.eraseSector(pagePos, 0)) showErrorMsg(flash.error());
      if (!flash.writeAnything(pagePos, (uint8_t) 0, configData)) showErrorMsg(flash.error()+10);
      pagePos++;
      if (!flash.writeAnything(pagePos, (uint8_t) 0, songData)) showErrorMsg(flash.error()+20);
      pagePos += 15;
      //
      for (byte p = 0; p < 64; p ++)
      {
        if (sectorErase && !flash.eraseSector(pagePos, 0)) showErrorMsg(flash.error());
        if (!flash.writeAnything(pagePos, (uint8_t) 0, patternData)) showErrorMsg(flash.error()+30);
        pagePos++;
        if (!flash.writeAnything(pagePos, (uint8_t) 0, stepsData)) showErrorMsg(flash.error()+40);
        pagePos += 15;
        porc++;
        showWaitMsg(byte(porc / 20));
      }
    }
    // Reset the Copy/Paste Area //
    if (sectorErase && !flash.eraseSector(pagePos, 0)) showErrorMsg(flash.error());
    if (!flash.writeAnything(pagePos, (uint8_t) 0, stepsData)) showErrorMsg(flash.error()+60);
    //
    // Reset the Init Area //
    pagePos += 16;
    if (sectorErase && !flash.eraseSector(pagePos, 0)) showErrorMsg(flash.error());
    if (!flash.writeAnything(pagePos, (uint8_t) 0, stepsData)) showErrorMsg(flash.error()+80);
    //
    // Finish //
    showWaitMsg(-1);
    header[0] = 'B';
    header[1] = '7';
    header[2] = '0';
    header[3] = '7';
    header[4] = 'V';
    header[5] = FLASH_VERSION;
    if (sectorErase && !flash.eraseSector(0, 0)) showErrorMsg(flash.error());
    if (!flash.writeAnything(0, (uint8_t) 0, header)) showErrorMsg(flash.error());
    if (!flash.readAnything(0, (uint8_t) 0, header)) showErrorMsg(flash.error());
    if (header[0] != 'B' || header[1] != '7' || header[2] != '0' || header[3] != '7' || header[4] != 'V' || header[5] != FLASH_VERSION) showErrorMsg(98);
  }
  //
  loadSong(0);
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

