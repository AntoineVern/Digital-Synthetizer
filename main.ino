

#include <Audio.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce2.h>
#include <SPI.h>
#include <Wire.h>      // this is needed even tho we aren't using it
#include "ILI9341_t3.h"
#include <XPT2046_Touchscreen.h>

// Variables Chorus
#define CHORUS_DELAY_LENGTH (16*AUDIO_BLOCK_SAMPLES) // Number of samples
short l_delaylineChorus[CHORUS_DELAY_LENGTH]; // Allocate the delay lines
short r_delaylineChorus[CHORUS_DELAY_LENGTH]; //..
int n_chorus = 10; // Number of "voices"

// Variables Flange
#define FLANGE_DELAY_LENGTH (6*AUDIO_BLOCK_SAMPLES)
short l_delaylineFlange[FLANGE_DELAY_LENGTH];
short r_delaylineFlange[FLANGE_DELAY_LENGTH];
int s_idx = FLANGE_DELAY_LENGTH / 4;
int s_depth = FLANGE_DELAY_LENGTH / 4;
double s_freq = 1.0;

// Variables BMS
int UnitePourcentage = 0;
int DizainePourcentage = 0;
int pourcentage = 0;
int ABC = 0;
int Printbatterie = 1;

// Variables Bouton layers
int VA1 = 0;
int VA2 = 1;
int LayerSwitch = 1;
static unsigned long previousMillis2 = 0;
const unsigned long interval = 1500;

// Variables Nom fichiers .wav
const char*    Fichier1 = "Bouton1Layer1.wav";
const char*    Fichier2 = "Bouton2Layer1.wav";
const char*    Fichier3 = "Bouton3Layer1.wav";
const char*   Fichier4 = "Bouton4Layer1.wav";
const char*   Fichier5 = "Bouton5Layer1.wav";
const char*   Fichier6 = "Bouton6Layer1.wav";
const char*   Fichier7 = "Bouton7Layer1.wav";
const char*   Fichier8 = "Bouton8Layer1.wav";
const char*  Fichier9 = "Bouton9Layer1.wav";
const char*  Fichier10 = "Bouton10Layer1.wav";
const char*  Fichier11 = "Bouton11Layer1.wav";
const char*  Fichier12 = "Bouton12Layer1.wav";
const char*   Fichier13 = "Bouton13Layer1.wav";
const char*  Fichier14 = "Bouton14Layer1.wav";
const char*  Fichier15 = "Bouton15Layer1.wav";
const char*  Fichier16 = "Bouton16Layer1.wav";

// Variables I2C Ampli
#define MAX9744_I2CADDR 0x4B
#define VolumeMax 40
float MixerVolumeMax = 0.5;
float m = (VolumeMax - 16.0) / 100.0;


// Variables SDIO SD card
#define SDCARD_CS_PIN BUILTIN_SDCARD
#define SDCARD_MOSI_PIN 43 // not actually used
#define SDCARD_SCK_PIN 45  // not actually used

// Variables Écran pins SPI
#define TFT_DC      9
#define TFT_CS      10
#define TFT_RST     34  // 255 = unused, connect to 3.3V
#define TFT_MOSI    11
#define TFT_SCLK    13
#define TFT_MISO    12

// Variables Touch Screen pins SPI
#define CS_PIN 8
#define Tirq 2

// Variables Calibration datas Touch Screen
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

// Variables Box Sizes
#define BOXSIZE 40
#define PENRADIUS 0.8

// Variables Boutons Pads Constantes
#define PADSIZE 45
bool EtatBouton1 = 0;
bool EtatBouton2 = 0;
bool EtatBouton3 = 0;
bool EtatBouton4 = 0;
bool EtatBouton5 = 0;
bool EtatBouton6 = 0;
bool EtatBouton7 = 0;
bool EtatBouton8 = 0;
bool EtatBouton9 = 0;
bool EtatBouton10 = 0;
bool EtatBouton11 = 0;
bool EtatBouton12 = 0;
bool EtatBouton13 = 0;
bool EtatBouton14 = 0;
bool EtatBouton15 = 0;
bool EtatBouton16 = 0;


// Variables Etat Bouton
int Bouton = 1;

// Variables Etat Menu
int Etat = 1;
int Menu3Selected = 0;
int Menu4Selected = 0;
int Action = 0;
int Action2 = 0;

// Variables Volume
float Slide = 96.0;
float SlideBuffer = 96.0;
float PourcentageVolume = 50.0;

// Variables Envelope
float Attack = 10.5, Holds = 2.5, Decay = 35, Sustains = 0.5;
float PixelAttack = 81, PixelHolds = 81, PixelDecay = 81 , PixelSustains = 107;

// Variables Mixer
float Mixer1 = 140.0, Mixer2 = 140.0, Mixer3 = 140.0, Mixer4 = 140.0, Voix = 102, Damping = 83, Roomsize = 83,
      Offset = 102.0, Depth = 106.0, DelayRate = 128.0, PositionBatterie = 0;
float Mixer1Buffer = 140.0, Mixer2Buffer = 140.0, Mixer3Buffer = 140.0, Mixer4Buffer = 140.0, pourcentageBuffer = 101;
float PourcentageMixer1 = 0.0, PourcentageMixer2 = 0.0, PourcentageMixer3 = 0.0, PourcentageMixer4 = 0.0,
      NombreVoix = 10, NombreRoomsize = 0.5, NombreDamping = 0.5, NombreOffset = 1.0, NombreDepth = 4.0,
      NombreDelayRate = 1.0;

// Variables Volume Level
int8_t thevol = 28;

// Variables Effets
float a = 0.0;
float b = 0.0;
float c = 0.0;

// Variables Peak
uint8_t leftPeakBuffer = 0;
uint8_t rightPeakBuffer = 0;

//Fonctions Print
void ImagePrint(void);
void VolumePrint(void);
void MixerPrint(int);
void PadPrint(char);
void ReprintPads(void);
void AffichageMenu(void);

//Fonctions Test
void TestTactilAppuye(void);
void TestPadAppuye(void);

// Variables Bouton Layer
int k = 0;

// Object Screen
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);

// Object Tactile
XPT2046_Touchscreen ts(CS_PIN, Tirq);

