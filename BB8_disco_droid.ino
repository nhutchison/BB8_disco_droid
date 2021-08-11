// Updated by TheJugg1er July 2021
// Fixed bug with the Name not being updated
// Fixed issues with the iPhone APP sending commands that were not recognised
// Reconfigured the LED definitions to match the 3 per element config
// Updated the debug output to be clearer and to prevent flooding the serial monitor




//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!IMPORTANT EDIT THE VALUE for String MAC!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
String myNAME = "BB-80";//REPLACE the value inside the "" to your username, this will help you locate your Disco-Droid in a crowd
/////////////////////////////////////////////////////DO NOT EDIT BELOW THIS LINE//////////////////////////////////////////////////////
//Released under CC4 SA NC BY Jim Yu
//Libraries
#include "./Tlc5940.h"
#include <SoftwareSerial.h>
#include "./DFRobotDFPlayerMini.h"

//Pin Definitions
SoftwareSerial Sound(4, 5); // RX, TX
DFRobotDFPlayerMini MP3;
const int speakerIn = 7;
const int mp3In = 2;

//Global Variables

int volumeOut = 5;
int BT = 0;
int FTL = 4;
int FBL = 0;
int SL  = 4;
int RE = 4;
int HP = 1;
int PSI = 1;
int Play = 1;


int minVol = 2000;
int maxVol = 0;

class LED///////////////////////////////////////////START LED CLASS///////////////////////////////////////////////////
{
    // Class Local Variables
    int first;          // channel of the first LED in the array
    int last;           // channel of the last LED in the array
    long SWEEPinterval;      // milliseconds interval sweeper
    long FADEinterval;      // milliseconds interval fader
    long RANDinterval;      // milliseconds interval randomizer

    // These maintain the current state
    int dir;                    // for tracking sweep direction
    int current;                // for tracking LED switching
    int dimmer;                 // for tracking brightness
    int dim;
    unsigned long lastMillis;   // will store last time LED was updated

  public:
    LED(int firstLED, int lastLED, long SWEEPintervalTime, long FADEintervalTime, long RANDintervalTime)//Constructor
    {
      first = firstLED;
      last = lastLED;
      SWEEPinterval = SWEEPintervalTime;
      FADEinterval = FADEintervalTime;
      RANDinterval = RANDintervalTime;
      dir = 1;
      dimmer = 1;
      dim = 1;
      current = firstLED;
      lastMillis = 0;
    }
    void Update(int mode)//Update Function
    {
      unsigned long nowMillis = millis();//get current time
      switch (mode)
      {
        case 0://Off
          {
            if (nowMillis >= (lastMillis + 100))//Check if it is time to update
              //Serial.println("LED Off");
            {
              for (int channel = first; channel <= last; channel ++)
              {
                Tlc.set(channel, 0);
              }
            }
            break;
          }
        case 1://Sweeper
          {
            if (nowMillis >= (lastMillis + SWEEPinterval))//Check if it is time to update
            {
              //Serial.println("LED Sweep");
              lastMillis = nowMillis; //Save new time
              for (int channel = first; channel <= last; channel ++)
              {
                Tlc.set(channel, 0);
              }
              if (current <= last) //If current LED is not beyond the last
              {
                if (current == first) //and If currrent LED is the first
                {
                  dir = 1; //Set direction to forward
                }
                else //If Current LED is not the first LED and not the last LED
                {
                  //Tlc.set(current - 1, 1000); //Set the LED before to 25% brightness
                }
                Tlc.set(current, 4095); //Set current LED to maximum brightness
                if (current == last) //If current LED is the last
                {
                  dir = -1; //Set direction to reverse
                }
                current += dir;
              }
              if (current < first || current > last)
              {
                current = first;
              }
            }
            break;
          }
        case 2://Fader
          {
            if (nowMillis >= (lastMillis + FADEinterval))//Check if it is time to update
            {
              //Serial.println("LED Fader");
              lastMillis = nowMillis;//Save new time
              for (int channel = first; channel <= last; channel++)
              {
                Tlc.set(channel, 0);
              }
              current = first;
              if (dimmer >= 2000)
              {
                dim = -10;
              }
              else if (dimmer <= 1)
              {
                dim = 10;
              }
              dimmer = dimmer + dim;
              dimmer = constrain(dimmer, 0, 4095);//ensure brightness is between 0-4095
              while (current <= last)
              {
                Tlc.set(current, dimmer);
                current++;
              }
            }
            break;
          }
        case 3://Randomizer
          {
            if (nowMillis >= (lastMillis + RANDinterval))//Check if it is time to update
            {
              //Serial.println("LED Randomizer");
              lastMillis = nowMillis;//Save new time
              current = first;
              while (current <= last)
              {
                dimmer = random(0, 4095);
                Tlc.set(current, dimmer);
                current++;
              }
            }
            break;
          }
        case 4://Normal
          {
            if (nowMillis >= (lastMillis + 100))//Check if it is time to update
            {
              //Serial.println("LED Normal");
              for (int channel = first; channel <= last; channel ++)
              {
                Tlc.set(channel, 4095);
              }
            }
            break;
          }
        case 5://Radar 1
          {
            if (nowMillis >= (lastMillis + 100))//Check if it is time to update
            {
              //Serial.println("LED Radar");
              Tlc.set(last, 4095);
            }
            break;
          }
      }
    }
};//////////////////////////////////////////////////////END LED CLASS///////////////////////////////////////////////////
class MP3Sequencer/////////////////////////////////START MP3Sequencer CLASS//////////////////////////////////////////////
{
    bool Play;
    long maxInterval;
    long minInterval;

