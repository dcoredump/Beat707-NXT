/*
 * 
 * Created by William Kalfelz @ Beat707 (c) 2017 - http://www.Beat707.com
 * 
 */

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void createScreen()
{
  byte xVar = variation;
  if (forceVariation >= 0) xVar = forceVariation;
  //
  leds[0] = leds[1] = leds[2] = 0;
  if (seqPlaying) bitSet(leds[0], (seqPosition/4)); else bitSet(leds[0], 0);
  //
  if (mirror) leds[0] |= B11110000; 
  else 
  {
    if (forceVariation >= 0)
    {
      leds[0] |= B11110000;
      bitClear(leds[0], xVar + 4);
    }
    else bitSet(leds[0], xVar + 4);
  }
  //
  leds[1] = leds[2] = 0;
  if (curRightScreen == kRightPatternSelection)
  {
    leds[1] = patternBitsSelector;
    leds[2] = patternBitsSelector >> 8;
  }
  else
  {
    if (curTrack < DRUM_TRACKS)
    {
      if (curTrack < 8) bitSet(leds[1], curTrack); else bitSet(leds[2], curTrack - 8);
    }
    else
    {
      leds[1] = 0xFF;
      bitClear(leds[1], curTrack - DRUM_TRACKS);
    }
    //
    if (seqPlaying && curRightScreen != kRightTrackSelection)
    {
      if ((seqPosition < 8 && bitRead(leds[1], seqPosition)) ||
          (seqPosition >= 8 && bitRead(leds[2], seqPosition)))
      {
        if (seqPosition < 8) bitClear(leds[1], seqPosition); else bitClear(leds[2], seqPosition - 8);
      }
      else
      {
        if (seqPosition < 8) bitSet(leds[1], seqPosition); else bitSet(leds[2], seqPosition - 8);
      }
    }
  }
  //
  if (curLeftScreen == kLeftMain)
  {
    segments[0][0] = S_b;
    printNumber(0, 1, configData.BPM);
    if (showBPMdot) segments[0][3] |= B10000000;
    segments[0][4] = S_P;
    if (nextPattern != currentPattern) printNumber(0, 5, nextPattern + 1); else printNumber(0, 5, currentPattern + 1);
    segments[0][5] = S_T;
    if (streamNextPattern || loadPatternNow) segments[0][7] |= B10000000;
  }
  //
  memset(segments[1], 0, sizeof(segments[1]));
  memset(segments[2], 0, sizeof(segments[2]));
  //
  if (curRightScreen == kRightMenuCopyPaste)
  {
    showMenuCopyPaste();
  }
  else if (curRightScreen == kRightMenu)
  {
    showMenu();
  }
  else if (curRightScreen == kRightSteps || 
      curRightScreen == kRightTrackSelection ||
      curRightScreen == kRightABCDVariations ||
      curRightScreen == kRightPatternSelection)
  {
    if (curTrack < DRUM_TRACKS)
    {
      for (byte xs=0; xs < 2; xs++)
      {
        for (byte x=0; x < 8; x++)
        {       
          byte xc = bitRead(stepsData[x + (xs*8)].steps[curTrack], 1 + (xVar * 2)) << 1;
          xc |= bitRead(stepsData[x + (xs*8)].steps[curTrack], 0 + (xVar * 2));
          //
          if (curTrack == 15)
          {
            segments[xs + 1][x] = stepChars[xc];
          }
          else
          {
            if (xc == 1) // use accent track
            {
              byte xc = bitRead(stepsData[x + (xs*8)].steps[15], 1 + (xVar * 2)) << 1;
              xc |= bitRead(stepsData[x + (xs*8)].steps[15], 0 + (xVar * 2));
              segments[xs + 1][x] = stepChars[xc];
            }
            else if (xc == 3)
            {
              segments[xs + 1][x] = stepChars[3];
            }
            else if (xc == 2)
            {
              segments[xs + 1][x] = stepChars[1];
            }
            //
            if (bitRead(stepsData[x + (xs*8)].stepsDouble[xVar], curTrack)) segments[xs + 1][x] |= B10000000;
          }
        }
      }
    }
    else // NOTE TRACKS ---===----==---==-- //
    {
      if (editingNote)
      {
        // 1234 1234 1234 1234
        // GLDE      C# 1   V1
        // DBLE      C# 1   V1
        //                 OFF
        //
        byte xm = bitRead(stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][1], 1 + (editVariation * 2)) << 1;
        xm |= bitRead(stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][1], 0 + (editVariation * 2));
        //
        if (xm == 3)
        {
          segments[2][5] = S_O;
          segments[2][6] = S_F;
          segments[2][7] = S_F;
        }
        else if (stepsData[editStep].noteSteps[curTrack-DRUM_TRACKS][editVariation] > 0)
        {
          printMIDInote(stepsData[editStep].noteSteps[curTrack-DRUM_TRACKS][editVariation], 2, 0, 3);
          byte xc = bitRead(stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][0], 1 + (editVariation * 2)) << 1;
          xc |= bitRead(stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][0], 0 + (editVariation * 2));
          segments[2][6] = S_U;
          segments[2][7] = numbers[xc];
          //
          if (xm == 1) 
          {
            segments[1][0] = S_G;
            segments[1][1] = S_L;
            segments[1][2] = S_d;
            segments[1][3] = S_E;
          }
          else if (xm == 2) 
          {
            segments[1][0] = S_d;
            segments[1][1] = S_b;
            segments[1][2] = S_L;
            segments[1][3] = S_E;
          }
        }
        else
        {
          for (byte xs=0; xs < 8; xs++) { segments[2][xs + 8] = B01000000; }
        }
      }
      else
      {
        for (byte xs=0; xs < 2; xs++)
        {
          for (byte x=0; x < 8; x++)
          {
            byte xm = bitRead(stepsData[x + (xs*8)].noteStepsExtras[curTrack-DRUM_TRACKS][1], 1 + (xVar * 2)) << 1;
            xm |= bitRead(stepsData[x + (xs*8)].noteStepsExtras[curTrack-DRUM_TRACKS][1], 0 + (xVar * 2));
            //
            if (xm != 3 && stepsData[x + (xs*8)].noteSteps[curTrack-DRUM_TRACKS][xVar] > 0)
            {
              byte xc = bitRead(stepsData[x + (xs*8)].noteStepsExtras[curTrack-DRUM_TRACKS][0], 1 + (xVar * 2)) << 1;
              xc |= bitRead(stepsData[x + (xs*8)].noteStepsExtras[curTrack-DRUM_TRACKS][0], 0 + (xVar * 2));
              segments[xs + 1][x] = stepChars[xc];
            }
            //
            if (xm == 1) segments[xs + 1][x] |= B01000000;
            else if (xm == 2) segments[xs + 1][x] |= B10000000;
            else if (xm == 3) segments[xs + 1][x] |= B01000000;            
          }
        }       
      }
    }
  }
}

