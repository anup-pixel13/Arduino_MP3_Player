#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>
#include <AbleButtons.h>
#include <BlinkControl.h>

SoftwareSerial mySerial(10, 11); // TX, RX
DFPlayerMini_Fast myDFPlayer;

int LSRD = 8;
int L1 = 2;
int L2 = 3;
int ENRVRD = 4;
int HNVDRD = 5;
int MSPLCHVURD = 6;

using LTONOF = AblePullupCallbackButton;
using ENRV = AblePullupCallbackButton;
using HNVD = AblePullupCallbackDoubleClickerButton; 
using MSPLCHVU = AblePullupCallbackDoubleClickerButton; 

void swLtOnOf(LTONOF::CALLBACK_EVENT, uint8_t);
void plEnRv(ENRV::CALLBACK_EVENT, uint8_t);
void plHn(HNVD::CALLBACK_EVENT, uint8_t);
void plPaMs(MSPLCHVU::CALLBACK_EVENT, uint8_t);

LTONOF LTONOFBT(LSRD, swLtOnOf);
ENRV ENRVBT(ENRVRD, plEnRv);
HNVD HNVDBT(HNVDRD, plHn);
MSPLCHVU MSPLCHVUBT(MSPLCHVURD, plPaMs); 

bool ltrn = true;
bool mspv = true;
bool msch = true;
bool mspl = true;
bool vdrn = false;
bool vurn = false;

int enrun = 0;
int msrun = 0;
int hnrun = 0;
int blkEnabled = 0;
int blkDisabled = 1;

unsigned long t1 = 0;
unsigned long t2 = 0;
unsigned long t3 = 0;
unsigned long diff = 0;

BlinkControl led(2);
BlinkControl led1(3);

int playlist[138]; // 7 total chg for song addn
int index = 0;
int randomSong;
int lengthArr = 1;
bool alreadyPr = false;

int indexCrSg = -1;

int vol = 20;

void setup() 
{  
  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);

  Serial.begin(115200);
  mySerial.begin(9600);
  myDFPlayer.begin(mySerial);
  
  randomSeed(analogRead(0));

  LTONOFBT.begin();
  ENRVBT.begin();
  HNVDBT.begin();
  MSPLCHVUBT.begin();
  
  HNVD::setHeldTime(500);
  HNVD::setClickTime(1000);

  MSPLCHVU::setHeldTime(500);
  MSPLCHVU::setClickTime(1000);

  delay(300);
  
  playlist[0] = random(4,142); //
  
  present :
    delay(13);
    alreadyPr = false;
    randomSong = random(4,142); //
    for(int i = 0; i<lengthArr; i++)
    {
      if(playlist[i]==randomSong)
      {
        alreadyPr = true;
        break;
      }
    }
    
  if(alreadyPr)
  {
    goto present;   
  }
  else if(lengthArr <= 137) //
  {
    index = index + 1;
    playlist[index] = randomSong;
    lengthArr = lengthArr + 1;
    if(lengthArr < 138) //
    {
      goto present;
    }
  }
   
  myDFPlayer.play(1); //  "0001" start 11 sec
  myDFPlayer.volume(26);
  
  delay(20);
  digitalWrite(L1, HIGH);
  digitalWrite(L2, HIGH);
  delay(300); 
  digitalWrite(L1, LOW);
  digitalWrite(L2, LOW);
  delay(150);
  digitalWrite(L1, HIGH);
  digitalWrite(L2, HIGH);
  delay(330);
  digitalWrite(L1, LOW);
  digitalWrite(L2, LOW);
  delay(4600);
  digitalWrite(L1, HIGH);
  digitalWrite(L2, HIGH);
  delay(5454);
  digitalWrite(L1, LOW);
  digitalWrite(L2, LOW);
  
  if(myDFPlayer.isPlaying())
  {
    myDFPlayer.stop();
  }
}