// GUItool: begin automatically generated code
AudioPlaySdWav           playSdWav9;     //xy=123.66665649414062,488.99999237060547
AudioPlaySdWav           playSdWav8;     //xy=124.66665649414062,457.99999237060547
AudioPlaySdWav           playSdWav1;     //xy=127.66665649414062,238.99999237060547
AudioPlaySdWav           playSdWav2;     //xy=127.66665649414062,269.99999237060547
AudioPlaySdWav           playSdWav3;     //xy=127.66665649414062,300.99999237060547
AudioPlaySdWav           playSdWav11;    //xy=126.66665649414062,550.9999923706055
AudioPlaySdWav           playSdWav12;    //xy=126.66665649414062,581.9999923706055
AudioPlaySdWav           playSdWav4;     //xy=127.66665649414062,332.99999237060547
AudioPlaySdWav           playSdWav5;     //xy=127.66665649414062,362.99999237060547
AudioPlaySdWav           playSdWav13;    //xy=126.66665649414062,612.9999923706055
AudioPlaySdWav           playSdWav6;     //xy=127.66665649414062,393.99999237060547
AudioPlaySdWav           playSdWav14;    //xy=126.66665649414062,644.9999923706055
AudioPlaySdWav           playSdWav7;     //xy=127.66665649414062,424.99999237060547
AudioPlaySdWav           playSdWav15;    //xy=126.66665649414062,675.9999923706055
AudioPlaySdWav           playSdWav16;    //xy=126.66665649414062,706.9999923706055
AudioPlaySdWav           playSdWav10;    //xy=127.66665649414062,519.9999923706055
AudioEffectEnvelope      RightEnvelopeBouton14; //xy=380.6666564941406,1153.9999923706055
AudioEffectEnvelope      RightEnvelopeBouton15; //xy=380.6666564941406,1185.9999923706055
AudioEffectEnvelope      RightEnvelopeBouton16; //xy=380.6666564941406,1216.9999923706055
AudioEffectEnvelope      RightEnvelopeBouton12; //xy=381.6666564941406,1090.9999923706055
AudioEffectEnvelope      RightEnvelopeBouton13; //xy=381.6666564941406,1122.9999923706055
AudioEffectEnvelope      RightEnvelopeBouton10; //xy=382.6666564941406,1027.9999923706055
AudioEffectEnvelope      RightEnvelopeBouton11; //xy=382.6666564941406,1059.9999923706055
AudioEffectEnvelope      RightEnvelopeBouton6; //xy=383.6666564941406,901.9999923706055
AudioEffectEnvelope      RightEnvelopeBouton7; //xy=383.6666564941406,933.9999923706055
AudioEffectEnvelope      RightEnvelopeBouton8; //xy=383.6666564941406,964.9999923706055
AudioEffectEnvelope      RightEnvelopeBouton9; //xy=383.6666564941406,996.9999923706055
AudioEffectEnvelope      RightEnvelopeBouton4; //xy=384.6666564941406,838.9999923706055
AudioEffectEnvelope      RightEnvelopeBouton5; //xy=384.6666564941406,870.9999923706055
AudioEffectEnvelope      LeftEnvelopeBouton15; //xy=386.6666564941406,671.9999923706055
AudioEffectEnvelope      LeftEnvelopeBouton16; //xy=386.6666564941406,703.9999923706055
AudioEffectEnvelope      RightEnvelopeBouton2; //xy=386.6666564941406,775.9999923706055
AudioEffectEnvelope      LeftEnvelopeBouton11; //xy=387.6666564941406,545.9999923706055
AudioEffectEnvelope      RightEnvelopeBouton3; //xy=386.6666564941406,807.9999923706055
AudioEffectEnvelope      LeftEnvelopeBouton12; //xy=387.6666564941406,577.9999923706055
AudioEffectEnvelope      LeftEnvelopeBouton13; //xy=387.6666564941406,608.9999923706055
AudioEffectEnvelope      LeftEnvelopeBouton14; //xy=387.6666564941406,640.9999923706055
AudioEffectEnvelope      LeftEnvelopeBouton9; //xy=388.6666564941406,482.99999237060547
AudioEffectEnvelope      RightEnvelopeBouton1; //xy=387.6666564941406,744.9999923706055
AudioEffectEnvelope      LeftEnvelopeBouton10; //xy=388.6666564941406,514.9999923706055
AudioEffectEnvelope      LeftEnvelopeBouton7; //xy=389.6666564941406,419.99999237060547
AudioEffectEnvelope      LeftEnvelopeBouton8; //xy=389.6666564941406,451.99999237060547
AudioEffectEnvelope      LeftEnvelopeBouton3; //xy=390.6666564941406,293.99999237060547
AudioEffectEnvelope      LeftEnvelopeBouton4; //xy=390.6666564941406,325.99999237060547
AudioEffectEnvelope      LeftEnvelopeBouton5; //xy=390.6666564941406,356.99999237060547
AudioEffectEnvelope      LeftEnvelopeBouton6; //xy=390.6666564941406,388.99999237060547
AudioEffectEnvelope      LeftEnvelopeBouton1; //xy=391.6666564941406,230.99999237060547
AudioEffectEnvelope      LeftEnvelopeBouton2; //xy=391.6666564941406,262.99999237060547
AudioEffectEnvelope *envs[32] = {
  &LeftEnvelopeBouton1, &LeftEnvelopeBouton2, &LeftEnvelopeBouton3, &LeftEnvelopeBouton4,
  &LeftEnvelopeBouton5, &LeftEnvelopeBouton6, &LeftEnvelopeBouton7, &LeftEnvelopeBouton8,
  &LeftEnvelopeBouton9, &LeftEnvelopeBouton10, &LeftEnvelopeBouton11, &LeftEnvelopeBouton12,
  &LeftEnvelopeBouton13, &LeftEnvelopeBouton14, &LeftEnvelopeBouton15, &LeftEnvelopeBouton16,
  &RightEnvelopeBouton1, &RightEnvelopeBouton2, &RightEnvelopeBouton3, &RightEnvelopeBouton4,
  &RightEnvelopeBouton5, &RightEnvelopeBouton6, &RightEnvelopeBouton7, &RightEnvelopeBouton8,
  &RightEnvelopeBouton9, &RightEnvelopeBouton10, &RightEnvelopeBouton11, &RightEnvelopeBouton12,
  &RightEnvelopeBouton13, &RightEnvelopeBouton14, &RightEnvelopeBouton15, &RightEnvelopeBouton16
};
AudioMixer4              Lmixer1;        //xy=653.6666564941406,299.99999237060547
AudioMixer4              Rmixer1;        //xy=657.6666564941406,740.9999923706055
AudioMixer4              Lmixer2;        //xy=777.6666564941406,318.99999237060547
AudioMixer4              Rmixer2;        //xy=784.6666564941406,720.9999923706055
AudioMixer4              Lmixer3;        //xy=901.6666564941406,337.99999237060547
AudioMixer4              Rmixer3;        //xy=911.6666564941406,700.9999923706055
AudioMixer4              Lmixer4;        //xy=1023.6666564941406,357.99999237060547
AudioMixer4              Rmixer4;        //xy=1032.6666564941406,680.9999923706055
AudioMixer4              Lmixer5;        //xy=1145.6666564941406,376.99999237060547
AudioMixer4              Rmixer5;        //xy=1157.6666564941406,660.9999923706055
AudioEffectFlange        flange1;        //xy=1286.6666564941406,440.99999237060547
AudioEffectChorus        chorus1;        //xy=1288.6667861938477,409.00000381469727
AudioEffectFreeverbStereo freeverbs1;     //xy=1297.6666564941406,376.99999237060547
AudioEffectChorus        chorus2;        //xy=1303.6667861938477,689.9999828338623
AudioEffectFlange        flange2;        //xy=1303.6666564941406,723.9999923706055
AudioEffectFreeverbStereo freeverbs2;     //xy=1311.6666564941406,660.9999923706055
AudioMixer4              ReverbStereo2;  //xy=1472.6666564941406,851.9999923706055
AudioMixer4              ReverbStereo1;  //xy=1477.6666564941406,262.99999237060547
AudioMixer4              RmixerReverb;   //xy=1687.6666564941406,650.9999923706055
AudioMixer4              RmixerChorus;   //xy=1687.6666564941406,711.9999923706055
AudioMixer4              RmixerFlange;   //xy=1687.6666564941406,771.9999923706055
AudioMixer4              LmixerReverb;   //xy=1689.6666564941406,359.99999237060547
AudioMixer4              LmixerChorus;   //xy=1689.6666564941406,420.99999237060547
AudioMixer4              LmixerFlange;   //xy=1689.6666564941406,480.99999237060547
AudioMixer4              Leffectmixer1;  //xy=1916.6666564941406,475.99999237060547
AudioMixer4              Reffectmixer1;  //xy=1916.6666564941406,538.9999923706055
AudioAnalyzePeak         peak_R;          //xy=2088.515869140625,623.515625
AudioAnalyzeRMS          rms_R;           //xy=2090.5161743164062,590.515643119812
AudioAnalyzePeak         peak_L;          //xy=2092.7502517700195,417.75000381469727
AudioAnalyzeRMS          rms_L;           //xy=2093.7503051757812,384.7500104904175
AudioOutputI2S           i2s1;           //xy=2282.6668090820312,517.0000152587891
AudioConnection          patchCord1(playSdWav9, 0, LeftEnvelopeBouton9, 0);
AudioConnection          patchCord2(playSdWav9, 1, RightEnvelopeBouton9, 0);
AudioConnection          patchCord3(playSdWav8, 0, LeftEnvelopeBouton8, 0);
AudioConnection          patchCord4(playSdWav8, 1, RightEnvelopeBouton8, 0);
AudioConnection          patchCord5(playSdWav1, 0, LeftEnvelopeBouton1, 0);
AudioConnection          patchCord6(playSdWav1, 1, RightEnvelopeBouton1, 0);
AudioConnection          patchCord7(playSdWav2, 0, LeftEnvelopeBouton2, 0);
AudioConnection          patchCord8(playSdWav2, 1, RightEnvelopeBouton2, 0);
AudioConnection          patchCord9(playSdWav3, 0, LeftEnvelopeBouton3, 0);
AudioConnection          patchCord10(playSdWav3, 1, RightEnvelopeBouton3, 0);
AudioConnection          patchCord11(playSdWav11, 0, LeftEnvelopeBouton11, 0);
AudioConnection          patchCord12(playSdWav11, 1, RightEnvelopeBouton11, 0);
AudioConnection          patchCord13(playSdWav12, 0, LeftEnvelopeBouton12, 0);
AudioConnection          patchCord14(playSdWav12, 1, RightEnvelopeBouton12, 0);
AudioConnection          patchCord15(playSdWav4, 0, LeftEnvelopeBouton4, 0);
AudioConnection          patchCord16(playSdWav4, 1, RightEnvelopeBouton4, 0);
AudioConnection          patchCord17(playSdWav5, 0, LeftEnvelopeBouton5, 0);
AudioConnection          patchCord18(playSdWav5, 1, RightEnvelopeBouton5, 0);
AudioConnection          patchCord19(playSdWav13, 0, LeftEnvelopeBouton13, 0);
AudioConnection          patchCord20(playSdWav13, 1, RightEnvelopeBouton13, 0);
AudioConnection          patchCord21(playSdWav6, 0, LeftEnvelopeBouton6, 0);
AudioConnection          patchCord22(playSdWav6, 1, RightEnvelopeBouton6, 0);
AudioConnection          patchCord23(playSdWav14, 0, LeftEnvelopeBouton14, 0);
AudioConnection          patchCord24(playSdWav14, 1, RightEnvelopeBouton14, 0);
AudioConnection          patchCord25(playSdWav7, 0, LeftEnvelopeBouton7, 0);
AudioConnection          patchCord26(playSdWav7, 1, RightEnvelopeBouton7, 0);
AudioConnection          patchCord27(playSdWav15, 0, LeftEnvelopeBouton15, 0);
AudioConnection          patchCord28(playSdWav15, 1, RightEnvelopeBouton15, 0);
AudioConnection          patchCord29(playSdWav16, 0, LeftEnvelopeBouton16, 0);
AudioConnection          patchCord30(playSdWav16, 1, RightEnvelopeBouton16, 0);
AudioConnection          patchCord31(playSdWav10, 0, LeftEnvelopeBouton10, 0);
AudioConnection          patchCord32(playSdWav10, 1, RightEnvelopeBouton10, 0);
AudioConnection          patchCord33(RightEnvelopeBouton14, 0, Rmixer5, 0);
AudioConnection          patchCord34(RightEnvelopeBouton15, 0, Rmixer5, 1);
AudioConnection          patchCord35(RightEnvelopeBouton16, 0, Rmixer5, 2);
AudioConnection          patchCord36(RightEnvelopeBouton12, 0, Rmixer4, 1);
AudioConnection          patchCord37(RightEnvelopeBouton13, 0, Rmixer4, 2);
AudioConnection          patchCord38(RightEnvelopeBouton10, 0, Rmixer3, 2);
AudioConnection          patchCord39(RightEnvelopeBouton11, 0, Rmixer4, 0);
AudioConnection          patchCord40(RightEnvelopeBouton6, 0, Rmixer2, 1);
AudioConnection          patchCord41(RightEnvelopeBouton7, 0, Rmixer2, 2);
AudioConnection          patchCord42(RightEnvelopeBouton8, 0, Rmixer3, 0);
AudioConnection          patchCord43(RightEnvelopeBouton9, 0, Rmixer3, 1);
AudioConnection          patchCord44(RightEnvelopeBouton4, 0, Rmixer1, 3);
AudioConnection          patchCord45(RightEnvelopeBouton5, 0, Rmixer2, 0);
AudioConnection          patchCord46(LeftEnvelopeBouton15, 0, Lmixer5, 2);
AudioConnection          patchCord47(LeftEnvelopeBouton16, 0, Lmixer5, 3);
AudioConnection          patchCord48(RightEnvelopeBouton2, 0, Rmixer1, 1);
AudioConnection          patchCord49(LeftEnvelopeBouton11, 0, Lmixer4, 1);
AudioConnection          patchCord50(RightEnvelopeBouton3, 0, Rmixer1, 2);
AudioConnection          patchCord51(LeftEnvelopeBouton12, 0, Lmixer4, 2);
AudioConnection          patchCord52(LeftEnvelopeBouton13, 0, Lmixer4, 3);
AudioConnection          patchCord53(LeftEnvelopeBouton14, 0, Lmixer5, 1);
AudioConnection          patchCord54(LeftEnvelopeBouton9, 0, Lmixer3, 2);
AudioConnection          patchCord55(RightEnvelopeBouton1, 0, Rmixer1, 0);
AudioConnection          patchCord56(LeftEnvelopeBouton10, 0, Lmixer3, 3);
AudioConnection          patchCord57(LeftEnvelopeBouton7, 0, Lmixer2, 3);
AudioConnection          patchCord58(LeftEnvelopeBouton8, 0, Lmixer3, 1);
AudioConnection          patchCord59(LeftEnvelopeBouton3, 0, Lmixer1, 2);
AudioConnection          patchCord60(LeftEnvelopeBouton4, 0, Lmixer1, 3);
AudioConnection          patchCord61(LeftEnvelopeBouton5, 0, Lmixer2, 1);
AudioConnection          patchCord62(LeftEnvelopeBouton6, 0, Lmixer2, 2);
AudioConnection          patchCord63(LeftEnvelopeBouton1, 0, Lmixer1, 0);
AudioConnection          patchCord64(LeftEnvelopeBouton2, 0, Lmixer1, 1);
AudioConnection          patchCord65(Lmixer1, 0, Lmixer2, 0);
AudioConnection          patchCord66(Rmixer1, 0, Rmixer2, 3);
AudioConnection          patchCord67(Lmixer2, 0, Lmixer3, 0);
AudioConnection          patchCord68(Rmixer2, 0, Rmixer3, 3);
AudioConnection          patchCord69(Lmixer3, 0, Lmixer4, 0);
AudioConnection          patchCord70(Rmixer3, 0, Rmixer4, 3);
AudioConnection          patchCord71(Lmixer4, 0, Lmixer5, 0);
AudioConnection          patchCord72(Rmixer4, 0, Rmixer5, 3);
AudioConnection          patchCord73(Lmixer5, 0, LmixerReverb, 0);
AudioConnection          patchCord74(Lmixer5, freeverbs1);
AudioConnection          patchCord75(Lmixer5, chorus1);
AudioConnection          patchCord76(Lmixer5, 0, LmixerChorus, 0);
AudioConnection          patchCord77(Lmixer5, flange1);
AudioConnection          patchCord78(Lmixer5, 0, LmixerFlange, 0);
AudioConnection          patchCord79(Lmixer5, 0, Leffectmixer1, 3);
AudioConnection          patchCord80(Rmixer5, 0, RmixerReverb, 0);
AudioConnection          patchCord81(Rmixer5, freeverbs2);
AudioConnection          patchCord82(Rmixer5, chorus2);
AudioConnection          patchCord83(Rmixer5, 0, RmixerChorus, 0);
AudioConnection          patchCord84(Rmixer5, flange2);
AudioConnection          patchCord85(Rmixer5, 0, RmixerFlange, 0);
AudioConnection          patchCord86(Rmixer5, 0, Reffectmixer1, 3);
AudioConnection          patchCord87(flange1, 0, LmixerFlange, 1);
AudioConnection          patchCord88(chorus1, 0, LmixerChorus, 1);
AudioConnection          patchCord89(freeverbs1, 0, ReverbStereo1, 3);
AudioConnection          patchCord90(freeverbs1, 1, ReverbStereo2, 2);
AudioConnection          patchCord91(chorus2, 0, RmixerChorus, 1);
AudioConnection          patchCord92(flange2, 0, RmixerFlange, 1);
AudioConnection          patchCord93(freeverbs2, 0, ReverbStereo1, 2);
AudioConnection          patchCord94(freeverbs2, 1, ReverbStereo2, 3);
AudioConnection          patchCord95(ReverbStereo2, 0, RmixerReverb, 1);
AudioConnection          patchCord96(ReverbStereo1, 0, LmixerReverb, 1);
AudioConnection          patchCord97(RmixerReverb, 0, Reffectmixer1, 0);
AudioConnection          patchCord98(RmixerChorus, 0, Reffectmixer1, 1);
AudioConnection          patchCord99(RmixerFlange, 0, Reffectmixer1, 2);
AudioConnection          patchCord100(LmixerReverb, 0, Leffectmixer1, 0);
AudioConnection          patchCord101(LmixerChorus, 0, Leffectmixer1, 1);
AudioConnection          patchCord102(LmixerFlange, 0, Leffectmixer1, 2);
AudioConnection          patchCord103(Leffectmixer1, 0, i2s1, 0);
AudioConnection          patchCord104(Leffectmixer1, rms_L);
AudioConnection          patchCord105(Leffectmixer1, peak_L);
AudioConnection          patchCord106(Reffectmixer1, 0, i2s1, 1);
AudioConnection          patchCord107(Reffectmixer1, rms_R);
AudioConnection          patchCord108(Reffectmixer1, peak_R);
AudioControlSGTL5000     sgtl5000_1;     //xy=2082.6666564941406,701.9999923706055
// GUItool: end automatically generated code

//----------------------------------------------------------------------------------//
/*                                      Setup                                       */
//----------------------------------------------------------------------------------//

void setup()
{
  Serial.begin(9600);
  //I2C Interface
  Wire.begin();
  //I2C Amp (volume)
  Wire1.begin();

  AudioMemory(20);

  tft.begin();
  //tft.setClock(100000000);
  if (!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    while (1);
  }
  Serial.println("Touchscreen started");
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
  ImagePrint();
  VolumePrint();
  AffichageBatterie();

  // SPI MicroSD
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  // stop here, but print a message repetitively
  while (!(SD.begin(SDCARD_CS_PIN)))
  {
    // Draw a "insert sd card" message
    tft.fillRect(37, 117, 245, 20, ILI9341_BLACK);
    tft.drawRect(37, 117, 245, 20, ILI9341_WHITE);
    tft.setCursor(40, 119);
    tft.setTextColor(ILI9341_RED);
    tft.setTextSize(2);
    tft.println("Brancher la carte SD");
    Serial.println("Unable to access the SD card");
    delay(500);
  }
  ImagePrint();
  VolumePrint();
  AffichageBatterie();
  Serial.println("Accessed the SD card");

  // Set volume to Amp
  if (! - +setvolume(thevol))
  {
    Serial.println("Failed to set volume, MAX9744 not found!");
    delay(500);
  }

  // Set All Envelopes Parameters
  for (int i = 0; i < 32; i++) {
    envs[i]->attack(0.0);
    envs[i]->hold(20000.0);
    envs[i]->decay(0.0);
    envs[i]->sustain(0.0);
  }

  // Initialize the effect - Chorus Left and Right
  if (!chorus1.begin(l_delaylineChorus, CHORUS_DELAY_LENGTH, n_chorus))
  {
    Serial.println("AudioEffectChorus - left channel begin failed");
    while (1);
  }
  if (!chorus2.begin(r_delaylineChorus, CHORUS_DELAY_LENGTH, n_chorus))
  {
    Serial.println("AudioEffectChorus - right channel begin failed");
    while (1);
  }

  // Initialize the effect - Flange Left and Right
  flange1.begin(l_delaylineFlange, FLANGE_DELAY_LENGTH, s_idx, s_depth, s_freq);
  flange2.begin(r_delaylineFlange, FLANGE_DELAY_LENGTH, s_idx, s_depth, s_freq);

  SetEffects();

  Serial.println("setup done");

  AudioProcessorUsageMaxReset();
  AudioMemoryUsageMaxReset();
}

//unsigned long last_time = millis();
elapsedMillis frame;
elapsedMillis frame2;
uint8_t cnt = 0;

//----------------------------------------------------------------------------------//
/*                                       Main                                       */
//----------------------------------------------------------------------------------//

void loop(void) {

  //Draw Volume Peak Menu Page 2
  if ((frame > 100) && (Etat == 2))
  {
    if (peak_L.available() && peak_R.available())
    {
      frame = 0;
      uint8_t leftPeak = peak_L.read() * 60.0 * (PourcentageVolume / 100);
      uint8_t rightPeak = peak_R.read() * 60.0 * (PourcentageVolume / 100);
      if ((leftPeak != leftPeakBuffer) || (rightPeak != rightPeakBuffer))
      {
        AffichagePeak(leftPeak, rightPeak);
      }
      leftPeakBuffer = leftPeak;
      rightPeakBuffer = rightPeak;
    }
  }

  // Request the I2C bus that contains the interface infos (Buttons/Encoders)
  Wire.requestFrom(2, 11);  // request 6 bytes from slave device #8

  // Read bus if available
  while (Wire.available())
  {
    // Reading Fonction
    LectureI2C(Wire.read());
  }

  // If Touchscreen Is Touched
  if (ts.touched())
  {
    TestTactilAppuye();
  }
}

//----------------------------------------------------------------------------------//
/*                              Fonction Affichage Image                            */
//----------------------------------------------------------------------------------//

