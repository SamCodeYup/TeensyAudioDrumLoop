///This code is part 2 of final project synth
///the code is intended to play a kick drum sound after a button has been pressed

//libraries
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>
#include <synth_simple_drum.h>
#include <Encoder.h>


// GUItool: begin automatically generated code
AudioSynthSimpleDrum     drum1;          //xy=681.7142813546317,303.14285278320307
AudioSynthSimpleDrum     drum2;          //xy=707.4285670689173,340.28570992606024
AudioSynthSimpleDrum     drum3;          //xy=707.4285670689173,421.7142813546317
AudioSynthSimpleDrum     drum4;          //xy=717.4285670689173,495.99999564034596
AudioEffectEnvelope      envelope1;      //xy=830.2857099260602,304.5714242117745
AudioMixer4              mixer1;         //xy=943.142852783203,390.28570992606024
AudioOutputI2S           i2s1;           //xy=1143.1428527832031,400.28570992606024
AudioConnection          patchCord1(drum1, envelope1);
AudioConnection          patchCord2(drum2, 0, mixer1, 1);
AudioConnection          patchCord3(drum3, 0, mixer1, 2);
AudioConnection          patchCord4(drum4, 0, mixer1, 3);
AudioConnection          patchCord5(envelope1, 0, mixer1, 0);
AudioConnection          patchCord6(mixer1, 0, i2s1, 0);
AudioConnection          patchCord7(mixer1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=1141.7142813546316,517.4285670689173
// GUItool: end automatically generated code

const int pinButtonK = A0; //kick drum button
const int pinButtonS = A1; //snare button
const int pinButtonHat = A2; //hi hat button
const int pin_A_k = 3; //clock
const int pin_B_k = 2; //data

//declare bounced objects
Bounce Kick_button = Bounce(pinButtonK, 15);
Bounce Snare_button = Bounce(pinButtonS, 15);
Bounce Hat_button = Bounce(pinButtonHat, 15);

//declare encoder object
Encoder kick_Encoder(pin_A_k, pin_B_k); //clock, data
// Encoder snare_Encoder(pin_A_s, pin_B_s);
// Encoder hat_Encoder(pin_A_h, pin_B_h);

//declare unsigned long int used for time
uint32_t tempo;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  tempo = millis() + 1000; //add 1 second to the current runtime

  pinMode(pinButtonK, INPUT_PULLUP); //defining all the variables used
  pinMode(pinButtonS, INPUT_PULLUP);
  pinMode(pinButtonHat, INPUT_PULLUP);
  pinMode(pin_A_k, INPUT_PULLUP);
  pinMode(pin_B_k, INPUT_PULLUP);

  AudioMemory(15); //required for teensy
  
  AudioNoInterrupts(); //required if you're using more than one signal, aka using a mixer.

  //envelope control
  envelope1.attack(2);
  envelope1.release(150);
  envelope1.delay(0);
  envelope1.hold(0);

  //kick drum control
  drum1.frequency(60);
  drum1.length(1500);
  drum1.secondMix(0.0);
  drum1.pitchMod(0.55);

  //snare control
  drum2.frequency(60);
  drum2.length(300);
  drum2.secondMix(0.0);
  drum2.pitchMod(1.0);
  
  //hat? control
  drum3.frequency(450);
  drum3.length(250);
  drum3.secondMix(0.8);
  drum3.pitchMod(0.45);

  //drum 4 control
  drum4.frequency(140);
  drum4.length(90);
  drum4.secondMix(0.0);
  drum4.pitchMod(0.25);
  
  // enable audio output 
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.75);

  AudioInterrupts(); 
}

void loop() {
  // put your main code here, to run repeatedly:
  static int instrument_count[3]; //3-element array to count button presses
  static int num = 0;
  long tempo_vary = kick_Encoder.read() / 2; //change the tempo based on the position of the encoder
  
  //update all the buttons to see if they have been pressed
  Kick_button.update(); 
  Snare_button.update();
  Hat_button.update();

  //increment counters if button is pressed
  if (Kick_button.fallingEdge()){
    instrument_count[0]++;
  }
  if (Snare_button.fallingEdge()){
    instrument_count[1]++;
  }
  if (Hat_button.fallingEdge()){
    instrument_count[2]++;
  }

  //on first run this will become true 1 second after setup
  //all other times will become true every 0.1 second
  if(millis() == tempo)
  {

    switch (num % 16) //to control the subdivisions of the beat - 1e&a 2e&a 3e&a4 4e&a 
    {
      case 0: //beat 1
      case 7: //beat 2 a
      case 10: //beat 3 e
        if (instrument_count[0] % 2){
          drum1.noteOn();
          envelope1.noteOn();
          break;
          }
      case 2: // beat 1 &
      case 6: 
      case 8:
      case 14:
        if (instrument_count[1] % 2){
          drum2.noteOn();
          break;
          }
      case 4:
      case 12:
        if (instrument_count[2] % 2){
          drum4.noteOn();
          break;
          }
      }
      num++; //increment the counter after playing sound
      tempo = millis() + 100 + tempo_vary; //add time to tempo to play again at correct time
  }
}
