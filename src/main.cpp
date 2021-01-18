#include "Arduino.h"
#include "WiFi.h"
#include "Audio.h"
#include "captive_portal.h"
#include "mytask.h"

// Digital I/O used for MAX98357
//#define I2S_DOUT      27  // DIN connection
//#define I2S_BCLK      14  // Bit clock
//#define I2S_LRC       12  // Left Right Clock

// Digital I/O used for PCM5102
#define I2S_DOUT      0  // DIN connection
#define I2S_BCLK      4  // Bit clock
#define I2S_LRC       2  // Left Right Clock

Audio audio;

//push button
int gpio32 = 1;
int last_gpio32 = 1;

// =====================================================
// tosk run by Taskscheduler to handle Audio
class TaskAudio : public Task {
  public:
    void (*_myCallback)();
    ~TaskAudio() {};
    TaskAudio(unsigned long interval, Scheduler* aS ) :  Task(interval, TASK_FOREVER, aS, true) {
      
    };
    bool Callback(){
      //_myCallback();
      audio.loop();
      return true;     
    };
};
TaskAudio * myTaskAudio;
//============================================
void button_loop(){
  //Serial.printf("millis=%lu gpio12=%d\r",millis(),gpio12);
  if(digitalRead(32)==1){
    //Serial.printf("GPIIO32=1\r");  
    gpio32 = gpio32 < 10 ? gpio32 + 1 : 10; 
  }else{
    //Serial.printf("GPIIO32=0\r");  
    gpio32 = gpio32 > 0 ? gpio32 - 1 : 0; 
  }
  if( (gpio32 == 10 || gpio32 == 0) && gpio32 != last_gpio32){
    //Serial.printf("gpio32 turned from %d to %d\r\n",last_gpio32,gpio32);

    if(last_gpio32 == 10 and gpio32 == 0){
      // someone pushed the button
      Serial.println("last_gpio32 == 10 and gpio32 == 0 -> cambia stazione");
      // find the next not empty url
      for(int i=0; i<5;i++){
         station = (station+1)%5;
         if(stations[station] != "")
             break;
      }   
      saveCredentials();
      audio.connecttohost(stations[station]);
    }
    last_gpio32 = gpio32;
  }
}

class  MyTaskButton:MyTask{
  public:
    void (*_myCallback)();
    ~MyTaskButton(){};
    MyTaskButton(unsigned long interval, Scheduler* aS, void (* myCallback)() )
                  :  MyTask(interval, aS, myCallback){};
    
} * myTaskButton;
//==============================================
void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();

    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(15); // 0...21


// MEDITATION
//    audio.connecttohost("http://hirschmilch.de:7000/chillout.aac"); //  128k aac
//    audio.connecttohost("http://uk2.internet-radio.com:31491"); //  128k mp3
//    audio.connecttohost("http://uk5.internet-radio.com:8347/");
     
//    audio.connecttospeech("Wenn die Hunde schlafen, kann der Wolf gut Schafe stehlen.", "de");
//    audio.connecttohost("http://media.ndr.de/download/podcasts/podcast4161/AU-20190404-0844-1700.mp3"); // podcast

  CaptivePortalSetup(); 
  _PL("TaskScheduler Audio Task");
  myTaskAudio = new TaskAudio(10,&myScheduler);

  pinMode(32, INPUT_PULLUP); // the button will short to ground
  myTaskButton = new MyTaskButton(20,&myScheduler,button_loop); 

}

void loop()
{
    myScheduler.execute();
    
}

// optional
void audio_info(const char *info){
    Serial.print("info        "); Serial.println(info);
}
void audio_id3data(const char *info){  //id3 metadata
    Serial.print("id3data     ");Serial.println(info);
}
void audio_eof_mp3(const char *info){  //end of file
    Serial.print("eof_mp3     ");Serial.println(info);
}
void audio_showstation(const char *info){
    Serial.print("station     ");Serial.println(info);
}
void audio_showstreaminfo(const char *info){
    Serial.print("streaminfo  ");Serial.println(info);
}
void audio_showstreamtitle(const char *info){
    Serial.print("streamtitle ");Serial.println(info);
}
void audio_bitrate(const char *info){
    Serial.print("bitrate     ");Serial.println(info);
}
void audio_commercial(const char *info){  //duration in sec
    Serial.print("commercial  ");Serial.println(info);
}
void audio_icyurl(const char *info){  //homepage
    Serial.print("icyurl      ");Serial.println(info);
}
void audio_lasthost(const char *info){  //stream URL played
    Serial.print("lasthost    ");Serial.println(info);
}
void audio_eof_speech(const char *info){
    Serial.print("eof_speech  ");Serial.println(info);
}