void ImagePrint() {

  switch (Etat) {
    case 1:
      //tft.writeRect(0, 0, 320, 240, (uint16_t*)MenuPNGPage1);

      MenuPrint();
      ReprintPads();

      /*tft.useFrameBuffer(1);
        tft.updateScreen();*/

      break;

    case 2:
      //tft.writeRect(0, 0, 320, 240, (uint16_t*)MenuPNGPage1);

      MenuPrint();

      /*tft.useFrameBuffer(1);
        tft.updateScreen();*/

      break;

    case 3:
      //tft.writeRect(0, 0, 320, 240, (uint16_t*)MenuPNGPage1);

      MenuPrint();

      tft.fillRoundRect(70, 26, 245, 50, 10, ILI9341_BLACK); //279-22+5 | 162-22
      tft.drawRoundRect(70, 26, 245, 50, 10, ILI9341_WHITE); //279-22+5 | 162-22
      tft.fillRoundRect(70, 78, 245, 50, 10, ILI9341_BLACK); //279-22+5 | 162-22
      tft.drawRoundRect(70, 78, 245, 50, 10, ILI9341_WHITE); //279-22+5 | 162-22
      tft.fillRoundRect(70, 130, 245, 50, 10, ILI9341_BLACK); //279-22+5 | 162-22
      tft.drawRoundRect(70, 130, 245, 50, 10, ILI9341_WHITE); //279-22+5 | 162-22
      tft.fillRoundRect(70, 182, 245, 50, 10, ILI9341_BLACK); //279-22+5 | 162-22
      tft.drawRoundRect(70, 182, 245, 50, 10, ILI9341_WHITE); //279-22+5 | 162-22

      tft.setCursor(105, 34);
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(5);
      tft.println("Reverb");
      tft.setCursor(105, 85);
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(5);
      tft.println("Chorus");
      tft.setCursor(105, 136);
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(5);
      tft.println("Flange");
      tft.setCursor(75, 189);
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(5);
      tft.println("Envelope");
      /*tft.useFrameBuffer(1);
        tft.updateScreen();*/

      break;

    case 4:
      //tft.writeRect(0, 0, 320, 240, (uint16_t*)MenuPNGPage1);

      MenuPrint();

      if ((Action == 0) || (Action == 2) || (Action == 3) || (Action == 4))
      {
        tft.fillRoundRect(70, 26, 245, 50, 10, ILI9341_BLACK); //279-22+5 | 162-22
        tft.drawRoundRect(70, 26, 245, 50, 10, ILI9341_WHITE); //279-22+5 | 162-22
      }
      else
      {
        tft.fillRoundRect(70, 26, 245, 50, 10, ILI9341_BLACK); //279-22+5 | 162-22
        tft.drawRoundRect(70, 26, 245, 50, 10, ILI9341_RED); //279-22+5 | 162-22
      }

      if ((Action == 0) || (Action == 1) || (Action == 3) || (Action == 4))
      {
        tft.fillRoundRect(70, 78, 245, 50, 10, ILI9341_BLACK); //279-22+5 | 162-22
        tft.drawRoundRect(70, 78, 245, 50, 10, ILI9341_WHITE); //279-22+5 | 162-22
      }
      else
      {
        tft.fillRoundRect(70, 78, 245, 50, 10, ILI9341_BLACK); //279-22+5 | 162-22
        tft.drawRoundRect(70, 78, 245, 50, 10, ILI9341_RED); //279-22+5 | 162-22
      }

      if ((Action == 0) || (Action == 1) || (Action == 2) || (Action == 4))
      {
        tft.fillRoundRect(70, 130, 245, 50, 10, ILI9341_BLACK); //279-22+5 | 162-22
        tft.drawRoundRect(70, 130, 245, 50, 10, ILI9341_WHITE); //279-22+5 | 162-22
      }
      else
      {
        tft.fillRoundRect(70, 130, 245, 50, 10, ILI9341_BLACK); //279-22+5 | 162-22
        tft.drawRoundRect(70, 130, 245, 50, 10, ILI9341_RED); //279-22+5 | 162-22
      }

      if ((Action == 0) || (Action == 1) || (Action == 2) || (Action == 3))
      {
        tft.fillRoundRect(70, 182, 245, 50, 10, ILI9341_BLACK); //279-22+5 | 162-22
        tft.drawRoundRect(70, 182, 245, 50, 10, ILI9341_WHITE); //279-22+5 | 162-22
      }
      else
      {
        tft.fillRoundRect(70, 182, 245, 50, 10, ILI9341_BLACK); //279-22+5 | 162-22
        tft.drawRoundRect(70, 182, 245, 50, 10, ILI9341_RED); //279-22+5 | 162-22
      }
      tft.setCursor(105, 34);
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(5);
      tft.println("Reverb");

      tft.setCursor(105, 85);
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(5);
      tft.println("Chorus");

      tft.setCursor(105, 136);
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(5);
      tft.println("Flange");

      tft.setCursor(75, 189);
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(5);
      tft.println("Envelope");
      break;

    case 5:
      //tft.writeRect(0, 0, 320, 240, (uint16_t*)MenuPNGPage1);
      MenuPrint();
      break;

    case 6:
      //tft.writeRect(0, 0, 320, 240, (uint16_t*)MenuPNGPage1);
      MenuPrint();
      break;

    case 7:
      //tft.writeRect(0, 0, 320, 240, (uint16_t*)MenuPNGPage1);
      MenuPrint();
      break;

    case 8:
      // Envelope
      //tft.writeRect(0, 0, 320, 240, (uint16_t*)MenuPNGPage1);
      MenuPrint();

      tft.fillRect(73, 26, 238, 208, 0x39C7);
      tft.fillRect(79, 32, 226, 144, 0x0000);
      tft.fillRect(81, 34, 222, 140, 0x0861);

      tft.fillCircle(102, 205, 23, 0x0861);
      tft.fillCircle(160, 205, 23, 0x0861);
      tft.fillCircle(218, 205, 23, 0x0861);
      tft.fillCircle(276, 205, 23, 0x0861);

      tft.fillCircle(102, 205, 19, 0x2945);
      tft.fillCircle(160, 205, 19, 0x2945);
      tft.fillCircle(218, 205, 19, 0x2945);
      tft.fillCircle(276, 205, 19, 0x2945);
      break;

    default:
      //tft.writeRect(0, 0, 320, 240, (uint16_t*)MenuPNGPage1);
      MenuPrint();
      break;
  }
}

//----------------------------------------------------------------------------------//
/*                           Fonction Affichage Menu                                */
//----------------------------------------------------------------------------------//

void MenuPrint() {

  tft.fillScreen(0x10A2);

  tft.fillRect(0, 0, 320, 20, 0x39C7); //Bande
  tft.setCursor(3, 7); //Volume
  tft.setTextColor(ILI9341_WHITE); //...
  tft.setTextSize(1); //...
  tft.println("Volume"); //...

  tft.fillRoundRect(293, 2, 22, 16, 2, ILI9341_WHITE); //Batterie
  tft.fillRoundRect(315, 6, 2, 8, 2, ILI9341_WHITE); //...

  tft.fillRoundRect(5, 26, 60, 60, 8, 0x39C7); //Home
  tft.fillRoundRect(5, 98, 60, 60, 8, 0x39C7); //Mixer
  tft.fillRoundRect(5, 170, 60, 60, 8, 0x39C7); //Settings

  //Logo 1
  tft.fillRoundRect(13, 34, 20, 20, 4, ILI9341_BLACK); //Block
  tft.fillRoundRect(37, 34, 20, 20, 4, ILI9341_BLACK); //...
  tft.fillRoundRect(13, 58, 20, 20, 4, ILI9341_BLACK); //...
  tft.fillRoundRect(37, 58, 20, 20, 4, ILI9341_BLACK); //...

  tft.drawRoundRect(13, 34, 20, 20, 4, ILI9341_WHITE); //Contour
  tft.drawRoundRect(37, 34, 20, 20, 4, ILI9341_WHITE); //...
  tft.drawRoundRect(13, 58, 20, 20, 4, ILI9341_WHITE); //...
  tft.drawRoundRect(37, 58, 20, 20, 4, ILI9341_WHITE); //...

  //Logo 2
  tft.fillRoundRect(18, 104, 8, 48, 3, ILI9341_BLACK); //Slide
  tft.drawRoundRect(18, 104, 8, 48, 3, ILI9341_WHITE); //Contour Slide

  tft.fillRoundRect(44, 104, 8, 48, 3, ILI9341_BLACK); //Slide2
  tft.drawRoundRect(44, 104, 8, 48, 3, ILI9341_WHITE); //Contour Slide2

  tft.fillRoundRect(12, 112, 20, 10, 2, ILI9341_BLACK); //Knob
  tft.drawRoundRect(12, 112, 20, 10, 2, ILI9341_WHITE); //Contour Knob

  tft.fillRoundRect(38, 134, 20, 10, 2, ILI9341_BLACK); //Knob
  tft.drawRoundRect(38, 134, 20, 10, 2, ILI9341_WHITE); //Contour Knob

  //Logo 3
  tft.fillRect(14, 200, 6, 20, ILI9341_BLACK); //1 Block
  tft.fillRect(23, 186, 6, 34, ILI9341_BLACK); //2
  tft.fillRect(32, 180, 6, 40, ILI9341_BLACK); //3
  tft.fillRect(41, 192, 6, 28, ILI9341_BLACK); //4
  tft.fillRect(50, 205, 6, 15, ILI9341_BLACK); //5

  tft.drawRect(14, 200, 6, 20, ILI9341_WHITE); //1 Contour
  tft.drawRect(23, 186, 6, 34, ILI9341_WHITE); //2
  tft.drawRect(32, 180, 6, 40, ILI9341_WHITE); //3
  tft.drawRect(41, 192, 6, 28, ILI9341_WHITE); //4
  tft.drawRect(50, 205, 6, 15, ILI9341_WHITE); //5
}

//----------------------------------------------------------------------------------//
/*                           Fonction Affichage Volume                              */
//----------------------------------------------------------------------------------//

void VolumePrint() {
  tft.fillRect(73, 2, 190, 16, 0x10A2); //Clear
  tft.fillRect(73, 2, int16_t(Slide), 16, ILI9341_RED); //Draw
  tft.drawRect(73, 2, 190, 16, ILI9341_WHITE); //Contour

  tft.fillRect(44, 2, 27, 15, 0x39C7);
  tft.setCursor(48, 8);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.print(int16_t(PourcentageVolume)); tft.println('%');
}

//----------------------------------------------------------------------------------//
/*                           Fonction Affichage Mixer1                              */
//----------------------------------------------------------------------------------//