    // These maintain the current state
    unsigned long lastMillis;   // will store last time LED was updated

    // Constructor - creates a Sequencer
    // and initializes the member variables and state
  public:
    MP3Sequencer(long intervalTime, long intervalTime2) //Constructor
    {
      maxInterval = intervalTime;
      minInterval = intervalTime2;
      lastMillis = 0;
    }

    void rndUpdate(bool isPlaying)//MP3Randomizer Update Function
    {
      unsigned long nowMillis = millis();//get current time
      Play = isPlaying;
      if (Play == HIGH) //Not Playing
      {
        int interval = random(minInterval, maxInterval);
        if (nowMillis >= (lastMillis + interval))//Check if it is time to update
        {
          MP3.next();//Play Next track
        }
      }
      else//Playing
      {
        lastMillis = millis();
      }
    }
    void ffUpdate(bool isPlaying)//FF
    {
      //mp3_stop();//Stop Playing
      Play = isPlaying;
      if (Play != HIGH) //Not Playing
      {
        MP3.next();//Play Next track
      }
      else
      {
        MP3.stop();//Stop Playing
        MP3.next();//Play Next track
      }
    }
    void rrUpdate(bool isPlaying)//MP3Randomizer Update Function
    {
      //mp3_stop();//Stop
      Play = isPlaying;
      if (Play != HIGH) //Not Playing
      {
        MP3.previous();//Play Previous track
      }
      else
      {
        MP3.stop();//Stop Playing
        MP3.previous();//Play Previous track
      }
    }
    void frcUpdate(bool isPlaying, int track)//MP3Randomizer Update Function
    {
      Play = isPlaying;
      if (Play != HIGH) //Not Playing
      {
        MP3.playMp3Folder(track);//Play Track
      }
      else
      {
        MP3.stop();//Stop Playing
        MP3.playMp3Folder(track);//Play Track
      }
    }
    void RandomUpdate(bool isPlaying)//MP3Randomizer Update Function
    {
      Play = isPlaying;
      int track = 40;
      if (Play != HIGH) //Not Playing
      {
        while (track > 20 && track < 90)
        {
          track = random(1, 99);
        }
        MP3.playMp3Folder(track);//Play Track
      }
      else
      {
        MP3.stop();//Stop Playing
        while (track > 20 && track < 90)
        {
          track = random(1, 99);
        }
        MP3.playMp3Folder(track);//Play Track
      }
    }
};/////////////////////////////////////////////////////END MP3SEQUENCER CLASS////////////////////////////////////////////////

LED Fr_Logics_1(0, 2, 75, 0, 75);
LED Fr_Logics_2(3, 5, 75, 0, 75);
LED Sd_Logics(6, 8, 75, 0, 75);
LED Radar_Eye(9, 11, 75, 0, 75);
MP3Sequencer Sequencer(30000, 15000);

