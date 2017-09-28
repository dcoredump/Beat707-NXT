/*
 * 
 * Created by William Kalfelz @ Beat707 (c) 2017 - http://www.Beat707.com
 * 
 */

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void createSongScreen()
{
  leds[0] = leds[1] = leds[2] = 0;
  if (seqPlaying) bitSet(leds[0], (seqPosition/4)); else bitSet(leds[0], 0);
  //
  bitSet(leds[0], variation + 4);
  //
  leds[1] = leds[2] = 0;
  if (seqPlaying) { if (seqPosition < 8) bitSet(leds[1], seqPosition); else bitSet(leds[2], seqPosition - 8); }
  //
  segments[0][0] = S_b;
  printNumber(0, 1, configData.BPM);
  if (showBPMdot) segments[0][3] |= B10000000;
  printNumber(0, 5, songPosition + 1);
  segments[0][4] = 0;
  segments[0][5] = S_P;
  //
  memset(segments[1], 0, sizeof(segments[1]));
  memset(segments[2], 0, sizeof(segments[2]));  
  //
  segments[1][2] = S_P;
  segments[1][3] = S_T;
  printNumber(1, 5, songData.pattern[0][songPosition] + 1);
  //
  segments[2][0] = S_A;
  byte xVarABCD = songData.pattern[1][songPosition] >> 4;
  if (xVarABCD >= 2) segments[2][1] = S_b;
  if (xVarABCD >= 3) segments[2][2] = S_C;
  if (xVarABCD >= 4) segments[2][3] = S_d;
  //
  byte xRepeat = songData.pattern[1][songPosition] & B00001111;
  segments[2][5] = S_R;
  segments[2][6] = S_P;
  segments[2][7] = (char)pgm_read_word(&numbers[xRepeat]);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void songInterface()
{

}

