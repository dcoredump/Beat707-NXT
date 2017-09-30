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
    uint16_t pagePos = 16 + (currentSong * ((64 * 16) + 16)) + (currentPattern * 16) + 16;
    if (!flash.eraseSector(pagePos, 0)) showErrorMsg(flash.error());
    if (!flash.writeAnything(pagePos, (uint8_t) 0, patternData)) showErrorMsg(flash.error());   
    pagePos++;
    if (!flash.writeAnything(pagePos, (uint8_t) 0, stepsData)) showErrorMsg(flash.error());    
  }
  // -------=========---------- //
  if (somethingChangedConfig)
  {
    somethingChangedConfig = false;
    uint16_t pagePos = 16 + (currentSong * ((64 * 16) + 16));
    if (!flash.eraseSector(pagePos, 0)) showErrorMsg(flash.error());
    if (!flash.writeAnything(pagePos, (uint8_t) 0, configData)) showErrorMsg(flash.error());
    pagePos++;
    if (!flash.writeAnything(pagePos, (uint8_t) 0, songData)) showErrorMsg(flash.error());
  }
  //
  if (changedSong)
  {
    changedSong = false;
    createFlashHeader(currentSong);
    if (!flash.eraseSector(0, 0)) showErrorMsg(flash.error());
    if (!flash.writeAnything(0, (uint8_t) 0, flashHeader)) showErrorMsg(flash.error());    
  }
}