void loop()
{  
  LTONOFBT.handle();
  ENRVBT.handle();
  HNVDBT.handle();
  MSPLCHVUBT.handle();

  led.loop();
  led1.loop();

  if(digitalRead(ENRVRD) == 0)
  {
    plEnRv(ENRV::PRESSED_EVENT, 1);
  }
  else if(digitalRead(ENRVRD) == 1)
  {
    plEnRv(ENRV::RELEASED_EVENT, 1);
  }
     
  if(HNVDBT.resetClicked()) 
  { 
    decVol();
  }

  if(HNVDBT.resetDoubleClicked()) 
  { 
    plMsPv();
  }

  if(MSPLCHVUBT.resetDoubleClicked()) 
  { 
    plMsRd();
  }

  if(MSPLCHVUBT.resetClicked()) 
  { 
    incVol();
  }
}

void swLtOnOf(LTONOF::CALLBACK_EVENT event, uint8_t id)
{
  if(!(ltrn))
  {
    return;
  }
  
  if(event == LTONOF::PRESSED_EVENT)
  {
    digitalWrite(L1, HIGH);
    digitalWrite(L2, HIGH);
  }
  else if(event == LTONOF::RELEASED_EVENT)
  {
    digitalWrite(L1, LOW);
    digitalWrite(L2, LOW); 
  }
} 

void incVol()
{
  if(!(vurn))
  {
    return;
  }
  if(myDFPlayer.isPlaying())
  {
    if(vol <= 26)
    {
      vol = vol + 2;
    }
    myDFPlayer.volume(vol);
  }
}

void decVol()
{
  if(!(vdrn))
  {
    return;
  }
  if(myDFPlayer.isPlaying())
  {
    if(vol >= 6)
    {
      vol = vol - 2;
    }
    myDFPlayer.volume(vol);
  }
}

void plHn(HNVD::CALLBACK_EVENT event, uint8_t id)
{   
  if(event == HNVD::HELD_EVENT)
  {  
    if(vol >= 6)
    {
       vol = vol+2;
    }
    
    vurn = false;
    vdrn = false;
    msch = false;
    mspl = false;
    mspv = false;

    int msEnd = 1;

    if(t1 == 0)
    {   
       t1 = millis();
    }
    else if(t1 != 0 && t2 == 0 && t3 == 0)
    {
       if(millis() - t1 >= 2000)
       {
         t1 = millis(); 
         t2 = 0;
         t3 = 0;
       }
       else
       {
         t2 = millis();
       }
    }
    else if(t1 != 0 && t2 != 0 && t3 == 0)
    {
       if(millis() - t2 >= 2000)
       {
         t1 = millis();
         t2 = 0;
         t3 = 0;
       }
       else
       {
         t3 = millis();

         diff = t3 - t1;
       
         if(diff >= 2500 && diff <= 4000)
         {  
           t1 = 0;
           t2 = 0;
           t3 = 0;
           
           if(blkEnabled == 1)
           {
             blkEnabled = 0; 
             blkDs(); 
           }
           else if(blkDisabled == 1)
           { 
             blkDisabled = 0; 
             blkEn();
           }
         } 
         else
         {
            t1 = 0;
            t2 = 0;
            t3 = 0;
         } 
       } 
    }
   
    if(myDFPlayer.isPlaying())
    {
      if(myDFPlayer.currentSdTrack() != 2);
      { 
        msEnd = 0;
      } 
      myDFPlayer.stop(); 
    }
    
    myDFPlayer.play(3); //  "0003" hn 840 ms
    myDFPlayer.volume(28); 
    delay(1000);
    
    hnrun = 1;
    msch = true;
    mspl = true;
    mspv = true;

    if(myDFPlayer.isPlaying())
    {
      myDFPlayer.stop();
    }
  
    if(msrun == 1 && msEnd == 0)
    {
      myDFPlayer.play(playlist[indexCrSg]);
      myDFPlayer.volume(vol);
            
      msrun = 1;
      hnrun = 0;
      enrun = 0;
      vdrn = true;
      vurn = true;
    }
    else if(enrun == 2)
    {
      plEnRv(ENRV::PRESSED_EVENT, 1);
    }
  }
}

