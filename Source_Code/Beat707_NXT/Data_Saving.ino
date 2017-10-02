/*
 * 
 * Created by William Kalfelz @ Beat707 (c) 2017 - http://www.Beat707.com
 * 
 */

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void checkIfDataNeedsSaving()
{
  if (somethingChangedPattern)
  {
    somethingChangedPattern = false;
    savePatternData(true);
    saveStepsData();
  }
  // -------=========---------- //
  if (somethingChangedConfig)
  {
    somethingChangedConfig = false;
    saveConfigData(true);
    saveSongData();
  }
  //
  if (changedSong)
  {
    changedSong = false;
    saveHeader(currentSong, true);
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void eraseSector(uint16_t pagePos)
{
  if (!flash.eraseSector(pagePos, 0)) showErrorMsg(flash.error());  
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void saveConfigData(byte _eraseSector)
{
  if (_eraseSector) eraseSector(16 + (currentSong * ((64 * 16) + 16)));
  if (!flash.writeAnything(16 + (currentSong * ((64 * 16) + 16)), (uint8_t) 0, configData)) showErrorMsg(flash.error());  
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void saveSongData()
{
  if (!flash.writeAnything(16 + 1 + (currentSong * ((64 * 16) + 16)), (uint8_t) 0, songData)) showErrorMsg(flash.error());
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void savePatternData(byte _eraseSector)
{
  if (_eraseSector) eraseSector(16 + (currentSong * ((64 * 16) + 16)) + (currentPattern * 16) + 16);
  if (!flash.writeAnything(16 + (currentSong * ((64 * 16) + 16)) + (currentPattern * 16) + 16, (uint8_t) 0, patternData)) showErrorMsg(flash.error());   
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void saveStepsData()
{
  if (!flash.writeAnything(16 + 1 + (currentSong * ((64 * 16) + 16)) + (currentPattern * 16) + 16, (uint8_t) 0, stepsData)) showErrorMsg(flash.error());
}