void setup()
{
  Tlc.init();//Initialize TLC ICs

  Sound.begin (9600);//MP3 Baud Rate
  String buff;
  char chr;
  bool reading = false;
  Serial.begin (115200);
  delay(500);
  Serial.print("+++");
  delay(500);
  String NAME = "AT+NAME=" + myNAME;
  Serial.println(NAME);
  delay(500);
  Serial.println("AT+EXIT");
  delay(400);
  MP3.begin(Sound);//set softwareSerial for DFPlayer-mini mp3 module
  pinMode(speakerIn, INPUT); //Connected to Speaker
  pinMode(mp3In, INPUT); //Connected to MP3 State

  volumeOut = 5;
}

void loop()
{
  bool Playing = digitalRead(mp3In);//check if MP3 is still playing
  if (Serial.available())//Read BT
  {
    BT = Serial.read();
    if (BT == 69)
    {
      int buff = 0;
      while (buff < 2)
      {
        if (Serial.available())//Read BT
        {
          BT = Serial.read();
          if (BT = 10)
            buff++;
        }
      }
      BT = 0;
    }
    Serial.print("Bluetooth: ");
    Serial.println(BT);
    //delay(200); Optional?
    switch (BT)
    {
      case 112: //FTL Off
        FTL = 0;
        break;
      case 113: //FTL Sweep
        FTL = 1;
        break;
      case 114: //FTL Fade
        FTL = 2;
        break;
      case 115: //FTL Random
        FTL = 3;
        break;
      case 116: //FTL On
        FTL = 4;
        break;
      case 117: //FBL Off
        FBL = 0;
        break;
      case 118: //FBL Sweep
        FBL = 1;
        break;
      case 119: //FBL Fade
        FBL = 2;
        break;
      case 120: //FBL Random
        FBL = 3;
        break;
      case 121: //FBL On
        FBL = 4;
        break;
      case 122: //SL Off
        SL = 0;
        break;
      case 123: //SL Sweep
        SL = 1;
        break;
      case 124: //SL Fade
        SL = 2;
        break;
      case 125: //SL Random
        SL = 3;
        break;
      case 126: //SL On
        SL = 4;
        break;
      case 127: //RE Off
        RE = 0;
        break;
      case 128: //RE Sweep
        RE = 1;
        break;
      case 129: //RE Fade
        RE = 2;
        break;
      case 130: //RE All (3) on
        RE = 3;
        break;
      case 131: //RE Only First on
        RE = 4;
        break;
      case 132: //HP Off
        HP = 0;
        break;
      case 133: //HP On
      case 134: //HP On
      case 135: //HP On
      case 136: //HP On
        HP = 1;
        break;
      case 137: //PSI Off
        PSI = 0;
        break;
      case 138: //PSI On Talking
        PSI = 1;
        break;
      case 139: //PSI On Full brightness
      case 140: //PSI On
      case 141: //PSI On
        PSI = 2;
        break;
      // Old code that doesn't work.
      case 1://FTL
        if (FTL >= 4)
          FTL = 0;
        else
          FTL++;
        break;
      case 2://FBL
        if (FBL >= 4)
          FBL = 0;
        else
          FBL++;
        break;
      case 3://SL
        if (SL >= 4)
          SL = 0;
        else
          SL++;
        break;
      case 4://Radar
        if (RE >= 5)
          RE = 0;
        else
          RE++;
        break;
      case 5://HP
        if (HP == 1)
          HP = 0;
        else
          HP++;
        break;
      case 6://PSI
        if (PSI >= 2)
          PSI = 0;
        else
          PSI++;
        break;
      // End old code that doesn't work

      case 7://Vol+
        if (volumeOut < 30)
          volumeOut++;
        else
          volumeOut = 30;
        Serial.print("Volume Up: ");
        Serial.println(volumeOut);
        break;
      case 8://Vol-
        if (volumeOut > 0)
          volumeOut--;
        else
          volumeOut = 0;
        Serial.print("Volume Down: ");
        Serial.println(volumeOut);
        break;
      case 9://VolMax
        volumeOut = 30;
        Serial.print("Volume Max: ");
        Serial.println(volumeOut);
        break;
      case 10://VolMin
        volumeOut = 0;
        Serial.print("Volume Min: ");
        Serial.println(volumeOut);
        break;
      case 11://FF
        Sequencer.ffUpdate(Playing);
        break;
      case 12://PP
        Sequencer.rrUpdate(Playing);
        break;
      case 13://Rnd
        Sequencer.RandomUpdate(Playing);
        break;
      case 14://Autotoggle
        if (Play == 1)
        {
          Play = 0;
        }
        else
        {
          Play++;
        }
        break;
      // Case 99 is the reset button in the app
      case 99://MovieMode
        BT = 0;
        FTL = 4;
        FBL = 0;
        SL  = 4;
        RE = 4;
        HP = 1;
        PSI = 1;
        break;
      default:
        {
          if (BT > 100 && BT < 200)
          {
            MP3.playMp3Folder((BT % 100));
          }
        }
        break;
    }
  }


  //Serial.println(BT);
  BT = 0;
  MP3.volume(volumeOut);//set volume

  if (Play == 1)
  {
    Sequencer.rndUpdate(Playing);
  }

  int PSIval = analogRead(speakerIn);//Get Speaker value

  // Store the Min Max of the SpeakerIn, so we can calibrate the LD output brightness.
  if (PSIval < minVol) minVol = PSIval;
  if (PSIval > maxVol) maxVol = PSIval;

  Fr_Logics_1.Update(FTL);//Update Front Logics 1 Display
  Fr_Logics_2.Update(FBL);//Update Front Logics 2 Display
  Sd_Logics.Update(SL);//Update Side Logics

  // Additional handling for the RE, since we added 3 LED's
  // Movie mode only uses the first one, so handle that
  // On the iPhone app, we use Off for all off, Sweep sweeps 3, Fade fades 3, Random has all 3 on, and On has just one on.

  switch (RE)
  {
    case 0:
      // All off
      Tlc.set(11, 0);//Set RE to Off
      Tlc.set(10, 0);//Set RE to Off
      Tlc.set(9, 0);//Set RE to Off
      break;
    case 1:
      // Sweep
    case 2: 
      //Fade
      Radar_Eye.Update(RE);//Update Radar Eye Logics
      break;
    case 3: 
      //All on
      Tlc.set(11, 4095);//Set RE to On
      Tlc.set(10, 4095);//Set RE to On
      Tlc.set(9, 4095);//Set RE to On
      break;
    case 4: 
      //Just one one
      Tlc.set(11, 0);//Set RE to Off
      Tlc.set(10, 0);//Set RE to Off
      Tlc.set(9, 4095);//Set RE to On
      break;
  }

  
  //Radar_Eye.Update(RE);//Update Radar Eye Logics
  //PSI = 1;
  //Serial.print("Playing: ");
  //Serial.print(Playing);
  //Serial.print(" Min: ");
  //Serial.print(minVol);
  //Serial.print(" Max: ");
  //Serial.println(maxVol);

  // 13 is white. (pin 13)
  // 12 is blue (pin 12)
  // 11 is red (pin 11)
  // 8 is blue (pin 8)
  // 7 is blue (pin 7)
  // 6 is blue (pin 6)
  // 5 is white (pin 5)
  // 4 is white (pin 4)
  // 3 is white (pin 3)
  

  switch (PSI)
  {
    case 1:
      if (Playing == HIGH /*|| PSIval <= 550*/) //Not Playing
      {
        Tlc.set(13, 0);//Set PSI to Off
      }
      else //Playing
      {
        Serial.print("PSI Sound: ");
        Serial.print(PSIval);
        //int PSIbri = constrain(map(PSIval, minVol, maxVol, 1024, 4095), 0, 4095);//Solve for PSI output brightness
        int PSIbri = map(PSIval, minVol, maxVol, 1024, 4095);  //Solve for PSI output brightness
        Serial.print(" : ");
        Serial.println(PSIbri);
        Tlc.set(13, PSIbri); //Set PSI Brightness // 
      }
      break;
    case 2:
      Tlc.set(13, 4095);//Set PSI to On, full brightness
      break;
    case 0:
      Tlc.set(13, 0);//Set PSI to Off
      break;
  }
  Tlc.set(12, (4095 * HP)); //Set Holoprojector brightness
  Tlc.update();//Push new values to ICs

}
