#include "mp3tf16p.h"

#define Rotary_Clock 2
#define Rotary_Data 3
#define Rotary_PushButton 4
#define PushButton_Debounce 250
#define Rotary_Debounce 250

int rotary_lastStateClock; // Store the PREVIOUS status of the clock pin (HIGH or LOW)
unsigned long rotary_lastTimeButtonPress = 0;
unsigned long rotary_lastTurn = 0;

#define volumeLevel 20
int trackCount;
int currentTrack = 0;

// MP3 Player
MP3Player mp3(10, 11);

void setup(void)
{
  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function
  randomSeed(analogRead(0));

  initRotaryEncoder();

  Serial.begin(9600);
  mp3.initialize();

  trackCount = mp3.player.readFileCounts();
  Serial.print("Number of tracks=");
  Serial.println(trackCount);
}

void loop(void)
{
  if (readRotaryPushButton())
  {
    currentTrack = random(1,trackCount+1);
    Serial.print("Random...Playing Track #");
    Serial.println(currentTrack);
    mp3.playTrackNumber(currentTrack, volumeLevel, false);
  }
  if (readRotaryEncoder())
  {
    Serial.print("Playing Track #");
    Serial.println(currentTrack);
    mp3.playTrackNumber(currentTrack, volumeLevel, false);
  }
  mp3.serialPrintStatus(MP3_ERROR_ONLY);
}

void initRotaryEncoder(void)
{
  pinMode(Rotary_Clock, INPUT_PULLUP);
  pinMode(Rotary_Data, INPUT_PULLUP);
  pinMode(Rotary_PushButton, INPUT_PULLUP);
  rotary_lastStateClock = digitalRead(Rotary_Clock);
}

boolean readRotaryPushButton(void)
{
  int buttonValue = digitalRead(Rotary_PushButton);
  if (buttonValue == LOW)
  {
    if (millis() - rotary_lastTimeButtonPress > PushButton_Debounce)
    {
      Serial.println("Button pressed!");
      // Remember last button press event
      rotary_lastTimeButtonPress = millis();
      return true;
    }
  }
  return false;
}

boolean readRotaryEncoder(void)
{
  boolean rotaryRead = false;
  // Read the current state of CLK
  int clockValue = digitalRead(Rotary_Clock);

  // If last and current state of Rotary_Clock are different, then "pulse occurred"
  // React to only 1 state change to avoid double count
  if (clockValue != rotary_lastStateClock && clockValue == 1 && (millis() - rotary_lastTurn > Rotary_Debounce))
  {

    // If the Rotary_Data state is different than the Rotary_Clock state then
    // the encoder is rotating "CCW" so we decrement
    if (digitalRead(Rotary_Data) != clockValue)
    {
      Serial.println("Clockwise");
      if (currentTrack == trackCount)
      {
        currentTrack = 1;
      }
      else
      {
        currentTrack++;
      }
    }
    else
    {
      if (currentTrack == 1 || currentTrack == 0)
      {
        currentTrack = trackCount;
      }
      else
      {
        currentTrack--;
      }
      Serial.println("Counterclockwise");
    }
    rotaryRead = true;
    rotary_lastTurn = millis();
  }
  rotary_lastStateClock = clockValue;
  return rotaryRead;
}