void MixerPrint(int MixerModifer) {

  switch (Etat) {

    case 2:

      switch (MixerModifer) {

        case 1:
          tft.fillRoundRect(80, 26, 44, 144, 5, 0x10A2); //Clear

          tft.fillRoundRect(97,  26, 10, 136, 5, ILI9341_BLACK); //Slides
          tft.drawRoundRect(97,  26, 10, 136, 5, ILI9341_WHITE);

          tft.fillRoundRect(80, Mixer1, 44, 22, 5, ILI9341_BLACK); //Knob
          tft.drawRoundRect(80, Mixer1, 44, 22, 5, ILI9341_WHITE);
          break;

        case 2:
          tft.fillRoundRect(141, 26, 44, 144, 5, 0x10A2); //Clear

          tft.fillRoundRect(158, 26, 10, 136, 5, ILI9341_BLACK); //Slides
          tft.drawRoundRect(158, 26, 10, 136, 5, ILI9341_WHITE);

          tft.fillRoundRect(141, Mixer2, 44, 22, 5, ILI9341_BLACK); //Knob
          tft.drawRoundRect(141, Mixer2, 44, 22, 5, ILI9341_WHITE);
          break;

        case 3:
          tft.fillRoundRect(201, 26, 44, 144, 5, 0x10A2); //Clear

          tft.fillRoundRect(218, 26, 10, 136, 5, ILI9341_BLACK); //Slides
          tft.drawRoundRect(218, 26, 10, 136, 5, ILI9341_WHITE);

          tft.fillRoundRect(201, Mixer3, 44, 22, 5, ILI9341_BLACK); //Knob
          tft.drawRoundRect(201, Mixer3, 44, 22, 5, ILI9341_WHITE);
          break;

        case 4:

          tft.fillRoundRect(97,  26, 10, 136, 5, ILI9341_BLACK); //Slides
          tft.drawRoundRect(97,  26, 10, 136, 5, ILI9341_WHITE);
          tft.fillRoundRect(80, Mixer1, 44, 22, 5, ILI9341_BLACK); //Knob
          tft.drawRoundRect(80, Mixer1, 44, 22, 5, ILI9341_WHITE);

          tft.fillRoundRect(158, 26, 10, 136, 5, ILI9341_BLACK); //Slides
          tft.drawRoundRect(158, 26, 10, 136, 5, ILI9341_WHITE);
          tft.fillRoundRect(141, Mixer2, 44, 22, 5, ILI9341_BLACK); //Knob
          tft.drawRoundRect(141, Mixer2, 44, 22, 5, ILI9341_WHITE);

          tft.fillRoundRect(218, 26, 10, 136, 5, ILI9341_BLACK); //Slides
          tft.drawRoundRect(218, 26, 10, 136, 5, ILI9341_WHITE);
          tft.fillRoundRect(201, Mixer3, 44, 22, 5, ILI9341_BLACK); //Knob
          tft.drawRoundRect(201, Mixer3, 44, 22, 5, ILI9341_WHITE);
          break;
      }

      tft.fillRoundRect(70, 170, 185, 64, 10, ILI9341_BLACK); //Carré noire (%)
      tft.fillRect(260, 26, 50, 208, ILI9341_BLACK); //Peak

      tft.drawRoundRect(70, 170, 185, 64, 10, ILI9341_WHITE); //Contour carré noire (%)
      tft.drawRect(260, 26, 25, 208, ILI9341_WHITE); //Contour L peak
      tft.drawRect(285, 26, 25, 208, ILI9341_WHITE); //Contour R peak

      tft.setCursor(80, 182);
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(1);
      tft.println("Mixer1:");
      tft.setCursor(77, 200);
      tft.setTextSize(3);
      tft.print(int16_t(PourcentageMixer1)); tft.println('%');

      tft.setCursor(141, 182);
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(1);
      tft.println("Mixer2:");
      tft.setCursor(138, 200);
      tft.setTextSize(3);
      tft.print(int16_t(PourcentageMixer2)); tft.println('%');

      tft.setCursor(202, 182);
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(1);
      tft.println("Mixer3:");
      tft.setCursor(199, 200);
      tft.setTextSize(3);
      tft.print(int16_t(PourcentageMixer3)); tft.println('%');
      break;

    case 5:

      switch (MixerModifer) {

        case 1:
          tft.fillRoundRect(89, 26, 44, 144, 5, 0x10A2); // Clear

          tft.fillRoundRect(105,  26, 10, 136, 5, ILI9341_BLACK); // Slide
          tft.drawRoundRect(105,  26, 10, 136, 5, ILI9341_WHITE);

          tft.fillRoundRect(89, Roomsize, 44, 22, 5, ILI9341_BLACK); // Knob
          tft.drawRoundRect(89, Roomsize, 44, 22, 5, ILI9341_WHITE);
          break;

        case 2:
          tft.fillRoundRect(170, 26, 44, 144, 5, 0x10A2); // Clear

          tft.fillRoundRect(186,  26, 10, 136, 5, ILI9341_BLACK); // Slide
          tft.drawRoundRect(186,  26, 10, 136, 5, ILI9341_WHITE);

          tft.fillRoundRect(170, Damping, 44, 22, 5, ILI9341_BLACK); // Knob
          tft.drawRoundRect(170, Damping, 44, 22, 5, ILI9341_WHITE);
          break;

        case 3:
          tft.fillRoundRect(252, 26, 44, 144, 5, 0x10A2); // Clear

          tft.fillRoundRect(268, 26, 10, 136, 5, ILI9341_BLACK); // Slide
          tft.drawRoundRect(268, 26, 10, 136, 5, ILI9341_WHITE);

          tft.fillRoundRect(252, Mixer1, 44, 22, 5, ILI9341_BLACK); // Knob
          tft.drawRoundRect(252, Mixer1, 44, 22, 5, ILI9341_WHITE);
          break;

        case 4:
          tft.fillRoundRect(105,  26, 10, 136, 5, ILI9341_BLACK); //Slide
          tft.drawRoundRect(105,  26, 10, 136, 5, ILI9341_WHITE);

          tft.fillRoundRect(89, Roomsize, 44, 22, 5, ILI9341_BLACK); // Knob
          tft.drawRoundRect(89, Roomsize, 44, 22, 5, ILI9341_WHITE);

          tft.fillRoundRect(186,  26, 10, 136, 5, ILI9341_BLACK); // Slide
          tft.drawRoundRect(186,  26, 10, 136, 5, ILI9341_WHITE);

          tft.fillRoundRect(170, Damping, 44, 22, 5, ILI9341_BLACK); // Knob
          tft.drawRoundRect(170, Damping, 44, 22, 5, ILI9341_WHITE);

          tft.fillRoundRect(268, 26, 10, 136, 5, ILI9341_BLACK); // Slide
          tft.drawRoundRect(268, 26, 10, 136, 5, ILI9341_WHITE);

          tft.fillRoundRect(252, Mixer1, 44, 22, 5, ILI9341_BLACK); // Knob
          tft.drawRoundRect(252, Mixer1, 44, 22, 5, ILI9341_WHITE);
          break;
      }
      //

      tft.fillRoundRect(70, 170, 245, 64, 10, ILI9341_BLACK);
      tft.drawRoundRect(70, 170, 245, 64, 10, ILI9341_WHITE);

      tft.setCursor(89, 182);
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(1);
      tft.println("Roomsize:");
      tft.setCursor(79, 200);
      tft.setTextSize(3);
      tft.print(float(NombreRoomsize));

      tft.setCursor(170, 182);
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(1);
      tft.println("Damping:");
      tft.setCursor(160, 200);
      tft.setTextSize(3);
      tft.print(float(NombreDamping));

      tft.setCursor(252, 182);
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(1);
      tft.println("Mixer1:");
      tft.setCursor(252, 200);
      tft.setTextSize(3);
      tft.print(int16_t(PourcentageMixer1)); tft.println('%');

      break;

    case 6:

      switch (MixerModifer) {

        case 1:

          tft.fillRoundRect(109, 26, 44, 144, 5, 0x10A2); //Clear

          tft.fillRoundRect(126,  26, 10, 136, 5, ILI9341_BLACK); //Slide
          tft.drawRoundRect(126,  26, 10, 136, 5, ILI9341_WHITE);

          tft.fillRoundRect(109, Voix, 44, 22, 5, ILI9341_BLACK); //Knob
          tft.drawRoundRect(109, Voix, 44, 22, 5, ILI9341_WHITE);
          break;

        case 2:

          tft.fillRoundRect(231, 26, 44, 144, 5, 0x10A2); //Clear

          tft.fillRoundRect(248, 26, 10, 136, 5, ILI9341_BLACK); //Slide
          tft.drawRoundRect(248, 26, 10, 136, 5, ILI9341_WHITE);

          tft.fillRoundRect(231, Mixer2, 44, 22, 5, ILI9341_BLACK); //Knob
          tft.drawRoundRect(231, Mixer2, 44, 22, 5, ILI9341_WHITE);
          break;

        case 3:

          tft.fillRoundRect(126,  26, 10, 136, 5, ILI9341_BLACK); //Slide
          tft.drawRoundRect(126,  26, 10, 136, 5, ILI9341_WHITE);

          tft.fillRoundRect(109, Voix, 44, 22, 5, ILI9341_BLACK); //Knob
          tft.drawRoundRect(109, Voix, 44, 22, 5, ILI9341_WHITE);

          tft.fillRoundRect(248, 26, 10, 136, 5, ILI9341_BLACK); //Slide
          tft.drawRoundRect(248, 26, 10, 136, 5, ILI9341_WHITE);

          tft.fillRoundRect(231, Mixer2, 44, 22, 5, ILI9341_BLACK); //Knob
          tft.drawRoundRect(231, Mixer2, 44, 22, 5, ILI9341_WHITE);
          break;
      }

      tft.fillRoundRect(70, 170, 245, 64, 10, ILI9341_BLACK);
      tft.drawRoundRect(70, 170, 245, 64, 10, ILI9341_WHITE);

      tft.setCursor(90, 182);
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(1);
      tft.println("Nombre de voix:");
      tft.setCursor(125, 200);
      tft.setTextSize(3);
      tft.print(int16_t(NombreVoix));

      tft.setCursor(235, 182);
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(1);
      tft.println("Mixer2:");
      tft.setCursor(235, 200);
      tft.setTextSize(3);
      tft.print(int16_t(PourcentageMixer2)); tft.println('%');
      break;

    case 7:

      switch (MixerModifer) {

        case 1:

          tft.fillRoundRect(80, 26, 44, 144, 5, 0x10A2); //Clear

          tft.fillRoundRect(97,  26, 10, 136, 5, ILI9341_BLACK); //Slide
          tft.drawRoundRect(97,  26, 10, 136, 5, ILI9341_WHITE);

          tft.fillRoundRect(80, Offset, 44, 22, 5, ILI9341_BLACK); //Knob
          tft.drawRoundRect(80, Offset, 44, 22, 5, ILI9341_WHITE);
          break;

        case 2:

          tft.fillRoundRect(141, 26, 44, 144, 5, 0x10A2); //Clear

          tft.fillRoundRect(158, 26, 10, 136, 5, ILI9341_BLACK); //Slide
          tft.drawRoundRect(158, 26, 10, 136, 5, ILI9341_WHITE);

          tft.fillRoundRect(141, Depth, 44, 22, 5, ILI9341_BLACK); //Knob
          tft.drawRoundRect(141, Depth, 44, 22, 5, ILI9341_WHITE);
          break;

        case 3:

          tft.fillRoundRect(201, 26, 44, 144, 5, 0x10A2); //Clear

          tft.fillRoundRect(218, 26, 10, 136, 5, ILI9341_BLACK); //Slide
          tft.drawRoundRect(218, 26, 10, 136, 5, ILI9341_WHITE);

          tft.fillRoundRect(201, DelayRate, 44, 22, 5, ILI9341_BLACK); //Knob
          tft.drawRoundRect(201, DelayRate, 44, 22, 5, ILI9341_WHITE);
          break;

        case 4:

          tft.fillRoundRect(262, 26, 44, 144, 5, 0x10A2); //Clear

          tft.fillRoundRect(279, 26, 10, 136, 5, ILI9341_BLACK); //Slide
          tft.drawRoundRect(279, 26, 10, 136, 5, ILI9341_WHITE);

          tft.fillRoundRect(262, Mixer3, 44, 22, 5, ILI9341_BLACK); //Knob
          tft.drawRoundRect(262, Mixer3, 44, 22, 5, ILI9341_WHITE);
          break;

        case 5:

          tft.fillRoundRect(97,  26, 10, 136, 5, ILI9341_BLACK); //Slide
          tft.drawRoundRect(97,  26, 10, 136, 5, ILI9341_WHITE);
          tft.fillRoundRect(80, Offset, 44, 22, 5, ILI9341_BLACK); //Knob
          tft.drawRoundRect(80, Offset, 44, 22, 5, ILI9341_WHITE);

          tft.fillRoundRect(158, 26, 10, 136, 5, ILI9341_BLACK); //Slide
          tft.drawRoundRect(158, 26, 10, 136, 5, ILI9341_WHITE);
          tft.fillRoundRect(141, Depth, 44, 22, 5, ILI9341_BLACK); //Knob
          tft.drawRoundRect(141, Depth, 44, 22, 5, ILI9341_WHITE);

          tft.fillRoundRect(218, 26, 10, 136, 5, ILI9341_BLACK); //Slide
          tft.drawRoundRect(218, 26, 10, 136, 5, ILI9341_WHITE);
          tft.fillRoundRect(201, DelayRate, 44, 22, 5, ILI9341_BLACK); //Knob
          tft.drawRoundRect(201, DelayRate, 44, 22, 5, ILI9341_WHITE);

          tft.fillRoundRect(279, 26, 10, 136, 5, ILI9341_BLACK); //Slide
          tft.drawRoundRect(279, 26, 10, 136, 5, ILI9341_WHITE);
          tft.fillRoundRect(262, Mixer3, 44, 22, 5, ILI9341_BLACK); //Knob
          tft.drawRoundRect(262, Mixer3, 44, 22, 5, ILI9341_WHITE);
          break;
      }

      tft.fillRoundRect(70, 170, 245, 64, 10, ILI9341_BLACK); //279-22+5 | 162-22
      tft.drawRoundRect(70, 170, 245, 64, 10, ILI9341_WHITE); //279-22+5 | 162-22

      tft.setCursor(85, 182);
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(1);
      tft.println("Offset:");
      tft.setCursor(85, 200);
      tft.setTextSize(3);
      tft.print(int16_t(NombreOffset)); tft.println('x');

      tft.setCursor(145, 182);
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(1);
      tft.println("Depth:");
      tft.setCursor(143, 200);
      tft.setTextSize(3);
      tft.print('%'); tft.println(int16_t(NombreDepth));

      tft.setCursor(190, 182);
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(1);
      tft.println("Delay Rate:");
      if (NombreDelayRate >= 10)
      {
        tft.setCursor(183, 200);
      }
      else
      {
        tft.setCursor(190, 200);
      }
      tft.setTextSize(3);
      tft.print(int16_t(NombreDelayRate)); tft.println("Hz");

      tft.setCursor(263, 182);
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(1);
      tft.println("Mixer3:");
      tft.setCursor(260, 200);
      tft.setTextSize(3);
      tft.print(int16_t(PourcentageMixer3)); tft.println('%');
      break;

    case 8:

      // Envelope
      switch (MixerModifer) {

        //Clear
        case 1:
          //1ere Ligne (Attack)
          tft.drawLine(81, 174, PixelAttack, 40, 0x0861);
          tft.fillCircle(PixelAttack, 40, 2, 0x0861);
          //2eme Ligne (Hold)
          tft.drawLine(PixelAttack, 40, PixelHolds, 40, 0x0861); //Additionne
          tft.fillCircle(PixelHolds, 40, 2, 0x0861);
          //3eme Ligne (Decay)
          tft.drawLine(PixelHolds, 40, PixelDecay, PixelSustains, 0x0861);
          tft.fillCircle(PixelDecay, PixelSustains, 2, 0x0861);
          //4eme Ligne (Sustain)
          tft.drawLine(PixelDecay, PixelSustains, 303, PixelSustains, 0x0861); //81 + 56 = 137 et 174 = plus bas
          break;

        //Afficher
        case 2:
          //1ere Ligne (Attack)
          tft.drawLine(81, 174, PixelAttack, 40, ILI9341_WHITE);
          tft.fillCircle(PixelAttack, 40, 2, ILI9341_WHITE);
          //2eme Ligne (Hold)
          tft.drawLine(PixelAttack, 40, PixelHolds, 40, ILI9341_WHITE); //Additionne
          tft.fillCircle(PixelHolds, 40, 2, ILI9341_WHITE);
          //3eme Ligne (Decay)
          tft.drawLine(PixelHolds, 40, PixelDecay, PixelSustains, ILI9341_WHITE);
          tft.fillCircle(PixelDecay, PixelSustains, 2, ILI9341_WHITE);
          //4eme Ligne (Sustain)
          tft.drawLine(PixelDecay, PixelSustains, 303, PixelSustains, ILI9341_WHITE); //81 + 56 = 137 et 174 = plus bas
          break;
      }
      break;
  }
}

//----------------------------------------------------------------------------------//
/*                          Fonction Affichage Batterie                             */
//----------------------------------------------------------------------------------//

void  AffichageBatterie()
{
  PositionBatterie = float(0.18 * pourcentage);
  tft.fillRect(295,  4, 18, 12, 0x10A2);
  tft.fillRect(295,  4, PositionBatterie, 12, ILI9341_GREEN);

  tft.fillRect(268, 2, 22, 15, 0x39C7);
  tft.setCursor(270, 8);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.print(int16_t(pourcentage)); tft.println('%');
  Serial.println(pourcentage);
}

//----------------------------------------------------------------------------------//
/*                          Fonction Affichage Peak                                 */
//----------------------------------------------------------------------------------//

void  AffichagePeak(uint8_t peakleft, uint8_t peakright)
{
  uint16_t  RightPeakLength = -6.933333333 * peakright + 234;
  uint16_t  RightPeakHight = 234 - RightPeakLength;

  tft.fillRect(260, 26, 25, 208, ILI9341_BLACK); //279-22+5 | 162-22à
  tft.drawRect(260, 26, 25, 208, ILI9341_WHITE); //279-22+5 | 162-22
  tft.drawRect(260, RightPeakLength, 25, RightPeakHight, ILI9341_GREEN);

  uint16_t LeftPeakLength = -6.933333333 * peakright + 234;
  uint16_t  LeftPeakHight = 234 - LeftPeakLength;

  tft.fillRect(285, 26, 25, 208, ILI9341_BLACK); //279-22+5 | 162-22
  tft.drawRect(285, 26, 25, 208, ILI9341_WHITE); //279-22+5 | 162-22
  tft.drawRect(285, RightPeakLength, 25, LeftPeakHight, ILI9341_GREEN);
}

//----------------------------------------------------------------------------------//
/*                              Fonction Test Tactil                                */
//----------------------------------------------------------------------------------//

