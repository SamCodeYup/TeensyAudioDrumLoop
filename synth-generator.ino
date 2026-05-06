///This code is part 2 of final project synth
///the code is intended to play a kick drum sound after a button has been pressed

//libraries
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Encoder.h>

#include <synth_simple_drum.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>


//audio connection scheme
AudioSynthSimpleDrum     drum2;          //xy=399,244
AudioSynthSimpleDrum     drum3;          //xy=424,310
AudioSynthSimpleDrum     drum1;          //xy=431,197
AudioSynthSimpleDrum     drum4;          //xy=464,374
AudioMixer4              mixer1;         //xy=737,265
AudioOutputI2S           i2s1;           //xy=979,214
// acid bass
AudioSynthWaveform       osc; // oscillator wave form
AudioSynthWaveform       osc2; // oscillator wave form
AudioSynthWaveform       osc3;
AudioFilterLadder        ladder; // Ladder filter for resonance
AudioEffectEnvelope      ampEnv;

AudioConnection          patchCord7(osc, ampEnv);
AudioConnection          patchCord8(ampEnv, 0, ladder, 0);
AudioConnection          patchCord9(ladder, 0, i2s1, 0);
AudioConnection          patchCord10(ladder, 0, i2s1, 1);
AudioConnection          patchCord11(osc2, 0, mixer1, 1);
AudioConnection          patchCord12(osc3, 0, mixer1, 1);

AudioConnection          patchCord1(drum2, 0, mixer1, 1);
AudioConnection          patchCord2(drum3, 0, mixer1, 2);
AudioConnection          patchCord3(drum1, 0, mixer1, 0);
AudioConnection          patchCord4(drum4, 0, mixer1, 3);
AudioConnection          patchCord5(mixer1, 0, i2s1, 0);
AudioConnection          patchCord6(mixer1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=930,518

const int pinButtonK = A0;
const int pinButtonS = A1;
const int pinButtonHat = A2;
const int pot1Pin = A8;
const int pot2Pin = A3;

// encoder and button pins
const int encoder_clk = 2;
const int encoder_dt = 3;

const int encoder_sw = 14;
const int encoder_sw2 = 15;
const int encoder_sw3 = 16; 

const int onButtonPin = 5;
int button_state = 0;

// int tempo_count = 1000;

// Encoder myEnc(2,3); // CLK and DT pins

// long oldPosition  = -999;
// int lastButtonState = HIGH;


Bounce Kick_button = Bounce(encoder_sw, 15);
Bounce Snare_button = Bounce(encoder_sw2, 15);
Bounce Hat_button = Bounce(encoder_sw3, 15);
Bounce on_Button = Bounce(onButtonPin, 15);

// uint32_t HardCount;

// uint32_t softCount;

// pentatonic scale for synth
float pentScale[] = {0.0, 220.00, 261.63, 293.66, 329.63, 392.00, 440.00};
float pentScale2[] = {0.0, 110.00, 130.81, 146.83, 164.81, 195.99, 220.00};
float pentScale3[] = {0.0, 55.0, 65.41, 73.42, 82.4, 98.0, 110.0};

int numNotes = 7; // How many notes are in the array
// setting BPM and step duration
int bpm = 130;
int stepDuration4 = (60000 / bpm); // 4th notes
int stepDuration8 = (60000 / bpm) / 2; // 8th notes
int stepDuration16 = (60000 / bpm) / 4; // 16th notes

elapsedMillis stepTimer;
elapsedMillis envTimer;
elapsedMillis potTimer;

float oldLadderRes = -1.0;
float newLadderRes;

// Filter envelope variables
float currentCutoff = 150.0;
const float baseCutoff = 100.0; // Where the filter rests
const float peakCutoff = 4500.0; // The peak of the "squelch"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // HardCount = millis() + 1000;
  // int totalCount;

  pinMode(onButtonPin, INPUT_PULLUP); // turns wave form on
  pinMode(encoder_sw, INPUT_PULLUP); // rotary encoder switch
  pinMode(encoder_sw2, INPUT_PULLUP);
  pinMode(encoder_sw3, INPUT_PULLUP);


  pinMode(pot1Pin, INPUT_PULLUP); // pot 1 analog pin
  pinMode(pot2Pin, INPUT_PULLUP); // pot 2 analog pin

  AudioMemory(15);
  
  AudioNoInterrupts();

  drum1.frequency(60);
  drum1.length(1500);
  drum1.secondMix(0.0);
  drum1.pitchMod(0.55);
  
  drum2.frequency(60);
  drum2.length(300);
  drum2.secondMix(0.0);
  drum2.pitchMod(1.0);
  
  drum3.frequency(450);
  drum3.length(250);
  drum3.secondMix(0.8);
  drum3.pitchMod(0.45);

  drum4.frequency(1200);
  drum4.length(150);
  drum4.secondMix(0.0);
  drum4.pitchMod(0.0);
  
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);

  osc.begin(WAVEFORM_SAWTOOTH); // waveform generation
  osc.amplitude(0.3);

  osc2.begin(WAVEFORM_SAWTOOTH); // waveform generation
  osc2.amplitude(0.3);

  osc3.begin(WAVEFORM_SAWTOOTH); // waveform generation
  osc3.amplitude(0.3);
  
  // envelope filter
  ampEnv.attack(10);
  ampEnv.hold(50);
  ampEnv.decay(100);
  ampEnv.sustain(150);
  ampEnv.release(100);

  // Set up the Filter with high resonance for the "acid" sound
  ladder.resonance(0.0);

  AudioInterrupts();
}