void plMsRd()
{  
  if(vurn == true)
  {
    if(vol <= 26)
    {
      vol = vol-4;
    } 
  }
  else if(vurn == false)
  {
    if(vol <= 26)
    {
      vol = vol-2;
    }
  }
  
  if(!(msch))
  {
    return;
  }
  
  vdrn = true;
  vurn = true;
  mspl = true;
  mspv = true;
  
  if(indexCrSg >= -1 && indexCrSg <= 136) //
  {
    if(myDFPlayer.isPlaying())
    {
      myDFPlayer.stop();
    }
    
    indexCrSg = indexCrSg + 1;
    myDFPlayer.play(playlist[indexCrSg]);
    myDFPlayer.volume(vol);
        
    msrun = 1;
    hnrun = 0;
    enrun = 0;
  }
}

void plMsPv()
{ 
  if(vdrn == true)  
  {
    if(vol >= 6)
    {
      vol = vol+4;
    }
  }
  else if(vdrn == false)
  {
    if(vol >= 6)
    {
      vol = vol+2;
    }
  }
  
  if(!(mspv))
  {
    return;
  }
  
  vdrn = true;
  vurn = true;
  mspl = true;
  msch = true;

  if(indexCrSg >= 1 && indexCrSg <= 137) //
  {  
    if(myDFPlayer.isPlaying())
    {
      myDFPlayer.stop();
    }
    
    indexCrSg = indexCrSg - 1;
    myDFPlayer.play(playlist[indexCrSg]);
    myDFPlayer.volume(vol);
  
    msrun = 1;
    hnrun = 0;
    enrun = 0;
  }  
}

void plPaMs(MSPLCHVU::CALLBACK_EVENT event, uint8_t id)
{    
  if(event == MSPLCHVU::HELD_EVENT)
  {   
    if(!(myDFPlayer.isPlaying()) && vol <= 26)
    {
      vol = vol-2;
    }  
    
    if(!(mspl))
    {
      return;
    }
  
    vdrn = false;
    vurn = false;
    msch = true;
    mspv = true;

    if(myDFPlayer.isPlaying())
    {
      myDFPlayer.pause();
      msrun = 0;
      delay(1000);  
    }
    else if(!(myDFPlayer.isPlaying()) && indexCrSg != -1)
    {
      
      if(hnrun == 1 || enrun == 1)
      {
        myDFPlayer.play(playlist[indexCrSg]);
        myDFPlayer.volume(vol);
           
        msrun = 1; 
        hnrun = 0;
        enrun = 0; 
        vdrn = true;
        vurn = true;
        delay(1000);
      }
      else
      {
        myDFPlayer.resume();
        myDFPlayer.volume(vol);
                
        msrun = 1; 
        hnrun = 0;
        enrun = 0;
        vdrn = true;
        vurn = true;
        delay(1000); 
      }
    }
  }   
}

void plEnRv(ENRV::CALLBACK_EVENT event, uint8_t id)
{
  if(event == ENRV::PRESSED_EVENT)
  {
    if((myDFPlayer.isPlaying()))
    {
      return;
    }
    vdrn = false;
    vurn = false;
    msch = false;
    mspl = false;
    mspv = false;

    myDFPlayer.play(2); //  "0002" rv
    myDFPlayer.volume(15);
    
    enrun = 2; 
    hnrun = 0;
    msrun = 0;  
  }
  else if(event == ENRV::RELEASED_EVENT)
  {  
     if(myDFPlayer.isPlaying() && myDFPlayer.currentSdTrack() == 2)
     {
       myDFPlayer.stop();
  
       enrun = 1;    
       msch = true;
       mspl = true;
       mspv = true;
    }
  }
}

void blkEn()
{
  blkEnabled = 1;
  ltrn = false;
      
  digitalWrite(L1, LOW);
  digitalWrite(L2, LOW);
  
  led.begin();
  led1.begin();

  int timing[] = {850};

  led.blink(timing, 1);
  led1.blink(timing, 1);
}

void blkDs()
{
  blkDisabled = 1;
  
  led.off();
  led1.off();
  
   ltrn = true;
   
   if(digitalRead(LSRD) == 0)
   {
      digitalWrite(L1, HIGH);
      digitalWrite(L2, HIGH);
    }
}  