void TestTactilAppuye() {

  // Retrieve a point
  TS_Point p = ts.getPoint();

  // Scale using the calibration #'s
  // and rotate coordinate system
  p.x = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());
  p.y = map(p.y, TS_MAXX, TS_MINX, 0, tft.width());

  /*Serial.print("X = "); Serial.print(p.x);
    Serial.print("\tY = "); Serial.print(p.y);
    Serial.print("\tPressure = "); Serial.println(p.z);*/

  //Boutons tactils
  if (p.x > 180) {

    if ((p.y > 40) && (p.y < 120) && (Etat != 1)) {
      //tft.fillScreen(ILI9341_BLACK);
      Etat = 1;
      ImagePrint();
      VolumePrint();
      AffichageBatterie();
    } else if ((p.y > 130) && (p.y < 210) && (Etat != 2)) {
      //tft.fillScreen(ILI9341_BLACK);
      Etat = 2;
      ImagePrint();
      VolumePrint();
      MixerPrint(4);
      AffichageBatterie();
    } else if ((p.y > 255) && (p.y < 304) && (Etat != 3)) {
      //tft.fillScreen(ILI9341_BLACK);
      Etat = 3;
      ImagePrint();
      VolumePrint();
      AffichageBatterie();
    }
    Menu3Selected = 0;
    Action = 0;
    Menu4Selected = 0;
    Action2 = 0;
  }
  //Volume
  else if ((p.y >= 0) && (p.y < 30)) {
    if ((p.x > 50) && (p.x < 180)) {
      Slide = float((p.x - 181.0) / -0.6842);
      PourcentageVolume = float(100.0 * Slide / 190.0);
      int v = float((PourcentageVolume * m) + 16);
      setvolume(v);
      //Serial.println(PourcentageVolume);
      //Serial.println(Slide);
      //Serial.print("X = "); Serial.print(p.x);
      //Serial.print("\tY = "); Serial.print(p.y);
      //Serial.print("\tPressure = "); Serial.println(p.z);
      if ((Slide > SlideBuffer + 4) || (Slide < SlideBuffer - 4))
      {
        VolumePrint();
      }
      SlideBuffer = Slide;
    }
  }
  //Mixer1
  else if ((Etat == 2) && ((p.x >= 150) && (p.x < 175))) {
    if ((p.y > 40) && (p.y < 210)) {
      Mixer1 = float((0.670588235 * p.y) - 0.8235);
      PourcentageMixer1 = float(-0.877143 * Mixer1 + 122.80702);
      SetEffects();
      MixerPrint(1);
    }
  }
  //Mixer2
  else if ((Etat == 2) && ((p.x >= 110) && (p.x < 135))) {
    if ((p.y > 40) && (p.y < 210)) {
      Mixer2 = float((0.670588235 * p.y) - 0.8235);
      PourcentageMixer2 = float(-0.877143 * Mixer2 + 122.80702);
      SetEffects();
      MixerPrint(2);
    }
  }
  //Mixer3
  else if ((Etat == 2) && ((p.x >= 70) && (p.x < 95))) {
    if ((p.y > 40) && (p.y < 210)) {
      Mixer3 = float((0.670588235 * p.y) - 0.8235);
      PourcentageMixer3 = float(-0.877143 * Mixer3 + 122.80702);
      SetEffects();
      MixerPrint(3);
    }
  }
  //Reverb
  else if ((Etat == 3) && ((p.y >= 36) && (p.y < 97))) {
    if ((p.x > 19) && (p.x < 177))
    {
      Etat = 5;
      ImagePrint();
      VolumePrint();
      MixerPrint(4);
      AffichageBatterie();
    }
  }
  //Chorus
  else if ((Etat == 3) && ((p.y >= 102) && (p.y < 162))) {
    if ((p.x > 19) && (p.x < 177))
    {
      Etat = 6;
      ImagePrint();
      VolumePrint();
      MixerPrint(3);
      AffichageBatterie();
    }
  }
  //Flange
  else if ((Etat == 3) && ((p.y >= 174) && (p.y < 232))) {
    if ((p.x > 19) && (p.x < 177))
    {
      Etat = 7;
      ImagePrint();
      VolumePrint();
      MixerPrint(5);
      AffichageBatterie();
    }
  }
  //Enveloe
  else if ((Etat == 3) && ((p.y >= 241) && (p.y < 299))) {
    if ((p.x > 19) && (p.x < 177))
    {
      Etat = 8;
      Attack = 10.5, Holds = 2.5, Decay = 35, Sustains = 0.5, PixelAttack = 81, PixelHolds = 81, PixelDecay = 81 , PixelSustains = 107;
      ImagePrint();
      VolumePrint();
      MixerPrint(2);
      AffichageBatterie();
    }
  }

  //Roomsize (Reverb)
  else if ((Etat == 5) && ((p.x >= 140) && (p.x < 168))) {
    if ((p.y > 40) && (p.y < 210)) {
      Roomsize = float((0.670588235 * p.y) - 0.823529412);
      NombreRoomsize = float(-0.00877193 * Roomsize + 1.228070175);
      AudioNoInterrupts();
      freeverbs1.roomsize(NombreRoomsize);
      freeverbs2.roomsize(NombreRoomsize);
      AudioInterrupts();
      MixerPrint(1);
    }
  }

  //Damping (Reverb)
  else if ((Etat == 5) && ((p.x >= 85) && (p.x < 114))) {
    if ((p.y > 40) && (p.y < 210)) {
      Damping = float((0.670588235 * p.y) - 0.823529412);
      NombreDamping = float(-0.00877193 * Damping + 1.228070175);
      AudioNoInterrupts();
      freeverbs1.damping(NombreDamping);
      freeverbs2.damping(NombreDamping);
      AudioInterrupts();
      MixerPrint(2);
    }
  }

  //Mixer1 (Reverb)
  else if ((Etat == 5) && ((p.x >= 30) && (p.x < 59))) {
    if ((p.y > 40) && (p.y < 210)) {
      Mixer1 = float((0.670588235 * p.y) - 0.8235);
      PourcentageMixer1 = float(-0.877143 * Mixer1 + 122.80702);
      SetEffects();
      MixerPrint(3);
    }
  }

  //Nombdre de voix (Chorus)
  else if ((Etat == 6) && ((p.x >= 128) && (p.x < 155))) {
    if ((p.y > 40) && (p.y < 210)) {
      Voix = float((0.670588235 * p.y) - 0.823529412);
      NombreVoix = float(-0.087719298 * Voix + 12.28070175);
      //Serial.print("Voix: "); Serial.print(uint16_t(NombreVoix));
      AudioNoInterrupts();
      chorus1.voices(uint16_t(NombreVoix));
      chorus2.voices(uint16_t(NombreVoix));
      AudioInterrupts();
      MixerPrint(1);
    }
  }
  //Mixer2 (Chorus)
  else if ((Etat == 6) && ((p.x >= 44) && (p.x < 73))) {
    if ((p.y > 40) && (p.y < 210)) {
      Mixer2 = float((0.670588235 * p.y) - 0.8235);
      PourcentageMixer2 = float(-0.877143 * Mixer2 + 122.80702);
      //Serial.println("PourcentageMixer2: "); Serial.println(PourcentageMixer2);
      SetEffects();
      MixerPrint(2);
    }
  }
  //Offset (Flange)
  else if ((Etat == 7) && ((p.x >= 150) && (p.x < 175))) {
    if ((p.y > 40) && (p.y < 210)) {
      Offset = float((0.670588235294118 * p.y) - -0.823529411764703);
      NombreOffset = float(-0.0263157894736842 * Offset + 3.68421052631579);

      AudioNoInterrupts();
      int s_idx = NombreOffset * FLANGE_DELAY_LENGTH / 4;
      flange1.voices(s_idx, s_depth, s_freq);
      flange2.voices(s_idx, s_depth, s_freq);
      AudioInterrupts();

      //Serial.print("NombreOffset: "); Serial.println(float(NombreOffset));
      MixerPrint(1);
    }
  }
  //Depth (Flange)
  else if ((Etat == 7) && ((p.x >= 110) && (p.x < 135))) {
    if ((p.y > 40) && (p.y < 125))
    {
      Depth = 54;
      NombreDepth = 4;
    }
    else if ((p.y > 125) && (p.y < 210))
    {
      Depth = 106;
      NombreDepth = 8;
    }

    AudioNoInterrupts();
    int s_depth = FLANGE_DELAY_LENGTH / NombreDepth;
    flange1.voices(s_idx, s_depth, s_freq);
    flange2.voices(s_idx, s_depth, s_freq);
    AudioInterrupts();

    //Serial.print("NombreOffset: "); Serial.println(uint16_t(NombreDepth));
    MixerPrint(2);
  }
  //DelayRate (Flange)
  else if ((Etat == 7) && ((p.x >= 70) && (p.x < 95))) {
    if ((p.y > 40) && (p.y < 210)) {
      DelayRate = float((0.670588235 * p.y) - 0.823529412);
      NombreDelayRate = float(-0.096491228 * DelayRate + 13.50877193);

      AudioNoInterrupts();
      double s_freq = NombreDelayRate;
      flange1.voices(s_idx, s_depth, s_freq);
      flange2.voices(s_idx, s_depth, s_freq);
      AudioInterrupts();

      //Serial.print("NombreOffset: "); Serial.println(uint16_t(NombreDelayRate));
      MixerPrint(3);
    }
  }
  //Mixer3 (Flange)
  else if ((Etat == 7) && ((p.x >= 25) && (p.x < 50))) {
    if ((p.y > 40) && (p.y < 210)) {
      Mixer3 = float((0.670588235 * p.y) - 0.8235);
      PourcentageMixer3 = float(-0.877143 * Mixer3 + 122.80702);
      SetEffects();
      MixerPrint(4);
    }
  }
}

//----------------------------------------------------------------------------------//
/*                              Reprint Pads                                        */
//----------------------------------------------------------------------------------//