void loop() {
 
  // reading encoder position

  // long newPosition = myEnc.read();

  // Serial.print(oldPosition);
  // delay(100);

  // if (newPosition != oldPosition) {
  //     tempo_count = 50 * newPosition + 1000;
  //     if (tempo_count <= 0) {
  //     tempo_count = 50; // Set a minimum limit of 50ms
  //   }
  //   if (tempo_count >= 2000) {
  //     tempo_count = 2000; // Set a max limit of 2000ms
  //   }
  //   Serial.print("Tempo Count (ms) is: ");
  //   Serial.println(tempo_count);

  //   oldPosition = newPosition;
  //   }
  
  // if (millis() >= HardCount) //increment the hard count by 1 second when it reaches t
  // {
  //   HardCount = millis() + tempo_count;
  // }


  float pot1val = analogRead(pot1Pin);
  float ladderRes = pot1val / 1023.0;

  if (Hat_button.fallingEdge())
  {
      button_state++;
  }

  if (button_state % 2) {
    ampEnv.noteOn();
  }

  if (stepTimer >= stepDuration4) {
    stepTimer = 0;

    drum1.noteOn();
  }


  if (stepTimer >= stepDuration16) {
    stepTimer = 0;

    int randomIndex = random(numNotes);
    // Convert MIDI note to frequency

    float freq = pentScale[randomIndex];
    float freq2 = pentScale2[randomIndex];
    float freq3 = pentScale3[randomIndex];

    osc.frequency(freq);
    osc2.frequency(freq2);
    osc3.frequency(freq3);
    // Trigger the volume envelope
    ampEnv.noteOn();
        // ampEnv.noteOn();


    // Reset the filter cutoff to its peak to start the "squelch"
    currentCutoff = peakCutoff;

  }

  //filter cutoff every 2 milliseconds
  if (envTimer >= 3) {
    envTimer = 0;
    
    // Multiply by a fraction less than 1 for an exponential decay curve.
    // Change this number (e.g., 0.90 to 0.98) to change how long the squelch lasts.
    currentCutoff = currentCutoff * 0.9; 
    
    // Prevent the filter from closing too far
    if (currentCutoff < baseCutoff) {
      currentCutoff = baseCutoff;
    }

    // Apply the new frequency to the audio hardware
    ladder.frequency(currentCutoff);
  }
  
  if (potTimer >= 20) {
    newLadderRes = ladderRes;
  }

  if (newLadderRes >= 1) {
    newLadderRes = 0.95;
  }

  if (abs(newLadderRes - oldLadderRes) > 0.02) {
    ladder.resonance(newLadderRes);
    oldLadderRes = newLadderRes;
  }
  
}

// int transNote1(int noteIndex) {

//   int pot2val = analogRead(pot2Pin);
//   int semitoneShift = map(pot2val, 0, 1023, 0, 11);
//   float pitchMult = pow(2.0, semitoneShift / 12.0);
//   float baseFreq = pentScale[noteIndex];
//   float finalFreq1 = baseFreq * pitchMult;

//   return baseFreq * pitchMult;

// }

// int transNote2(int noteIndex) {

//   int pot2val = analogRead(pot2Pin);
//   int semitoneShift = map(pot2val, 0, 1023, 0, 11);
//   float pitchMult = pow(2.0, semitoneShift / 12.0);
//   float baseFreq = pentScale2[noteIndex];
//   return baseFreq * pitchMult;

// }

// int transNote3(int noteIndex) {

//   int pot2val = analogRead(pot2Pin);
//   int semitoneShift = map(pot2val, 0, 1023, 0, 11);
//   float pitchMult = pow(2.0, semitoneShift / 12.0);
//   float baseFreq = pentScale3[noteIndex];
//   float finalFreq3 = baseFreq * pitchMult;

//   return baseFreq * pitchMult;

// }
