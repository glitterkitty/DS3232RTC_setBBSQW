
/*
  - circuit like in DS3231 Switching power mos-mos.jpg
  - when mosfet off: ~2uA from cr2032, when on: ~5uA
  - n-ch mosfet: bs170, 1MO testing, 100KOhm ok 3vBatt Gate - SQW Source
  - p-ch mosfet: TSM480P06, 1MOhm gate-source
  - 10k pullups to SCA/SCL via pin 17 A3, direct to 3v3 arduino possible!
  - action led: p-ch mosfet drain - led - res - gnd
  - lil' beeper on pin 6
  - mosfet switch override button: gnd - button - n-ch mosfet source
  
note: with ftdi connected to arduino and PC, current draw from 
      cr2032 may be ~90 or higher... current measurements taken 
      with ftdi disconnected from PC

*/


#include <DS3232RTC.h>      // https://github.com/JChristensen/DS3232RTC
#include <LowPower.h>

DS3232RTC myRTC;

#define TONE_PIN  6

void setup()
{
  
  Serial.begin(115200);
  tone(TONE_PIN, 770, 60);


  // initialize
  //
  myRTC.begin();
  Serial.print("\nstart: ");
  printTime(myRTC.get());
  Serial.println();

}


void loop()
{

  // calc alarm
  //
  int wakeInterval = 17;  // minutes to next alarm
  time_t t_nul = myRTC.get();
  time_t t_alm = t_nul + (wakeInterval * 60);


  // setup alarm
  //
  myRTC.setAlarm(DS3232RTC::ALM1_MATCH_HOURS, minute(t_alm), hour(t_alm), 1);
  myRTC.alarmInterrupt(DS3232RTC::ALARM_1, true);
  myRTC.alarmInterrupt(DS3232RTC::ALARM_2, !true);
  myRTC.squareWave(DS3232RTC::SQWAVE_NONE);
  myRTC.setBBSQW(true); // needed to get sqw low when on battery!


  // printout 
  //
  Serial.print("loop:  ");
  printTime(t_nul);
  Serial.println();
  Serial.println( "intvl: " + String(wakeInterval) );
  Serial.print( "alarm: ");
  printTime(t_alm);


  // workzone
  //
  tone(TONE_PIN, 660, 60);
  delay(5000);



  // ... let the micro do some work here



  // work done
  //
  Serial.println("sleep");
  Serial.flush();
  tone(TONE_PIN, 400, 60);
  delay(70);


  // poweroff by clearing alarm flags
  //
  myRTC.alarm(DS3232RTC::ALARM_1) ;
  myRTC.alarm(DS3232RTC::ALARM_2) ;

  
  // in case rtc-off fails for whatever reason
  //
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  Serial.println("! no rtc wakeup !");


}


void printTime(time_t t) {

  Serial.print((hour(t) < 10) ? "0" : " ");
  Serial.print( hour(t)  );

  Serial.print((minute(t) < 10) ? ":0" : ":");
  Serial.print( minute(t) );

  Serial.print((second(t) < 10) ? ":0" : ":");
  Serial.print( second(t) );
  
}

