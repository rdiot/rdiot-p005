/* Arduino Yun + Temboo + Google Spreadsheets [P005] : http://rdiot.tistory.com/26 [RDIoT Demo] */

#include <Bridge.h>
#include <Process.h>
#include <Temboo.h>
#include "DHT.h"
//#include "TembooAccount.h" // contains Temboo account information, as described belo
#define DHTPIN 5     

int R = 9; // OUTPUT PIN red
int G = 10; // OUTPUT PIN green

int numRuns = 1;   // Execution count, so this doesn't run forever
int maxRuns = 3000;   // Maximum number of times the Choreo should be executed
Process date;
unsigned long time;

#define TEMBOO_ACCOUNT "rdiot"  // Your Temboo account name 
#define TEMBOO_APP_KEY_NAME "ArduinoYun"  // Your Temboo app key name
#define TEMBOO_APP_KEY "XXXXXXXXXXXXXXXXXXXXXXXXXXXXX"  // Your Temboo app key

DHT dht(DHTPIN, DHT22);

void setup()
{

 pinMode(R, OUTPUT);
 pinMode(G, OUTPUT);

  Serial.begin(9600);
  delay(3000);
  while(!Serial)
  Bridge.begin();
  time = millis();
  if(!date.running())
  {
    date.begin("date");
    date.addParameter("+%D-%T"); //-- 10/12/14-13:22:15 
    date.run();
  }

  Serial.println("start"); 
  
/*
  date.begin("date");
  date.addParameter("+%D-%T"); //-- 10/12/14-13:22:15  
  date.run();
  String timeString = date.readString();
  Serial.println("date="+timeString); 
*/

}

void loop()
{
 analogWrite(R, 0);
 analogWrite(G, 0);
  if (numRuns <= maxRuns) {
    Serial.println("Running AppendRow - Run #" + String(numRuns++));
 
 switch(dht.read())
  {
    case DHT_OK:
      Serial.println("temperature="+(String)dht.temperature+"'C"); 
      Serial.println("humidity="+(String)dht.humidity+"%"); 
      break;
    case DHT_ERR_CHECK:
      Serial.println("DHT CHECK ERROR"); 
      break;
    case DHT_ERR_TIMEOUT:
      Serial.println("DHT TIMEOUT ERROR"); 
      break;
    default:
      Serial.println("UNKNOWN ERROR"); 
      break;
    }
    runAppendRow(dht.temperature, dht.humidity);

 // RED + Green
 for (int i = 0; i < 256; i++)
 {
 analogWrite(R, i);
 analogWrite(G, i);
 delay(10);
 }

 for (int i = 0; i < 256; i++)
 {
 analogWrite(R, 255 - i);
 analogWrite(G, 255 - i);
 delay(10);
 } 
    
  }

 
  Serial.println("Waiting...");
  delay(60000); 

}

void runAppendRow(float temperature, float humidity)
{
    TembooChoreo AppendRowChoreo;

    // Invoke the Temboo client
    AppendRowChoreo.begin();

    // Set Temboo account credentials
    AppendRowChoreo.setAccountName(TEMBOO_ACCOUNT);
    AppendRowChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    AppendRowChoreo.setAppKey(TEMBOO_APP_KEY);

    AppendRowChoreo.setProfile("yunTemperatureSpreadsheet");

    // Identify the Choreo to run
    AppendRowChoreo.setChoreo("/Library/Google/Spreadsheets/AppendRow");
    
    if(!date.running())
    {
     date.begin("date");
     date.addParameter("+%D-%T"); //-- 10/12/14-13:22:15 
     date.run();
    }
    String timeString = date.readString();
    timeString.trim();
    String data = "";
    data = data + timeString + "," + String(temperature) + "," + String(humidity);
    Serial.println(data);

    AppendRowChoreo.addInput("RowData", data);

    // Run the Choreo; when results are available, print them to serial
    AppendRowChoreo.run();

    while(AppendRowChoreo.available()) {
      char c = AppendRowChoreo.read();
      Serial.print(c);
    }
    AppendRowChoreo.close();

}