/*
 * 0 Play
 * 1 Stop
 * 2 (-) >
 * 3 (+) > change tempo or any Menu option
 * 4 Force Accent
 * 5 ABCD -> Mirror Mode
 * 6 Pattern Selection  >
 * 7 Track Selection    > Both = Menu
 */

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void checkInterface()
{
  if (!somethingHappened) return;
  somethingHappened = false;
  //
  // Start Sequencer //
  if (buttonEvent[0][0] >= kButtonClicked)
  {
    buttonEvent[0][0] = 0;
    startSequencer();
  }
  //
  // Stop Sequencer //
  if (buttonEvent[0][1] >= kButtonClicked)
  {
    buttonEvent[0][1] = 0;
    stopSequencer();
  }
  //
  // (-) BPM or Menu  //
  if (buttonEvent[0][2] >= kButtonClicked)
  {
    if (buttonEvent[0][2] == kButtonClicked)
    {
      if (curRightScreen == kRightMenu)
      {
        processMenu(-1);
      }
      else
      {
        if (configData.BPM > 10) 
        { 
          configData.BPM--;
          somethingChangedConfig = true;
        }
        updateSequencerSpeed();
      }
      updateScreen = true;
    }
    buttonEvent[0][2] = 0;
  }
  // (+) BPM or Menu  //
  if (buttonEvent[0][3] >= kButtonClicked)
  {
    if (buttonEvent[0][3] == kButtonClicked)
    {
      if (curRightScreen == kRightMenu)
      {
        processMenu(1);
      }
      else
      {
        if (configData.BPM < 255)
        {
          configData.BPM++;
          somethingChangedConfig = true;
        }
        updateSequencerSpeed();
      }
      updateScreen = true;
    }
    buttonEvent[0][3] = 0;
  }
  //
  if (curRightScreen == kRightSteps) // Regular Steps Editing
  {
    byte xVar = variation;
    if (forceVariation >= 0) xVar = forceVariation;
    if (mirror) xVar = 0;
    //
    if (curTrack < DRUM_TRACKS)
    {
      for (byte x=0; x<8; x++)
      {
        for (byte i=0; i<2; i++)
        {
          if (buttonEvent[i + 1][x] > kButtonNone)
          {
            byte xc = bitRead(stepsData[x + (i*8)].steps[curTrack], 1 + (xVar * 2)) << 1;
            xc |= bitRead(stepsData[x + (i*8)].steps[curTrack], 0 + (xVar * 2));
            //
            if (buttonEvent[i + 1][x] == kButtonClicked)
            {
              bitClear(stepsData[x + (i*8)].steps[curTrack], (xVar * 2));
              bitClear(stepsData[x + (i*8)].steps[curTrack], (xVar * 2) + 1);
              //
              somethingChangedPattern = true;
              //
              if (curTrack == 15)
              {
                if (xc == 0) xc = 1; 
                else if (xc == 1) xc = 2;
                else if (xc == 2) xc = 3;
                else if (xc == 3) xc = 1;
                stepsData[x + (i*8)].steps[curTrack] |= xc << (xVar * 2);
              }
              else
              {
                if (forceAccent)
                {
                  if (xc == 3) xc = 2;
                  else xc = 3;
                }
                else
                {
                  if (xc == 0) xc = 1; else xc = 0;
                }
                stepsData[x + (i*8)].steps[curTrack] |= xc << (xVar * 2);
              }
              //
              if (mirror)
              {
                stepsData[x + (i*8)].steps[curTrack] = 0x00;
                stepsData[x + (i*8)].steps[curTrack] |= xc;
                stepsData[x + (i*8)].steps[curTrack] |= xc << 2;
                stepsData[x + (i*8)].steps[curTrack] |= xc << 4;
                stepsData[x + (i*8)].steps[curTrack] |= xc << 6;
              }
            }
            else if (buttonEvent[i + 1][x] == kButtonHold && curTrack != 15)
            {
              somethingChangedPattern = true;
              //
              if (bitRead(stepsData[x + (i*8)].stepsDouble[xVar], curTrack))
              {
                bitClear(stepsData[x + (i*8)].stepsDouble[xVar], curTrack);
                if (mirror)
                {
                  bitClear(stepsData[x + (i*8)].stepsDouble[1], curTrack);
                  bitClear(stepsData[x + (i*8)].stepsDouble[2], curTrack);
                  bitClear(stepsData[x + (i*8)].stepsDouble[3], curTrack);
                }
              }
              else
              {
                bitSet(stepsData[x + (i*8)].stepsDouble[xVar], curTrack);
                if (mirror)
                {
                  bitSet(stepsData[x + (i*8)].stepsDouble[1], curTrack);
                  bitSet(stepsData[x + (i*8)].stepsDouble[2], curTrack);
                  bitSet(stepsData[x + (i*8)].stepsDouble[3], curTrack);
                }
              }
            }
            //
            buttonEvent[i + 1][x] = 0;
            updateScreen = true;
          }
        }
      }
    }
    else // NOTE TRACKS ---===----==---==-- //
    {
      for (byte x=0; x<8; x++)
      {
        for (byte i=0; i<2; i++)
        {
          if (buttonEvent[i + 1][x] > kButtonNone)
          {
            if (editingNote)
            {
              if (i == 1 && x == 7)  // Ok, close //
              {
                if (buttonEvent[i + 1][x] == kButtonClicked) editingNote = false;
              }
              else if (i == 1 && x == 6) // Octave
              {
                if (buttonEvent[i + 1][x] == kButtonClicked)
                {
                  uint16_t xNote = uint16_t(stepsData[editStep].noteSteps[curTrack-DRUM_TRACKS][editVariation]) + 12;
                  if (xNote > 108) xNote = uint16_t(stepsData[editStep].noteSteps[curTrack-DRUM_TRACKS][editVariation]) - ((uint16_t(stepsData[editStep].noteSteps[curTrack-DRUM_TRACKS][editVariation]) / 12) * 12);
                  if (xNote <= 0) xNote = 12;
                  stepsData[editStep].noteSteps[curTrack-DRUM_TRACKS][editVariation] = byte(xNote & B01111111);
                  patternData.lastNote[curTrack-DRUM_TRACKS] = stepsData[editStep].noteSteps[curTrack-DRUM_TRACKS][editVariation];
                  checkIfMirrorAndCopy(editStep);
                }
              }
              else if (i == 1 && x == 5) // Velocity
              {
                if (buttonEvent[i + 1][x] == kButtonClicked)
                {
                  byte xc = bitRead(stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][0], 1 + (editVariation * 2)) << 1;
                  xc |= bitRead(stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][0], 0 + (editVariation * 2));
                  //
                  bitClear(stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][0], (editVariation * 2));
                  bitClear(stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][0], (editVariation * 2) + 1);               
                  //
                  if (xc == 1) xc = 2;
                  else if (xc == 2) xc = 3;
                  else xc = 1;
                  lastVelocity = xc;
                  //
                  stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][0] |= xc << (editVariation * 2);
                  checkIfMirrorAndCopy(editStep);
                }
              }
              else if (i == 1 && x == 4) // Type (Glide or NoteOff / Hold=Double)
              {
                if (buttonEvent[i + 1][x] == kButtonClicked)
                {
                  byte xc = bitRead(stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][1], 1 + (editVariation * 2)) << 1;
                  xc |= bitRead(stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][1], 0 + (editVariation * 2));
                  //
                  bitClear(stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][1], (editVariation * 2));
                  bitClear(stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][1], (editVariation * 2) + 1);               
                  //
                  if (xc == 0) xc = 1;
                  else if (xc == 1) xc = 2;
                  else if (xc == 2) xc = 3;
                  else xc = 0;
                  //
                  stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][1] |= xc << (editVariation * 2);
                  checkIfMirrorAndCopy(editStep);                
                }
              }
              else // The other first 12 buttons for notes //
              {
                if (buttonEvent[i + 1][x] == kButtonClicked)
                {
                  byte xOct = stepsData[editStep].noteSteps[curTrack-DRUM_TRACKS][editVariation] / 12;
                  stepsData[editStep].noteSteps[curTrack-DRUM_TRACKS][editVariation] = (xOct * 12) + (x + (i * 8));
                  patternData.lastNote[curTrack-DRUM_TRACKS] = stepsData[editStep].noteSteps[curTrack-DRUM_TRACKS][editVariation];
                  checkIfMirrorAndCopy(editStep);
                }
              }
            }
            else // Not Editing Note //
            {
              somethingChangedPattern = true;
              //
              editStep = x + (i * 8);
              editVariation = xVar;
              //
              if (buttonEvent[i + 1][x] == kButtonClicked)
              {
                if (stepsData[editStep].noteSteps[curTrack-DRUM_TRACKS][editVariation] == 0 && patternData.lastNote[curTrack-DRUM_TRACKS] == 0)
                {
                  clearStepsExtrasBits(editStep, editVariation);
                  editingNote = true;
                  patternData.lastNote[curTrack-DRUM_TRACKS] = 60;
                  stepsData[editStep].noteSteps[curTrack-DRUM_TRACKS][editVariation] = patternData.lastNote[curTrack-DRUM_TRACKS];
                  stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][0] |= lastVelocity << (editVariation * 2); 
                }
                else if (stepsData[editStep].noteSteps[curTrack-DRUM_TRACKS][editVariation] == 0 && patternData.lastNote[curTrack-DRUM_TRACKS] > 0)
                {
                  clearStepsExtrasBits(editStep, editVariation);
                  stepsData[editStep].noteSteps[curTrack-DRUM_TRACKS][editVariation] = patternData.lastNote[curTrack-DRUM_TRACKS];
                  stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][0] |= lastVelocity << (editVariation * 2);
                }
                else if (stepsData[editStep].noteSteps[curTrack-DRUM_TRACKS][editVariation] > 0)
                {
                  byte xc = bitRead(stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][0], 1 + (editVariation * 2)) << 1;
                  xc |= bitRead(stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][0], 0 + (editVariation * 2));
                  byte xc2 = bitRead(stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][1], 1 + (editVariation * 2)) << 1;
                  xc2 |= bitRead(stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][1], 0 + (editVariation * 2));
                  clearStepsExtrasBits(editStep, editVariation);
                  //
                  if (xc == 3) xc = 2;
                  else if (xc == 2) xc = 1;
                  else
                  {
                    xc = 0;
                    patternData.lastNote[curTrack-DRUM_TRACKS] = stepsData[editStep].noteSteps[curTrack-DRUM_TRACKS][editVariation];
                    stepsData[editStep].noteSteps[curTrack-DRUM_TRACKS][editVariation] = 0;
                  }
                  //
                  stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][0] |= xc << (editVariation * 2);
                  stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][1] |= xc2 << (editVariation * 2);
                }
                //
                checkIfMirrorAndCopy(editStep);
              }
              else if (buttonEvent[i + 1][x] == kButtonHold)
              {
                editingNote = true;
                //
                if (stepsData[editStep].noteSteps[curTrack-DRUM_TRACKS][editVariation] == 0)
                {
                  clearStepsExtrasBits(editStep, editVariation);
                  //
                  if (patternData.lastNote[curTrack-DRUM_TRACKS] == 0) patternData.lastNote[curTrack-DRUM_TRACKS] = 60;
                  stepsData[editStep].noteSteps[curTrack-DRUM_TRACKS][editVariation] = patternData.lastNote[curTrack-DRUM_TRACKS];
                  stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][0] |= lastVelocity << (editVariation * 2);
                  //
                  checkIfMirrorAndCopy(editStep);
                }
              }
            }
            buttonEvent[i + 1][x] = 0;
            updateScreen = true;
          }
        }
      }      
    }
  }
  //
  // ---===------===------===------===------===------===------===------===---
  //
  if (curRightScreen == kRightTrackSelection ||
      curRightScreen == kRightABCDVariations ||
      curRightScreen == kRightPatternSelection ||
      curRightScreen == kRightMenuCopyPaste ||
      curRightScreen == kRightMenu)
  {
    char leButton = -1;
    for (byte x=0; x<8; x++)
    {
      for (byte i=0; i<2; i++)
      {
        if (buttonEvent[i + 1][x] == kButtonClicked)
        {
          buttonEvent[i + 1][x] = 0;
          updateScreen = true;
          leButton = x + (i * 8);
        }
      }
    }
    //
    if (leButton >= 0)
    {
      if (curRightScreen == kRightMenu && menuPosition == 9)
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
      else if (curRightScreen == kRightTrackSelection ||  curRightScreen == kRightMenu)
      {
        editingNote = false;
        //
        if (curTrack == leButton)
        {
          if (leButton < 8) curTrack = leButton + DRUM_TRACKS;
        }
        else curTrack = leButton;
      }
      else if (curRightScreen == kRightABCDVariations)
      {
        if (leButton <= 3) forceVariation = leButton; else forceVariation = -1;
        mirror = false;
      }
      else if (curRightScreen == kRightMenuCopyPaste)
      {
        processMenuCopyPaste(leButton);
      }
      else if (curRightScreen == kRightPatternSelection)
      {
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
  }
  //  
  // ---===------===------===------===------===------===------===------===---
  //
  if (buttonEvent[0][5] == kButtonClicked)
  {
    checkMenuClose();
    updateScreen = true;
    buttonEvent[0][5] = 0;
    if (forceVariation >= 0)
    {
      mirror = false;
      forceVariation = -1;    
    }
    else
    {
      mirror = !mirror;
      forceVariation = -1;
    }
  }
  else if (buttonEvent[0][5] == kButtonHold)
  {
    checkMenuClose();
    buttonEvent[0][5] = 0;
    curRightScreen = kRightABCDVariations;
  }
  else if (buttonEvent[0][5] > kButtonHold)
  {
    updateScreen = true;
    buttonEvent[0][5] = 0;
    curRightScreen = kRightSteps;
  }
  //
  // ----------============------------
  //
  if (buttonEvent[0][7] == kButtonClicked)
  {
    updateScreen = true;
    buttonEvent[0][7] = 0;
  }
  else if (buttonEvent[0][7] == kButtonHold)
  {
    updateScreen = true;
    buttonEvent[0][7] = 0;
    if (curRightScreen == kRightMenu)
    {
      menuPosition++;
      initMode = 0;
      if (menuPosition > lastMenu) menuPosition = 0;
    }
    else if (forceAccent)
    {
      curRightScreen = kRightMenu;
      initMode = menuPosition = 0;
    }
    else curRightScreen = kRightTrackSelection;
  }
  else if (buttonEvent[0][7] > kButtonHold)
  {
    if (curRightScreen == kRightTrackSelection) curRightScreen = kRightSteps;
    updateScreen = true;
    buttonEvent[0][7] = 0;
  }
  //
  // ----------============------------
  //
  if (buttonEvent[0][4] == kButtonHold)
  {
    checkMenuClose();
    buttonEvent[0][4] = 0;
    forceAccent = true;
  }
  else if (buttonEvent[0][4] > kButtonHold ||
           buttonEvent[0][4] == kButtonClicked)
  {
    forceAccent = false;
    buttonEvent[0][4] = 0;
    updateScreen = true;
  }
  //
  // ----------============------------
  //
  if (buttonEvent[0][6] == kButtonHold)
  {
    if (curRightScreen == kRightSteps) curRightScreen = kRightPatternSelection;
    buttonEvent[0][6] = 0;
    updateScreen = true;
  }
  else if (buttonEvent[0][6] > kButtonHold ||
           buttonEvent[0][6] == kButtonClicked)
  {
    if (curRightScreen == kRightMenu)
    {
      if (menuPosition > 0) menuPosition--;
      else menuPosition = lastMenu;
      initMode = 0;
    }    
    else if (forceAccent)
    {
      curRightScreen = kRightMenuCopyPaste;
    }
    else
    {
      if (buttonEvent[0][6] == kButtonReleaseNothingClicked
          && currentPattern < 64) nextPattern = currentPattern + 1;
      curRightScreen = kRightSteps;
      if (currentPattern != nextPattern) loadPattern(nextPattern);
    }
    //
    buttonEvent[0][6] = 0;
    updateScreen = true;
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void checkIfMirrorAndCopy(byte thestep)
{
  if (mirror)
  {
    stepsData[thestep].noteSteps[curTrack-DRUM_TRACKS][1] = stepsData[thestep].noteSteps[curTrack-DRUM_TRACKS][0];
    stepsData[thestep].noteSteps[curTrack-DRUM_TRACKS][2] = stepsData[thestep].noteSteps[curTrack-DRUM_TRACKS][0];
    stepsData[thestep].noteSteps[curTrack-DRUM_TRACKS][3] = stepsData[thestep].noteSteps[curTrack-DRUM_TRACKS][0];
    //
    stepsData[thestep].noteStepsExtras[curTrack-DRUM_TRACKS][0] &= B00000011;
    stepsData[thestep].noteStepsExtras[curTrack-DRUM_TRACKS][0] |= stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][0] << 2;
    stepsData[thestep].noteStepsExtras[curTrack-DRUM_TRACKS][0] |= stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][0] << 4;
    stepsData[thestep].noteStepsExtras[curTrack-DRUM_TRACKS][0] |= stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][0] << 6;
    //
    stepsData[thestep].noteStepsExtras[curTrack-DRUM_TRACKS][1] &= B00000011;
    stepsData[thestep].noteStepsExtras[curTrack-DRUM_TRACKS][1] |= stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][1] << 2;
    stepsData[thestep].noteStepsExtras[curTrack-DRUM_TRACKS][1] |= stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][1] << 4;
    stepsData[thestep].noteStepsExtras[curTrack-DRUM_TRACKS][1] |= stepsData[editStep].noteStepsExtras[curTrack-DRUM_TRACKS][1] << 6;
  }    
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void clearStepsExtrasBits(byte thestep, byte xVar)
{
  bitClear(stepsData[thestep].noteStepsExtras[curTrack-DRUM_TRACKS][0], (editVariation * 2));
  bitClear(stepsData[thestep].noteStepsExtras[curTrack-DRUM_TRACKS][0], (editVariation * 2) + 1);
  bitClear(stepsData[thestep].noteStepsExtras[curTrack-DRUM_TRACKS][1], (editVariation * 2));
  bitClear(stepsData[thestep].noteStepsExtras[curTrack-DRUM_TRACKS][1], (editVariation * 2) + 1);
}  
