/*
 * 
 * Created by William Kalfelz @ Beat707 (c) 2017 - http://www.Beat707.com
 * 
 */

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ISR(TIMER1_COMPA_vect) 
{
  doTickSequencer();
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void doTickSequencer()
{
  if (seqPlaying)
  {
    if (configData.seqSyncOut)
    {
      if (ppq24Counter == 0) Serial.write(0xF8); // Midi Clock Sync Tick 24PPQ
      ppq24Counter++;
      if (ppq24Counter >= 4) ppq24Counter = 0;
    }
    // Check For Echo //
    for (byte xe = 0; xe < ECHOS; xe++)
    {
      if (echoCounter[xe][0] > 0)
      {
        echoCounter[xe][1]--;
        if (echoCounter[xe][1] == 0) 
        {
          echoCounter[xe][0]--;
          if (echoCounter[xe][0] > 0) 
          {
            echoCounter[xe][1] = echoSpace[xe];
            //
            trackNoteOn(echoTrack[xe], configData.trackNote[echoTrack[xe]-1], getMPVelocity(patternData.trackProcessor[echoTrack[xe]-1], echoVelocity[xe]));
            //
            int newV = echoVelocity[xe] + echoAttackDecay[xe];
            if (newV < 0) newV = 0;
            if (newV > 127) newV = 127;
            echoVelocity[xe] = newV;
          }
        }
      }
    }
    // Check Solo and Mute Tracks //
    if (hasSoloTrack)
    {
      if (prevSoloTrack != configData.soloTrack)
      {
        for (byte x=0; x<(NOTE_TRACKS+DRUM_TRACKS+1); x++)
        {
          if (bitRead(prevSoloTrack, x) != bitRead(configData.soloTrack, x) && bitRead(configData.soloTrack, x) == 0)
          {
            if (x >= NOTE_TRACKS) stopDrumTrackPrevNote(x - NOTE_TRACKS, true); else stopDrumTrackPrevNote(x, false);
          }
        }
        prevSoloTrack = configData.soloTrack;
      }
    }
    else
    {
      if (prevMuteTrack != configData.muteTrack)
      {
        for (byte x=0; x<(NOTE_TRACKS+DRUM_TRACKS+1); x++)
        {
          if (bitRead(prevMuteTrack, x) != bitRead(configData.muteTrack, x) && bitRead(configData.muteTrack, x) == 1)
          {
            if (x >= NOTE_TRACKS) stopDrumTrackPrevNote(x - NOTE_TRACKS, true); else stopDrumTrackPrevNote(x, false);
          }
        }
        prevMuteTrack = configData.muteTrack;
      }
    }
    //
    if (seqCounter == 0 || seqCounter == 12)
    {      
      midiClockBeats++;
      if (midiClockBeats >= 16) midiClockBeats = 0;
      //
      byte isDouble = 1;
      if (seqCounter == 0) isDouble = 0;
      //    
      byte accent = bitRead(stepsData[seqPosition].steps[15], 1 + (variation * 2)) << 1;
      accent |= bitRead(stepsData[seqPosition].steps[15], 0 + (variation * 2));
      //
      for (byte x=0; x<(DRUM_TRACKS-1); x++)
      {
        if ((hasSoloTrack && bitRead(configData.soloTrack, NOTE_TRACKS + x) == 1) ||
        (!hasSoloTrack && bitRead(configData.muteTrack, NOTE_TRACKS + x) != 1))
        {
          byte xc = bitRead(stepsData[seqPosition].steps[x], 1 + (variation * 2)) << 1;
          xc |= bitRead(stepsData[seqPosition].steps[x], 0 + (variation * 2));
          bool isDouble = bitRead(stepsData[seqPosition].stepsDouble[variation], x);
          //
          if ((xc > 0 && seqCounter == 0) || (seqCounter == 12 && isDouble))
          {
            byte orgXc = xc;
            if (xc == 1) xc = accent; // use accent track 
            //else if (xc == 3) xc = 3;
            //else if (xc == 2) xc = 2;
            //
            if (xc == 0 && isDouble) xc = prevVel;
            if (xc <= 0 || xc > 3) xc = 1;
            prevVel = xc;
            byte theVelocity = configData.accentValues[xc-1];
            //
            // Process Echo //
            for (byte xe = 0; xe < ECHOS; xe++)
            {
              if (((patternData.echoConfig[xe].track - 1) == x) && 
              ( (patternData.echoConfig[xe].type == echoTypeOnAllNotes) || ((patternData.echoConfig[xe].type == echoTypeForceMaxVelocity && orgXc == 3)) || ((patternData.echoConfig[xe].type == echoTypeForceLowVelocity && orgXc == 2)) ))
              {
                echoCounter[xe][0] = patternData.echoConfig[xe].ticks;
                echoCounter[xe][1] = patternData.echoConfig[xe].space;
                echoAttackDecay[xe] = patternData.echoConfig[xe].attackDecay;
                echoSpace[xe] = patternData.echoConfig[xe].space;
                echoTrack[xe] = patternData.echoConfig[xe].track;
                if (patternData.echoConfig[xe].attackDecay > 0) echoVelocity[xe] = patternData.echoConfig[xe].attackDecay; else echoVelocity[xe] = configData.accentValues[xc-1];
                theVelocity = echoVelocity[xe];
              }
            }
            //
            trackNoteOn(x, configData.trackNote[x], getMPVelocity(patternData.trackProcessor[x], theVelocity));
          }
        }
      }
      //
      for (byte x=0; x<(NOTE_TRACKS-1); x++)
      {
        if ((hasSoloTrack && bitRead(configData.soloTrack, x) == 1) ||
        (!hasSoloTrack && bitRead(configData.muteTrack, x) != 1))
        {
          byte xvel = bitRead(stepsData[seqPosition].noteStepsExtras[x][0], 1 + (variation * 2)) << 1;
          xvel |= bitRead(stepsData[seqPosition].noteStepsExtras[x][0], 0 + (variation * 2));
          byte xc = bitRead(stepsData[seqPosition].noteStepsExtras[x][1], 1 + (variation * 2)) << 1;
          xc |= bitRead(stepsData[seqPosition].noteStepsExtras[x][1], 0 + (variation * 2));
          //
          bool isSlide = (xc == 1);
          bool isDouble = (xc == 2);
          byte xnote = stepsData[seqPosition].noteSteps[x][variation];
          //
          if ((xnote > 0 && seqCounter == 0) || (xnote > 0 && seqCounter == 12 && isDouble))
          {
            noteTrackNoteOn(DRUM_TRACKS+x, xnote, getMPVelocity(patternData.trackProcessor[DRUM_TRACKS+x], configData.accentValues[xvel-1]), isSlide);
          }
        }
      }
      //
      // ---===-------===-------===-------===-------===-------===-------===-------===-------===----
      //
      //if (currentMode == patternMode)
      if (streamNextPattern && seqCounter == 12 && (seqPosition+1) >= STEPS && (variation+1) >= patternData.totalVariations)
      {
        streamNextPattern = false;
        loadPatternNow = true;
      }
    }
  }
  //
  seqCounter++;
  if (seqCounter >= 24)
  {
    seqCounter = 0;
    seqPosition++;
    updateScreen = true;
    showBPMdot = false;
    //
    switch (seqPosition)
    {
      case 0: showBPMdot = true; break;
      case 4: showBPMdot = true; break;
      case 8: showBPMdot = true; break;
      case 12: showBPMdot = true; break;
    }
    //
    if (seqPosition >= STEPS) 
    {
      seqPosition = 0; 
      showBPMdot = updateScreen = true;
      if (seqPlaying)
      {
        variation++;
        if (variation >= patternData.totalVariations)
        {
          variation = 0;
          if (loadPatternNow) showErrorMsg(99);
        }
      }    
    }
  }
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
byte getMPVelocity(byte xproc, byte xorgvelocity)
{
  switch(xproc)
  {    
    case fadeIn:    
      return max(procFadeMin,min(127,xorgvelocity-(128-((seqPosition + (variation * 16)) * 2))));
      break;
      
    case fadeOut:   
      return max(procFadeMin,min(127,xorgvelocity-(((seqPosition + (variation * 16)) * 2))));
      break;
      
    case fadeInD:
      if (variation == 3) return max(procFadeMin,min(127,xorgvelocity-(128-(seqPosition * 9))));
      break;

    case fadeOutD:
      if (variation == 3) return max(procFadeMin,min(127,xorgvelocity-((seqPosition * 9))));
      break;

    case fadeInOut:
      if (variation <= 1) return max(procFadeMin,min(127,xorgvelocity-(128-((seqPosition + (variation * 16)) * 4))));
        else return max(procFadeMin,min(127,xorgvelocity-(((seqPosition + ((variation-2) * 16)) * 4))));
      break;

    case fadeOutIn:
      if (variation <= 1) return max(procFadeMin,min(127,xorgvelocity-(((seqPosition + (variation * 16)) * 4))));
        else return max(procFadeMin,min(127,xorgvelocity-(128-((seqPosition + ((variation-2) * 16)) * 4))));
      break;

    case randomVel:
      return max(procFadeMin,min(127,xorgvelocity-byte(random(procFadeMin, 127))));
      break;
  }
  //
  return xorgvelocity;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void trackNoteOn(byte xtrack, byte xnote, byte xvelocity)
{
  sendMidiEvent(midiNoteOff, xnote, 0, configData.trackMidiCH[xtrack]);
  sendMidiEvent(midiNoteOn, xnote, xvelocity, configData.trackMidiCH[xtrack]);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void noteTrackNoteOn(byte xtrack, byte xnote, byte xvelocity, bool slide)
{
  if ((!slide && prevPlayedNote[xtrack-DRUM_TRACKS] > 0) || prevPlayedNote[xtrack-DRUM_TRACKS] == xnote)  sendMidiEvent(midiNoteOff, prevPlayedNote[xtrack-DRUM_TRACKS], 0, configData.trackMidiCH[xtrack]);
  sendMidiEvent(midiNoteOn, xnote, xvelocity, configData.trackMidiCH[xtrack]);
  if (slide && prevPlayedNote[xtrack-DRUM_TRACKS] > 0 && prevPlayedNote[xtrack-DRUM_TRACKS] != xnote)  sendMidiEvent(midiNoteOff, prevPlayedNote[xtrack-DRUM_TRACKS], 0, configData.trackMidiCH[xtrack]);
  prevPlayedNote[xtrack-DRUM_TRACKS] = xnote;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void resetSequencer()
{
  for (byte xe = 0; xe < ECHOS; xe++)
  {
    echoCounter[xe][0] = echoCounter[xe][1] = 0;
  }
  seqPosition = seqCounter = ppq24Counter = variation = midiClockBeats = 0;
  showBPMdot = true;
  //
  for (byte x=0; x<DRUM_TRACKS; x++)
  {
    stopDrumTrackPrevNote(x, true);
  }
  //
  for (byte x=0; x<NOTE_TRACKS; x++)
  {
    stopDrumTrackPrevNote(x, false);
  }
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void stopDrumTrackPrevNote(byte track, bool isDrumTrack)
{
  if (isDrumTrack)
  {
    sendMidiEvent(midiNoteOff, configData.trackNote[track], 0, configData.trackMidiCH[track]);
  }
  else
  {
    if (prevPlayedNote[track] > 0)
    {
      sendMidiEvent(midiNoteOff, prevPlayedNote[track], 0, configData.trackMidiCH[DRUM_TRACKS+track]);
      prevPlayedNote[track] = 0;
    } 
  }
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void startSequencer()
{
  stopTimer();
  seqPlaying = true;
  resetSequencer();  
  doTickSequencer();
  if (configData.seqSyncOut) Serial.write(0xFA); // MIDI Start
  startTimer();
  updateScreen = true;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void startTimer()
{
  TCCR1A = TCCR1B = 0;
  bitWrite(TCCR1B, CS11, 1);
  bitWrite(TCCR1B, WGM12, 1);
  updateSequencerSpeed();
  bitWrite(TIMSK1, OCIE1A, 1); 
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void stopTimer()
{
  bitWrite(TIMSK1, OCIE1A, 0);
  TCCR1A = TCCR1B = OCR1A = 0; 
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void updateSequencerSpeed()
{
  // Calculates the Frequency for the Timer, used by the PPQ clock (Pulses Per Quarter Note) 96 in this case //
  // This uses the 16-bit Timer1, unused by the Arduino, unless you use the analogWrite or Tone functions //
  //
  OCR1A = (F_CPU / 8) / ((((configData.BPM)*(96))/60)) - 1;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void stopSequencer(void)
{
  // Stop Timer //
  stopTimer();
  //
  if (configData.seqSyncOut) Serial.write(0xFC); // MIDI Stop
  //
  if (!seqPlaying) songPosition = 0;
  seqPlaying = false;
  resetSequencer();
  MIDIallNotesOff();
  //
  checkIfDataNeedsSaving();  
  if (streamNextPattern)
  {
    streamNextPattern = false;
    loadPatternNow = true;
  }
  //
  startTimer();
  updateScreen = true;  
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MIDIallNotesOff()
{
  for (byte x=0; x<midiChannels; x++)
  {
    Serial.write(0xB0+x);
    Serial.write(0x7B);
    Serial.write((byte)0x00);
  }
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void sendMidiEvent(byte type, byte byte1, byte byte2, byte channel)
{
  Serial.write(type+channel);
  Serial.write(byte1);
  Serial.write(byte2);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void startMIDIinterface()
{
  #if DEBUG_SERIAL
    Serial.begin(9600);
    Serial.println("Startup");
  #else
    #if MIDI_OVER_USB 
      Serial.begin(38400); 
    #else 
      Serial.begin(31250); // 31250 MIDI Interface //
    #endif
    Serial.flush();
    resetSequencer();
    MIDIallNotesOff();
  #endif
}