void ReprintPads() {

  if (EtatBouton1 == 1)
  {
    if (LayerSwitch == 1)
    {
      tft.fillRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_RED);
    }
    else
    {
      tft.fillRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_GREEN);
    }
    tft.drawRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton2 == 1) {
    if (LayerSwitch == 1)
    {
      tft.fillRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_RED);
    }
    else
    {
      tft.fillRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_GREEN);
    }
    tft.drawRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton3 == 1) {
    if (LayerSwitch == 1)
    {
      tft.fillRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_RED);
    }
    else
    {
      tft.fillRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_GREEN);
    }
    tft.drawRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton4 == 1) {
    if (LayerSwitch == 1)
    {
      tft.fillRoundRect(260, 26,  PADSIZE, PADSIZE, 10, ILI9341_RED);
    }
    else
    {
      tft.fillRoundRect(260, 26,  PADSIZE, PADSIZE, 10, ILI9341_GREEN);
    }
    tft.drawRoundRect(260, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(260, 26,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(260, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton5 == 1) {
    if (LayerSwitch == 1)
    {
      tft.fillRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_RED);
    }
    else
    {
      tft.fillRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_GREEN);
    }
    tft.drawRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton6 == 1) {
    if (LayerSwitch == 1)
    {
      tft.fillRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_RED);
    }
    else
    {
      tft.fillRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_GREEN);
    }
    tft.drawRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton7 == 1) {
    if (LayerSwitch == 1)
    {
      tft.fillRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_RED);
    }
    else
    {
      tft.fillRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_GREEN);
    }
    tft.drawRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton8 == 1) {
    if (LayerSwitch == 1)
    {
      tft.fillRoundRect(260, 81,  PADSIZE, PADSIZE, 10, ILI9341_RED);
    }
    else
    {
      tft.fillRoundRect(260, 81,  PADSIZE, PADSIZE, 10, ILI9341_GREEN);
    }
    tft.drawRoundRect(260, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(260, 81,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(260, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton9 == 1) {
    if (LayerSwitch == 1)
    {
      tft.fillRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_RED);
    }
    else
    {
      tft.fillRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_GREEN);
    }
    tft.drawRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton10 == 1) {
    if (LayerSwitch == 1)
    {
      tft.fillRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_RED);
    }
    else
    {
      tft.fillRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_GREEN);
    }
    tft.drawRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton11 == 1) {
    if (LayerSwitch == 1)
    {
      tft.fillRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_RED);
    }
    else
    {
      tft.fillRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_GREEN);
    }
    tft.drawRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton12 == 1) {
    if (LayerSwitch == 1)
    {
      tft.fillRoundRect(260, 136, PADSIZE, PADSIZE, 10, ILI9341_RED);
    }
    else
    {
      tft.fillRoundRect(260, 136, PADSIZE, PADSIZE, 10, ILI9341_GREEN );
    }
    tft.drawRoundRect(260, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(260, 136, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(260, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton13 == 1) {
    if (LayerSwitch == 1)
    {
      tft.fillRoundRect(80,  191, PADSIZE, PADSIZE, 10, ILI9341_RED);
    }
    else
    {
      tft.fillRoundRect(80,  191, PADSIZE, PADSIZE, 10, ILI9341_GREEN );
    }
    tft.drawRoundRect(80,  191, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(80,  191, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(80,  191, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton14 == 1) {
    if (LayerSwitch == 1)
    {
      tft.fillRoundRect(140,  191, PADSIZE, PADSIZE, 10, ILI9341_RED);
    }
    else
    {
      tft.fillRoundRect(140,  191, PADSIZE, PADSIZE, 10, ILI9341_GREEN);
    }
    tft.drawRoundRect(140,  191, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(140,  191, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(140,  191, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton15 == 1) {
    if (LayerSwitch == 1)
    {
      tft.fillRoundRect(200,  191, PADSIZE, PADSIZE, 10, ILI9341_RED);
    }
    else
    {
      tft.fillRoundRect(200,  191, PADSIZE, PADSIZE, 10, ILI9341_GREEN);
    }
    tft.drawRoundRect(200,  191, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(200,  191, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(200,  191, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  if (EtatBouton16 == 1) {
    if (LayerSwitch == 1)
    {
      tft.fillRoundRect(260,  191, PADSIZE, PADSIZE, 10, ILI9341_RED);
    }
    else
    {
      tft.fillRoundRect(260,  191, PADSIZE, PADSIZE, 10, ILI9341_RED);
    }
    tft.drawRoundRect(260,  191, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
  else {
    tft.fillRoundRect(260,  191, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
    tft.drawRoundRect(260,  191, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
  }
}

//----------------------------------------------------------------------------------//
/*                         Setting the volume                                       */
//----------------------------------------------------------------------------------//

boolean setvolume(int8_t v) {
  // cant be higher than 63 or lower than 0
  if (v > VolumeMax)
    v = VolumeMax;
  if (v < 0)
    v = 0;

  //Serial.print("Setting volume to ");
  //Serial.println(v);
  Wire1.beginTransmission(MAX9744_I2CADDR);
  Wire1.write(v);
  if (Wire1.endTransmission() == 0)
    return true;
  else
    return false;
}

//----------------------------------------------------------------------------------//
/*                     Jouer un fichier audio                                       */
//----------------------------------------------------------------------------------//

void playFile(const char *filename, int chord)
{
  //Serial.print("Playing file: ");
  //Serial.println(filename);
  //Serial.print("Chord: ");
  //Serial.println(chord);

  if (chord == 1)
  {
    playSdWav1.play(filename);
    LeftEnvelopeBouton1.noteOn();
    RightEnvelopeBouton1.noteOn();
  }

  if (chord == 2)
  {
    playSdWav2.play(filename);
    LeftEnvelopeBouton2.noteOn();
    RightEnvelopeBouton2.noteOn();
  }
  if (chord == 3)
  {
    playSdWav3.play(filename);
    LeftEnvelopeBouton3.noteOn();
    RightEnvelopeBouton3.noteOn();
  }
  if (chord == 4)
  {
    playSdWav4.play(filename);
    LeftEnvelopeBouton4.noteOn();
    RightEnvelopeBouton4.noteOn();
  }
  if (chord == 5)
  {
    playSdWav5.play(filename);
    LeftEnvelopeBouton5.noteOn();
    RightEnvelopeBouton5.noteOn();
  }
  if (chord == 6)
  {
    playSdWav6.play(filename);
    LeftEnvelopeBouton6.noteOn();
    RightEnvelopeBouton6.noteOn();
  }
  if (chord == 7)
  {
    playSdWav7.play(filename);
    LeftEnvelopeBouton7.noteOn();
    RightEnvelopeBouton7.noteOn();
  }
  if (chord == 8)
  {
    playSdWav8.play(filename);
    LeftEnvelopeBouton8.noteOn();
    RightEnvelopeBouton8.noteOn();
  }
  if (chord == 9)
  {
    playSdWav9.play(filename);
    LeftEnvelopeBouton9.noteOn();
    RightEnvelopeBouton9.noteOn();
  }
  if (chord == 10)
  {
    playSdWav10.play(filename);
    LeftEnvelopeBouton10.noteOn();
    RightEnvelopeBouton10.noteOn();
  }
  if (chord == 11)
  {
    playSdWav11.play(filename);
    LeftEnvelopeBouton11.noteOn();
    RightEnvelopeBouton11.noteOn();
  }
  if (chord == 12)
  {
    playSdWav12.play(filename);
    LeftEnvelopeBouton12.noteOn();
    RightEnvelopeBouton12.noteOn();
  }
  if (chord == 13)
  {
    playSdWav13.play(filename);
    LeftEnvelopeBouton13.noteOn();
    RightEnvelopeBouton13.noteOn();
  }
  if (chord == 14)
  {
    playSdWav14.play(filename);
    LeftEnvelopeBouton14.noteOn();
    RightEnvelopeBouton14.noteOn();
  }
  if (chord == 15)
  {
    playSdWav15.play(filename);
    LeftEnvelopeBouton15.noteOn();
    RightEnvelopeBouton15.noteOn();
  }
  if (chord == 16)
  {
    playSdWav16.play(filename);
    LeftEnvelopeBouton16.noteOn();
    RightEnvelopeBouton16.noteOn();
  }
}

//----------------------------------------------------------------------------------//
/*                       Lecture Trame Module Interface                             */
//----------------------------------------------------------------------------------//

void LectureI2C(int i)
{


  if (i == 0x69 || i == 0xE1 || i == 0xE2 || i == 0xE3 || i == 0xE4 || i == 0xE5 || i == 0xE6 || i == 0xEE)
  {
    //Serial.println("Rien"); // print the character
  }
  else
  {
    // Pourcentage batterie
    if (i > 0xD0 && i < 0xDB)
    {
      DizainePourcentage = i - 208;
    }
    if (i >= 0xC0 && i < 0xCA)
    {
      UnitePourcentage = i - 192;
    }

    pourcentage = DizainePourcentage * 10 + UnitePourcentage;

    static unsigned long previousMillis = 0;
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= 5000 || Printbatterie == 1)
    {
      AffichageBatterie();
      Serial.println(pourcentage);
      previousMillis = currentMillis;
      Printbatterie = 0;
    }

    //Pad 1 Activé
    if (i == 0x01)
    {
      playFile(Fichier1, 1);     // filenames are always uppercase 8.3 format
      if (Etat == 1)
      {
        EtatBouton1 = !EtatBouton1;
      }

      if (Etat == 1)
      {
        if (EtatBouton1 == 1) {
          if (LayerSwitch == 1)
          {
            tft.fillRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_RED);
          }
          else
          {
            tft.fillRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_GREEN);
          }
          tft.drawRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
        else {
          tft.fillRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
          tft.drawRoundRect(80,  26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
      }
    }
    //Pad 2 Activé
    if (i == 0x02)
    {
      playFile(Fichier2, 2);
      if (Etat == 1)
      {
        EtatBouton2 = !EtatBouton2;
      }

      if (Etat == 1)
      {
        if (EtatBouton2 == 1) {
          if (LayerSwitch == 1)
          {
            tft.fillRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_RED);
          }
          else
          {
            tft.fillRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_GREEN);
          }
          tft.drawRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
        else {
          tft.fillRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
          tft.drawRoundRect(140, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
      }
    }
    //Pad 3 Activé
    if (i == 0x03)
    {
      playFile(Fichier3, 3);
      if (Etat == 1)
      {
        EtatBouton3 = !EtatBouton3;
      }

      if (Etat == 1)
      {
        if (EtatBouton3 == 1) {
          if (LayerSwitch == 1)
          {
            tft.fillRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_RED);
          }
          else
          {
            tft.fillRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_GREEN);
          }
          tft.drawRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
        else {
          tft.fillRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
          tft.drawRoundRect(200, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
      }
    }
    //Pad 4 Activé
    if (i == 0x04)
    {
      playFile(Fichier4, 4);
      if (Etat == 1)
      {
        EtatBouton4 = !EtatBouton4;
      }

      if (Etat == 1)
      {
        if (EtatBouton4 == 1) {
          if (LayerSwitch == 1)
          {
            tft.fillRoundRect(260, 26,  PADSIZE, PADSIZE, 10, ILI9341_RED);
          }
          else
          {
            tft.fillRoundRect(260, 26,  PADSIZE, PADSIZE, 10, ILI9341_GREEN);
          }
          tft.drawRoundRect(260, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
        else {
          tft.fillRoundRect(260, 26,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
          tft.drawRoundRect(260, 26,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
      }
    }
    //Pad 5 Activé
    if (i == 0x05)
    {
      playFile(Fichier5, 5);
      if (Etat == 1)
      {
        EtatBouton5 = !EtatBouton5;
      }

      if (Etat == 1)
      {
        if (EtatBouton5 == 1) {
          if (LayerSwitch == 1)
          {
            tft.fillRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_RED);
          }
          else
          {
            tft.fillRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_GREEN);
          }
          tft.drawRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
        else {
          tft.fillRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
          tft.drawRoundRect(80,  81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
      }
    }
    //Pad 6 Activé
    if (i == 0x06)
    {
      playFile(Fichier6, 6);
      if (Etat == 1)
      {
        EtatBouton6 = !EtatBouton6;
      }

      if (Etat == 1)
      {
        if (EtatBouton6 == 1) {
          if (LayerSwitch == 1)
          {
            tft.fillRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_RED);
          }
          else
          {
            tft.fillRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_GREEN);
          }
          tft.drawRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
        else {
          tft.fillRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
          tft.drawRoundRect(140, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
      }
    }
    //Pad 7 Activé
    if (i == 0x07)
    {
      playFile(Fichier7, 7);
      if (Etat == 1)
      {
        EtatBouton7 = !EtatBouton7;
      }

      if (Etat == 1)
      {
        if (EtatBouton7 == 1) {
          if (LayerSwitch == 1)
          {
            tft.fillRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_RED);
          }
          else
          {
            tft.fillRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_GREEN);
          }
          tft.drawRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
        else {
          tft.fillRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
          tft.drawRoundRect(200, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
      }
    }
    //Pad 8 Activé
    if (i == 0x08)
    {
      playFile(Fichier8, 8);
      if (Etat == 1)
      {
        EtatBouton8 = !EtatBouton8;
      }

      if (Etat == 1)
      {
        if (EtatBouton8 == 1) {
          if (LayerSwitch == 1)
          {
            tft.fillRoundRect(260, 81,  PADSIZE, PADSIZE, 10, ILI9341_RED);
          }
          else
          {
            tft.fillRoundRect(260, 81,  PADSIZE, PADSIZE, 10, ILI9341_GREEN);
          }
          tft.drawRoundRect(260, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
        else {
          tft.fillRoundRect(260, 81,  PADSIZE, PADSIZE, 10, ILI9341_BLACK);
          tft.drawRoundRect(260, 81,  PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
      }
    }
    //Pad 9 Activé
    if (i == 0x09)
    {
      playFile(Fichier9, 9);
      if (Etat == 1)
      {
        EtatBouton9 = !EtatBouton9;
      }

      if (Etat == 1)
      {
        if (EtatBouton9 == 1) {
          if (LayerSwitch == 1)
          {
            tft.fillRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_RED);
          }
          else
          {
            tft.fillRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_GREEN);
          }
          tft.drawRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
        else {
          tft.fillRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
          tft.drawRoundRect(80,  136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
      }
    }
    //Pad 10 Activé
    if (i == 0x10)
    {
      playFile(Fichier10, 10);
      if (Etat == 1)
      {
        EtatBouton10 = !EtatBouton10;
      }

      if (Etat == 1)
      {
        if (EtatBouton10 == 1) {
          if (LayerSwitch == 1)
          {
            tft.fillRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_RED);
          }
          else
          {
            tft.fillRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_GREEN);
          }
          tft.drawRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
        else {
          tft.fillRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
          tft.drawRoundRect(140, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
      }
    }
    //Pad 11 Activé
    if (i == 0x11)
    {
      playFile(Fichier11, 11);
      if (Etat == 1)
      {
        EtatBouton11 = !EtatBouton11;
      }
      if (Etat == 1)
      {
        if (EtatBouton11 == 1) {
          if (LayerSwitch == 1)
          {
            tft.fillRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_RED);
          }
          else
          {
            tft.fillRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_GREEN);
          }
          tft.drawRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
        else {
          tft.fillRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
          tft.drawRoundRect(200, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
      }
    }
    //Pad 12 Activé
    if (i == 0x12)
    {
      playFile(Fichier12, 12);
      if (Etat == 1)
      {
        EtatBouton12 = !EtatBouton12;
      }
      if (Etat == 1)
      {
        if (EtatBouton12 == 1) {
          if (LayerSwitch == 1)
          {
            tft.fillRoundRect(260, 136, PADSIZE, PADSIZE, 10, ILI9341_RED);
          }
          else
          {
            tft.fillRoundRect(260, 136, PADSIZE, PADSIZE, 10, ILI9341_GREEN );
          }
          tft.drawRoundRect(260, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
        else {
          tft.fillRoundRect(260, 136, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
          tft.drawRoundRect(260, 136, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
      }
    }
    //Pad 13 Activé
    if (i == 0x13)
    {
      playFile(Fichier13, 13);
      if (Etat == 1)
      {
        EtatBouton13 = !EtatBouton13;
      }
      if (Etat == 1)
      {
        if (EtatBouton13 == 1) {
          if (LayerSwitch == 1)
          {
            tft.fillRoundRect(80,  191, PADSIZE, PADSIZE, 10, ILI9341_RED);
          }
          else
          {
            tft.fillRoundRect(80,  191, PADSIZE, PADSIZE, 10, ILI9341_GREEN );
          }
          tft.drawRoundRect(80,  191, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
        else {
          tft.fillRoundRect(80,  191, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
          tft.drawRoundRect(80,  191, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
      }
    }
    //Pad 14 Activé
    if (i == 0x14)
    {
      playFile(Fichier14, 14);
      if (Etat == 1)
      {
        EtatBouton14 = !EtatBouton14;
      }

      if (Etat == 1)
      {
        if (EtatBouton14 == 1) {
          if (LayerSwitch == 1)
          {
            tft.fillRoundRect(140,  191, PADSIZE, PADSIZE, 10, ILI9341_RED);
          }
          else
          {
            tft.fillRoundRect(140,  191, PADSIZE, PADSIZE, 10, ILI9341_GREEN);
          }
          tft.drawRoundRect(140,  191, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
        else {
          tft.fillRoundRect(140,  191, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
          tft.drawRoundRect(140,  191, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
      }
    }
    //Pad 15 Activé
    if (i == 0x15)
    {
      playFile(Fichier15, 15);
      if (Etat == 1)
      {
        EtatBouton15 = !EtatBouton15;
      }

      if (Etat == 1)
      {
        if (EtatBouton15 == 1) {
          if (LayerSwitch == 1)
          {
            tft.fillRoundRect(200,  191, PADSIZE, PADSIZE, 10, ILI9341_RED);
          }
          else
          {
            tft.fillRoundRect(200,  191, PADSIZE, PADSIZE, 10, ILI9341_GREEN);
          }
          tft.drawRoundRect(200,  191, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
        else {
          tft.fillRoundRect(200,  191, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
          tft.drawRoundRect(200,  191, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
      }
    }
    //Pad 16 Activé
    if (i == 0x16)
    {
      playFile(Fichier16, 16);
      if (Etat == 1)
      {
        EtatBouton16 = !EtatBouton16;
      }

      if (Etat == 1)
      {
        if (EtatBouton16 == 1) {
          if (LayerSwitch == 1)
          {
            tft.fillRoundRect(260,  191, PADSIZE, PADSIZE, 10, ILI9341_RED);
          }
          else
          {
            tft.fillRoundRect(260,  191, PADSIZE, PADSIZE, 10, ILI9341_GREEN);
          }
          tft.drawRoundRect(260,  191, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
        else {
          tft.fillRoundRect(260,  191, PADSIZE, PADSIZE, 10, ILI9341_BLACK);
          tft.drawRoundRect(260,  191, PADSIZE, PADSIZE, 10, ILI9341_WHITE);
        }
      }
    }


    //Encodeur 5 = Home
    switch (Etat) {

      case 1:
        if (i == 0x5e)
        {
          Etat = 2;
          ImagePrint();
          VolumePrint();
          MixerPrint(4);
          AffichageBatterie();
        }
        else if (i == 0x5f)
        {
          Etat = 3;
          ImagePrint();
          VolumePrint();
          AffichageBatterie();
        }
        break;

      case 2:
        if (i == 0x5e)
        {
          Etat = 3;
          ImagePrint();
          VolumePrint();
          AffichageBatterie();
        }
        else if (i == 0x5f)
        {
          Etat = 1;
          ImagePrint();
          VolumePrint();
          AffichageBatterie();
        }

        //Encodeur 1 = Mixer1 [Reverb]
        if (i == 0x1e)
        {
          CalculMixersEncodeur(1, '-');
        }
        else if (i == 0x1f)
        {
          CalculMixersEncodeur(1, '+');
        }

        //Encodeur 2 = Mixer2
        if (i == 0x2e)
        {
          CalculMixersEncodeur(2, '+');
        }
        else if (i == 0x2f)
        {
          CalculMixersEncodeur(2, '-');
        }

        //Encodeur 3 = Mixer3
        if (i == 0x3e)
        {
          CalculMixersEncodeur(3, '+');
        }
        else if (i == 0x3f)
        {
          CalculMixersEncodeur(3, '-');
        }

        //Encodeur 4 = Mixer4
        // +
        if (i == 0x4e)
        {
          CalculMixersEncodeur(4, '+');
        }
        // -
        else if (i == 0x4f)
        {
          CalculMixersEncodeur(4, '-');
        }
        break;

      case 3:
        if (i == 0x5e)
        {
          Etat = 1;
          ImagePrint();
          VolumePrint();
          AffichageBatterie();
        }
        else if (i == 0x5f)
        {
          Etat = 2;
          ImagePrint();
          VolumePrint();
          MixerPrint(4);
          AffichageBatterie();
        }
        else if (i == 0xb2)
        {
          Etat = 4;
          Action = 1;
          ImagePrint();
          VolumePrint();
          AffichageBatterie();
        }
        break;

      case 4:
        if (i == 0x5e)
        {
          Action = Action + 1;
          if (Action > 4) {
            Action = 1;
          }
          ImagePrint();
          VolumePrint();
          AffichageBatterie();
        }
        else if (i == 0x5f)
        {
          Action = Action - 1;
          if (Action < 1) {
            Action = 4;
          }
          ImagePrint(); 
          VolumePrint(); 
          AffichageBatterie();
        }
        else if (i == 0xb2)
        {
          switch (Action) {
            case 1:
              Etat = 5;
              ImagePrint();
              VolumePrint();
              MixerPrint(4);
              AffichageBatterie();
              break;
            case 2:
              Etat = 6;
              ImagePrint();
              VolumePrint();
              MixerPrint(3);
              AffichageBatterie();
              break;
            case 3:
              Etat = 7;
              ImagePrint();
              VolumePrint();
              MixerPrint(5);
              AffichageBatterie();
              break;
            case 4:
              Etat = 8;
              Attack = 10.5, Holds = 2.5, Decay = 35, Sustains = 0.5, PixelAttack = 81, PixelHolds = 81, PixelDecay = 81 , PixelSustains = 107;
              ImagePrint();
              VolumePrint();
              MixerPrint(2);
              AffichageBatterie();
              break;
          }
        }
        break;

      case 5:
        // Roomsize (Reverb)
        if (i == 0x1e) // +
        {
          NombreRoomsize = float(NombreRoomsize + 0.1);
          if (NombreRoomsize > 0.99)
          {
            NombreRoomsize = 0.99;
          }
          Roomsize = -114 * NombreRoomsize + 140;

          freeverbs1.roomsize(NombreRoomsize);
          freeverbs2.roomsize(NombreRoomsize);

          MixerPrint(1);
        }
        else if (i == 0x1f) // -
        {
          NombreRoomsize = float(NombreRoomsize - 0.1);
          if (NombreRoomsize < 0.00)
          {
            NombreRoomsize = 0.00;
          }
          Roomsize = -114 * NombreRoomsize + 140;

          freeverbs1.roomsize(NombreRoomsize);
          freeverbs2.roomsize(NombreRoomsize);

          MixerPrint(1);
        }

        // Damping (Reverb)
        else if (i == 0x2e) // +
        {
          NombreDamping = float(NombreDamping + 0.1);
          if (NombreDamping > 0.99)
          {
            NombreDamping = 0.99;
          }
          Damping = -114 * NombreDamping + 140;

          freeverbs1.damping(NombreDamping);
          freeverbs2.damping(NombreDamping);

          MixerPrint(2);
        }
        else if (i == 0x2f) // -
        {
          NombreDamping = float(NombreDamping - 0.1);
          if (NombreDamping < 0)
          {
            NombreDamping = 0;
          }
          Damping = -114 * NombreDamping + 140;

          freeverbs1.damping(NombreDamping);
          freeverbs2.damping(NombreDamping);

          MixerPrint(2);
        }

        // Mixer1 (Reverb)
        else if (i == 0x3e) // +
        {
          CalculMixersEncodeur(1, '+');
        }
        else if (i == 0x3f) // -
        {
          CalculMixersEncodeur(1, '-');
        }
        // Back (Chorus)
        else if (i == 0xb2) // +
        {
          Etat = 4;
          ImagePrint();
          VolumePrint();
          MixerPrint(0);
          AffichageBatterie();
        }
        break;

      case 6:
        // Voix (Reverb)
        if (i == 0x1e) // +
        {
          NombreVoix = NombreVoix + 1;
          if (NombreVoix > 10)
          {
            NombreVoix = 10;
          }
          Voix = float(-11.4 * NombreVoix + 140);

          //Serial.print(NombreVoix);
          chorus1.voices(uint16_t(NombreVoix));
          chorus2.voices(uint16_t(NombreVoix));

          MixerPrint(1);
        }
        else if (i == 0x1f) // -
        {
          NombreVoix = NombreVoix - 1;
          if (NombreVoix < 0)
          {
            NombreVoix = 0;
          }
          Voix = float(-11.4 * NombreVoix + 140);

          chorus1.voices(uint16_t(NombreVoix));
          chorus2.voices(uint16_t(NombreVoix));

          MixerPrint(1);
        }

        // Mixer2 (Chorus)
        else if (i == 0x2e) // +
        {
          CalculMixersEncodeur(2, '+');
        }
        else if (i == 0x2f) // -
        {
          CalculMixersEncodeur(2, '-');
        }

        // Back (Chorus)
        else if (i == 0xb2) // +
        {
          Etat = 4;
          ImagePrint();
          VolumePrint();
          MixerPrint(0);
          AffichageBatterie();
        }
        break;

      case 7:
        // Offset (Flange)
        if (i == 0x1e) // +
        {
          NombreOffset = NombreOffset + 1;
          if (NombreOffset > 3)
          {
            NombreOffset = 3;
          }
          Offset = -38 * NombreOffset + 140;

          int s_idx = NombreOffset * FLANGE_DELAY_LENGTH / 4;
          flange1.voices(s_idx, s_depth, s_freq);
          flange2.voices(s_idx, s_depth, s_freq);

          MixerPrint(1);
        }
        else if (i == 0x1f) // -
        {
          NombreOffset = NombreOffset - 1;
          if (NombreOffset < 0)
          {
            NombreOffset = 0;
          }
          Offset = -38 * NombreOffset + 140;

          int s_idx = NombreOffset * FLANGE_DELAY_LENGTH / 4;
          flange1.voices(s_idx, s_depth, s_freq);
          flange2.voices(s_idx, s_depth, s_freq);

          MixerPrint(1);
        }

        // Depth (Flange)
        else if (i == 0x2e) // +
        {
          NombreDepth = NombreDepth + 4;
          if (NombreDepth > 8)
          {
            NombreDepth = 8;
          }
          Depth = 54;

          int s_depth = FLANGE_DELAY_LENGTH / NombreDepth;
          flange1.voices(s_idx, s_depth, s_freq);
          flange2.voices(s_idx, s_depth, s_freq);

          MixerPrint(2);
        }
        else if (i == 0x2f) // -
        {
          NombreDepth = NombreDepth - 4;
          if (NombreDepth < 4)
          {
            NombreDepth = 4;
          }
          Depth = 106;

          int s_depth = FLANGE_DELAY_LENGTH / NombreDepth;
          flange1.voices(s_idx, s_depth, s_freq);
          flange2.voices(s_idx, s_depth, s_freq);

          MixerPrint(2);
        }

        // DelayRate (Flange)
        else if (i == 0x3e) // +
        {
          NombreDelayRate = NombreDelayRate + 1;
          if (NombreDelayRate > 10)
          {
            NombreDelayRate = 10;
          }
          DelayRate = -11.4 * NombreDelayRate + 140;

          double s_freq = NombreDelayRate;
          flange1.voices(s_idx, s_depth, s_freq);
          flange2.voices(s_idx, s_depth, s_freq);


          MixerPrint(3);
        }
        else if (i == 0x3f) // -
        {
          NombreDelayRate = NombreDelayRate - 1;
          if (NombreDelayRate < 0)
          {
            NombreDelayRate = 0;
          }
          DelayRate = -11.4 * NombreDelayRate + 140;

          double s_freq = NombreDelayRate;
          flange1.voices(s_idx, s_depth, s_freq);
          flange2.voices(s_idx, s_depth, s_freq);


          MixerPrint(3);
        }

        // Mixer3 (Flange)
        else if (i == 0x4e) // +
        {
          CalculMixersEncodeur(3, '+');
        }
        else if (i == 0x4f) // -
        {
          CalculMixersEncodeur(3, '-');
        }
        // Back (Chorus)
        else if (i == 0xb2) // +
        {
          Etat = 4;
          ImagePrint();
          VolumePrint();
          MixerPrint(0);
          AffichageBatterie();
        }
        break;

      case 8: 

        // Envelope
        if ((i == 0x1e) || (i == 0x1f) || (i == 0x2e) || (i == 0x2f) || (i == 0x3e) || (i == 0x3f) || (i == 0x4e) || (i == 0x4f))
        {
          // Attack (Envelope)
          if (i == 0x1e) // +
          {
            Attack = Attack + 100.0;
            if (Attack > 11800.0)
            {
              Attack = 11800.0;
            }
            //Attack = 0.0; // ***
          }
          else if (i == 0x1f) // -
          {
            Attack = Attack - 100.0;
            if (Attack < 0.0)
            {
              Attack = 0.0;
            }
          }

          // Hold (Envelope)
          if (i == 0x2e) // +
          {
            Holds = Holds + 100.0;
            if (Holds > 11800.0)
            {
              Holds = 11800.0;
            }

            //Holds = 11800.0; // ***
          }
          else if (i == 0x2f) // -
          {
            Holds = Holds - 100.0;
            if (Holds < 0.0)
            {
              Holds = 0.0;
            }
          }

          // Decay (Envelope)
          if (i == 0x3e) // +
          {
            Decay = Decay + 100.0;
            if (Decay > 11800.0)
            {
              Decay = 11800.0;
            }

            //Decay = 0.0; // ***
          }
          else if (i == 0x3f) // -
          {
            Decay = Decay - 100.0;
            if (Decay < 0.0)
            {
              Decay = 0.0;
            }
          }

          // Sustain (Envelope) //
          if (i == 0x4e) // +
          {
            Sustains = Sustains + 0.1;
            if (Sustains > 1.0)
            {
              Sustains = 1.0;
            }

            //Sustains = 0.0; //***
          }
          else if (i == 0x4f) // -
          {
            Sustains = Sustains - 0.1;
            if (Sustains < 0.0)
            {
              Sustains = 0.0;
            }
          }
          MixerPrint(1); //Clear Ancient
          PixelAttack = 81 + (0.00474576271186441 * Attack);
          PixelHolds = PixelAttack + (0.00474576271186441 * Holds);
          PixelDecay = PixelHolds + (0.00474576271186441 * Decay);
          PixelSustains = (Sustains * -134 + 174);
          MixerPrint(2); //Afficher Nouveau
        }

        // Association des parametres des envelopes
        if (EtatBouton1 == 1) {
          AudioNoInterrupts();
          LeftEnvelopeBouton1.attack(Attack), LeftEnvelopeBouton1.hold(Holds), LeftEnvelopeBouton1.decay(Decay), LeftEnvelopeBouton1.sustain(Sustains);
          RightEnvelopeBouton1.attack(Attack), RightEnvelopeBouton1.hold(Holds), RightEnvelopeBouton1.decay(Decay), RightEnvelopeBouton1.sustain(Sustains);
          AudioInterrupts();
        }
        if (EtatBouton2 == 1) {
          AudioNoInterrupts();
          LeftEnvelopeBouton2.attack(Attack), LeftEnvelopeBouton2.hold(Holds), LeftEnvelopeBouton2.decay(Decay), LeftEnvelopeBouton2.sustain(Sustains);
          RightEnvelopeBouton2.attack(Attack), RightEnvelopeBouton2.hold(Holds), RightEnvelopeBouton2.decay(Decay), RightEnvelopeBouton2.sustain(Sustains);
          AudioInterrupts();
        }
        if (EtatBouton3 == 1) {
          AudioNoInterrupts();
          LeftEnvelopeBouton3.attack(Attack), LeftEnvelopeBouton3.hold(Holds), LeftEnvelopeBouton3.decay(Decay), LeftEnvelopeBouton3.sustain(Sustains);
          RightEnvelopeBouton3.attack(Attack), RightEnvelopeBouton3.hold(Holds), RightEnvelopeBouton3.decay(Decay), RightEnvelopeBouton3.sustain(Sustains);
          AudioInterrupts();
        }
        if (EtatBouton4 == 1) {
          AudioNoInterrupts();
          LeftEnvelopeBouton4.attack(Attack), LeftEnvelopeBouton4.hold(Holds), LeftEnvelopeBouton4.decay(Decay), LeftEnvelopeBouton4.sustain(Sustains);
          RightEnvelopeBouton4.attack(Attack), RightEnvelopeBouton4.hold(Holds), RightEnvelopeBouton4.decay(Decay), RightEnvelopeBouton4.sustain(Sustains);
          AudioInterrupts();
        }
        if (EtatBouton5 == 1) {
          AudioNoInterrupts();
          LeftEnvelopeBouton5.attack(Attack), LeftEnvelopeBouton5.hold(Holds), LeftEnvelopeBouton5.decay(Decay), LeftEnvelopeBouton5.sustain(Sustains);
          RightEnvelopeBouton5.attack(Attack), RightEnvelopeBouton5.hold(Holds), RightEnvelopeBouton5.decay(Decay), RightEnvelopeBouton5.sustain(Sustains);
          AudioInterrupts();
        }
        if (EtatBouton6 == 1) {
          AudioNoInterrupts();
          LeftEnvelopeBouton6.attack(Attack), LeftEnvelopeBouton6.hold(Holds), LeftEnvelopeBouton6.decay(Decay), LeftEnvelopeBouton6.sustain(Sustains);
          RightEnvelopeBouton6.attack(Attack), RightEnvelopeBouton6.hold(Holds), RightEnvelopeBouton6.decay(Decay), RightEnvelopeBouton6.sustain(Sustains);
          AudioInterrupts();
        }
        if (EtatBouton7 == 1) {
          AudioNoInterrupts();
          LeftEnvelopeBouton7.attack(Attack), LeftEnvelopeBouton7.hold(Holds), LeftEnvelopeBouton7.decay(Decay), LeftEnvelopeBouton7.sustain(Sustains);
          RightEnvelopeBouton7.attack(Attack), RightEnvelopeBouton7.hold(Holds), RightEnvelopeBouton7.decay(Decay), RightEnvelopeBouton7.sustain(Sustains);
          AudioInterrupts();
        }
        if (EtatBouton8 == 1) {
          AudioNoInterrupts();
          LeftEnvelopeBouton8.attack(Attack), LeftEnvelopeBouton8.hold(Holds), LeftEnvelopeBouton8.decay(Decay), LeftEnvelopeBouton8.sustain(Sustains);
          RightEnvelopeBouton8.attack(Attack), RightEnvelopeBouton8.hold(Holds), RightEnvelopeBouton8.decay(Decay), RightEnvelopeBouton8.sustain(Sustains);
          AudioInterrupts();
        }
        if (EtatBouton9 == 1) {
          AudioNoInterrupts();
          LeftEnvelopeBouton9.attack(Attack), LeftEnvelopeBouton9.hold(Holds), LeftEnvelopeBouton9.decay(Decay), LeftEnvelopeBouton9.sustain(Sustains);
          RightEnvelopeBouton9.attack(Attack), RightEnvelopeBouton9.hold(Holds), RightEnvelopeBouton9.decay(Decay), RightEnvelopeBouton9.sustain(Sustains);
          AudioInterrupts();
        }
        if (EtatBouton10 == 1) {
          AudioNoInterrupts();
          LeftEnvelopeBouton10.attack(Attack), LeftEnvelopeBouton10.hold(Holds), LeftEnvelopeBouton10.decay(Decay), LeftEnvelopeBouton10.sustain(Sustains);
          RightEnvelopeBouton10.attack(Attack), RightEnvelopeBouton10.hold(Holds), RightEnvelopeBouton10.decay(Decay), RightEnvelopeBouton10.sustain(Sustains);
          AudioInterrupts();
        }
        if (EtatBouton11 == 1) {
          AudioNoInterrupts();
          LeftEnvelopeBouton11.attack(Attack), LeftEnvelopeBouton11.hold(Holds), LeftEnvelopeBouton11.decay(Decay), LeftEnvelopeBouton11.sustain(Sustains);
          RightEnvelopeBouton11.attack(Attack), RightEnvelopeBouton11.hold(Holds), RightEnvelopeBouton11.decay(Decay), RightEnvelopeBouton11.sustain(Sustains);
          AudioInterrupts();
        }
        if (EtatBouton12 == 1) {
          AudioNoInterrupts();
          LeftEnvelopeBouton12.attack(Attack), LeftEnvelopeBouton12.hold(Holds), LeftEnvelopeBouton12.decay(Decay), LeftEnvelopeBouton12.sustain(Sustains);
          RightEnvelopeBouton12.attack(Attack), RightEnvelopeBouton12.hold(Holds), RightEnvelopeBouton12.decay(Decay), RightEnvelopeBouton12.sustain(Sustains);
          AudioInterrupts();
        }
        if (EtatBouton13 == 1) {
          AudioNoInterrupts();
          LeftEnvelopeBouton13.attack(Attack), LeftEnvelopeBouton13.hold(Holds), LeftEnvelopeBouton13.decay(Decay), LeftEnvelopeBouton13.sustain(Sustains);
          RightEnvelopeBouton13.attack(Attack), RightEnvelopeBouton13.hold(Holds), RightEnvelopeBouton13.decay(Decay), RightEnvelopeBouton13.sustain(Sustains);
          AudioInterrupts();
        }
        if (EtatBouton14 == 1) {
          AudioNoInterrupts();
          LeftEnvelopeBouton14.attack(Attack), LeftEnvelopeBouton14.hold(Holds), LeftEnvelopeBouton14.decay(Decay), LeftEnvelopeBouton14.sustain(Sustains);
          RightEnvelopeBouton14.attack(Attack), RightEnvelopeBouton14.hold(Holds), RightEnvelopeBouton14.decay(Decay), RightEnvelopeBouton14.sustain(Sustains);
          AudioInterrupts();
        }
        if (EtatBouton15 == 1) {
          AudioNoInterrupts();
          LeftEnvelopeBouton15.attack(Attack), LeftEnvelopeBouton15.hold(Holds), LeftEnvelopeBouton15.decay(Decay), LeftEnvelopeBouton15.sustain(Sustains);
          RightEnvelopeBouton15.attack(Attack), RightEnvelopeBouton15.hold(Holds), RightEnvelopeBouton15.decay(Decay), RightEnvelopeBouton15.sustain(Sustains);
          AudioInterrupts();
        }
        if (EtatBouton16 == 1) {
          AudioNoInterrupts();
          LeftEnvelopeBouton16.attack(Attack), LeftEnvelopeBouton16.hold(Holds), LeftEnvelopeBouton16.decay(Decay), LeftEnvelopeBouton16.sustain(Sustains);
          RightEnvelopeBouton16.attack(Attack), RightEnvelopeBouton16.hold(Holds), RightEnvelopeBouton16.decay(Decay), RightEnvelopeBouton16.sustain(Sustains);
          AudioInterrupts();
        }

        // Back (Envelope)
        if (i == 0xb2) // +
        {
          Etat = 4;
          ImagePrint();
          VolumePrint();
          MixerPrint(0);
          AffichageBatterie();
        }
        break;
    }

    //Encodeur 6 = Volume
    if (i == 0x6e)
    {
      PourcentageVolume = PourcentageVolume + 1;
      if (PourcentageVolume > 100)
      {
        PourcentageVolume = 100;
      }

      int v = float((PourcentageVolume * m) + 16);
      Slide = float(1.9 * PourcentageVolume);
      setvolume(v);
      //Serial.println(PourcentageVolume);
      //Serial.println(Slide);
      VolumePrint();
    }
    else if (i == 0x6f)
    {
      PourcentageVolume = PourcentageVolume - 1;
      if (PourcentageVolume < 0)
      {
        PourcentageVolume = 0;
      }

      int v = float((PourcentageVolume * m) + 16);
      Slide = float(1.9 * PourcentageVolume);
      setvolume(v);
      //Serial.println(PourcentageVolume);
      //Serial.println(Slide);
      VolumePrint();
    }
    else if (i == 0xb1)
    {
      if (playSdWav1.isPlaying())
      {
        playSdWav1.stop();
      }
      if (playSdWav2.isPlaying())
      {
        playSdWav2.stop();
      }
      if (playSdWav3.isPlaying())
      {
        playSdWav3.stop();
      }
      if (playSdWav4.isPlaying())
      {
        playSdWav4.stop();
      }
      if (playSdWav5.isPlaying())
      {
        playSdWav5.stop();
      }
      if (playSdWav6.isPlaying())
      {
        playSdWav6.stop();
      }
      if (playSdWav7.isPlaying())
      {
        playSdWav7.stop();
      }
      if (playSdWav8.isPlaying())
      {
        playSdWav8.stop();
      }
      if (playSdWav9.isPlaying())
      {
        playSdWav9.stop();
      }
      if (playSdWav10.isPlaying())
      {
        playSdWav10.stop();
      }
      if (playSdWav11.isPlaying())
      {
        playSdWav11.stop();
      }
      if (playSdWav12.isPlaying())
      {
        playSdWav12.stop();
      }
      if (playSdWav13.isPlaying())
      {
        playSdWav13.stop();
      }
      if (playSdWav14.isPlaying())
      {
        playSdWav14.stop();
      }
      if (playSdWav15.isPlaying())
      {
        playSdWav15.stop();
      }
      if (playSdWav16.isPlaying())
      {
        playSdWav16.stop();
      }
    }

    if ((i == 0xA2) && (VA1 == 0))
    {
      VA1 = 1;
      VA2 = 0;
      ChangementLayer(1);
      Serial.printf("%x\n\r", i);
    }
    else if (i == 0xA1 && (VA2 == 0))
    {
      VA2 = 1;
      VA1 = 0;
      ChangementLayer(2);
      Serial.printf("%x\n\r", i);
    }
    if ((i != 0xA1) && (i != 0xA2) && ((i > 0xD0) && (i < 0xDB)) && ((i > 0xC0) && (i < 0xCB))) {
      Serial.printf("%x\n\r", i);
    }
  }

  //delay(5);
}

//----------------------------------------------------------------------------------//
/*                      Fonction d'Implémentation du Reverb                         */
//----------------------------------------------------------------------------------//

void SetEffects()
{
  if ((PourcentageMixer3 + PourcentageMixer2 + PourcentageMixer1) == 0)
  {
    //Reverb
    LmixerReverb.gain(0, 0.0);
    LmixerReverb.gain(1, 0.0);
    Leffectmixer1.gain(0, 0.0);
    RmixerReverb.gain(0, 0.0);
    RmixerReverb.gain(1, 0.0);
    Reffectmixer1.gain(0, 0.0);
    //Chorus
    LmixerChorus.gain(0, 0.0);
    LmixerChorus.gain(1, 0.0);
    Leffectmixer1.gain(1, 0.0);
    RmixerChorus.gain(0, 0.0);
    RmixerChorus.gain(1, 0.0);
    Reffectmixer1.gain(1, 0.0);
    //Flange
    LmixerFlange.gain(0, 0.0);
    LmixerFlange.gain(1, 0.0);
    Leffectmixer1.gain(2, 0.0);
    RmixerFlange.gain(0, 0.0);
    RmixerFlange.gain(1, 0.0);
    Reffectmixer1.gain(2, 0.0);
    //Raw
    Leffectmixer1.gain(3, MixerVolumeMax);
    Reffectmixer1.gain(3, MixerVolumeMax);
  }
  else
  {
    //Raw
    Leffectmixer1.gain(3, 0.0);
    Reffectmixer1.gain(3, 0.0);
    //Reverb
    b = float(MixerVolumeMax * PourcentageMixer1 / 100); //0.5
    a = float(MixerVolumeMax - b); //0.0
    c = float(MixerVolumeMax * PourcentageMixer1 / (PourcentageMixer1 + PourcentageMixer2 + PourcentageMixer3)); //1
    LmixerReverb.gain(0, a);
    LmixerReverb.gain(1, b);
    Leffectmixer1.gain(0, c);
    RmixerReverb.gain(0, a);
    RmixerReverb.gain(1, b);
    Reffectmixer1.gain(0, c);
    //Chorus
    /*Serial.println(PourcentageMixer1);
      Serial.println(PourcentageMixer2);
      Serial.println(PourcentageMixer3);*/
    b = float(MixerVolumeMax * PourcentageMixer2 / 100); //0.2
    a = float(MixerVolumeMax - b); //0.3
    c = float(MixerVolumeMax * PourcentageMixer2 / (PourcentageMixer1 + PourcentageMixer2 + PourcentageMixer3)); //0.5
    //Serial.print("a: "); Serial.print(a); Serial.print("b: "); Serial.print(b); Serial.print("c: "); Serial.println(c);
    LmixerChorus.gain(0, a);
    LmixerChorus.gain(1, b);
    Leffectmixer1.gain(1, c);
    RmixerChorus.gain(0, a);
    RmixerChorus.gain(1, b);
    Reffectmixer1.gain(1, c);
    //Flange
    b = float(MixerVolumeMax * PourcentageMixer3 / 100); //0.42
    a = float(MixerVolumeMax - b); //0.58
    c = float(MixerVolumeMax * PourcentageMixer3 / (PourcentageMixer1 + PourcentageMixer2 + PourcentageMixer3)); //1
    LmixerFlange.gain(0, a);
    LmixerFlange.gain(1, b);
    Leffectmixer1.gain(2, c);
    RmixerFlange.gain(0, a);
    RmixerFlange.gain(1, b);
    Reffectmixer1.gain(2, c);
  }
}

//----------------------------------------------------------------------------------//
/*                    Fonction Calcul Encodeur (0 a 100%)                           */
//----------------------------------------------------------------------------------//

void CalculMixersEncodeur(int Encodeur, char Mouvement)
{
  switch (Encodeur)
  {
    case 1:
      if (Mouvement == '+')
      {
        PourcentageMixer1 = PourcentageMixer1 + 1;
        if (PourcentageMixer1 > 99)
        {
          PourcentageMixer1 = 99;
        }
        SetEffects();
        Mixer1 = float(-1.14 * PourcentageMixer1 + 140.0);
        //Serial.print(PourcentageMixer1); Serial.println("%");
        //Serial.println(Mixer1);
      }
      else
      {
        PourcentageMixer1 = PourcentageMixer1 - 1;
        if (PourcentageMixer1 < 0)
        {
          PourcentageMixer1 = 0;
        }
        SetEffects();
        Mixer1 = float(-1.14 * PourcentageMixer1 + 140.0);
        //Serial.print(PourcentageMixer1); Serial.println("%");
        //Serial.println(Mixer1);
      }
      MixerPrint(1);
      break;
    case 2:
      if (Mouvement == '+')
      {
        PourcentageMixer2 = PourcentageMixer2 + 1;
        if (PourcentageMixer2 > 99)
        {
          PourcentageMixer2 = 99;
        }
        SetEffects();
        Mixer2 = float(-1.14 * PourcentageMixer2 + 140.0);
        //Serial.print(PourcentageMixer2); Serial.println("%");
        //Serial.println(Mixer2);
      }
      else
      {

        PourcentageMixer2 = PourcentageMixer2 - 1;
        if (PourcentageMixer2 < 0)
        {
          PourcentageMixer2 = 0;
        }
        SetEffects();
        Mixer2 = float(-1.14 * PourcentageMixer2 + 140.0);
        //Serial.print(PourcentageMixer2); Serial.println("%");
        //Serial.println(Mixer2);
      }
      MixerPrint(2);
      break;

    case 3:
      if (Mouvement == '+')
      {
        PourcentageMixer3 = PourcentageMixer3 + 1;
        if (PourcentageMixer3 > 99)
        {
          PourcentageMixer3 = 99;
        }
        SetEffects();
        Mixer3 = float(-1.14 * PourcentageMixer3 + 140.0);
        //Serial.print(PourcentageMixer3); Serial.println("%");
        //Serial.println(Mixer3);
      }
      else
      {
        PourcentageMixer3 = PourcentageMixer3 - 1;
        if (PourcentageMixer3 < 0)
        {
          PourcentageMixer3 = 0;
        }
        SetEffects();
        Mixer3 = float(-1.14 * PourcentageMixer3 + 140.0);
        //Serial.print(PourcentageMixer3); Serial.println("%");
        //Serial.println(Mixer3);
      }
      MixerPrint(3);
      break;
  }
}

//----------------------------------------------------------------------------------//
/*                          Fonction Changement de layer                            */
//----------------------------------------------------------------------------------//

void  ChangementLayer(int layer)
{
  if (layer == 1)
  {
    LayerSwitch = 1;
    Fichier1 = "Bouton1Layer1.wav";
    Fichier2 = "Bouton2Layer1.wav";
    Fichier3 = "Bouton3Layer1.wav";
    Fichier4 = "Bouton4Layer1.wav";
    Fichier5 = "Bouton5Layer1.wav";
    Fichier6 = "Bouton6Layer1.wav";
    Fichier7 = "Bouton7Layer1.wav";
    Fichier8 = "Bouton8Layer1.wav";
    Fichier9 = "Bouton9Layer1.wav";
    Fichier10 = "Bouton10Layer1.wav";
    Fichier11 = "Bouton11Layer1.wav";
    Fichier12 = "Bouton12Layer1.wav";
    Fichier13 = "Bouton13Layer1.wav";
    Fichier14 = "Bouton14Layer1.wav";
    Fichier15 = "Bouton15Layer1.wav";
    Fichier16 = "Bouton16Layer1.wav";

    if (Etat == 1)
    {
      ReprintPads();
    }
  }
  else if (layer == 2)
  {
    LayerSwitch = 2;
    Fichier1 = "Bouton1Layer2.wav";
    Fichier2 = "Bouton2Layer2.wav";
    Fichier3 = "Bouton3Layer2.wav";
    Fichier4 = "Bouton4Layer2.wav";
    Fichier5 = "Bouton5Layer2.wav";
    Fichier6 = "Bouton6Layer2.wav";
    Fichier7 = "Bouton7Layer2.wav";
    Fichier8 = "Bouton8Layer2.wav";
    Fichier9 = "Bouton9Layer2.wav";
    Fichier10 = "Bouton10Layer2.wav";
    Fichier11 = "Bouton11Layer2.wav";
    Fichier12 = "Bouton12Layer2.wav";
    Fichier13 = "Bouton13Layer2.wav";
    Fichier14 = "Bouton14Layer2.wav";
    Fichier15 = "Bouton15Layer2.wav";
    Fichier16 = "TURBO.wav";

    if (Etat == 1)
    {
      ReprintPads();
    }
  }
}
