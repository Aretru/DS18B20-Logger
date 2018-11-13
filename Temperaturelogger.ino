/*
 * (C) 2018 Rupert Lenzenweger - aretru.org
 * Temperature Logger for a RTM mold with 4 sensors.
 * As the mold is made of steel and has a high amount of heat capacity
 * changes will not occure very fast. Therefore a measurement every 2
 * seconds is more than enough.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <SD.h>
#include <virtuabotixRTC.h>

virtuabotixRTC myRTC(4, 5, 6); //Pins for the DS1302 RTC (CLK, DAT, RST)

#define ONE_WIRE_BUS 3  // Pin for sensors
#define DS18B20_Aufloesung 12  //
DeviceAddress DS18B20_Adress;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature myDS18B20(&oneWire);

#define NumberSensorsDS18B20  4  // Number of Sensors

const float NoVal = 999.99;
float Temperatur[4] = {NoVal,NoVal, NoVal, NoVal};

const int chipSelect = 10;  // For SD Card

// LED Setup
const int ledGreen = 7;
const int ledRed = 8;

int ledStateGrun = LOW;
int ledStateRot = LOW;

// Red and Green LEDS on: SD Card missing
// Red LED on: 1 Sensor disconnected
// Green LED blinks: Value written to SD Card
// Red LED blinks: Value NOT writeable to SD Card

void setup(void) {
  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);
  Serial.begin(9600);
  Serial.println("DS18B20 Test");
  Serial.println();
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    digitalWrite(ledGreen, 1);
    // don't do anything more, just signal with LEDs:
    while (1) {
      digitalWrite(ledRed, 1);
      delay(500);
      digitalWrite(ledRed, 0);
      delay(500);
    };
  }
  Serial.println("card initialized.");
  delay(1000);

  Serial.print("# active sensors: ");
  Serial.println(NumberSensorsDS18B20);

  if ((NumberSensorsDS18B20 > 0) and (NumberSensorsDS18B20 < 5)) {
    myDS18B20.begin();
    Serial.print("# active sensors: ");
    Serial.println(myDS18B20.getDeviceCount(), DEC);
    Serial.println();

    for(byte i=0 ;i < myDS18B20.getDeviceCount(); i++) {
      if(myDS18B20.getAddress(DS18B20_Adress, i)) {
        myDS18B20.setResolution(DS18B20_Adress, DS18B20_Aufloesung);
      }
    }
  }
}

void loop(void) {
  // make a string for assembling the data to log:
  String dataString = "";

  // Getting the time from the RTC and safe it to the string
  myRTC.updateTime();
  dataString += myRTC.dayofmonth; //You can switch between day and month if you're using American system
  dataString += ".";
  dataString += myRTC.month;
  dataString += ".";
  dataString += myRTC.year;
  dataString += " ";
  if (myRTC.hours < 10) {  // Check if leading 0 is necessary, just for layout reasons
    dataString += "0";
  };
  dataString += myRTC.hours;
  dataString += ":";
  if (myRTC.minutes < 10) {
    dataString += "0";
  };
  dataString += myRTC.minutes;
  dataString += ":";
  if (myRTC.seconds < 10) {
    dataString += "0";
  };
  dataString += myRTC.seconds;
  dataString += "\t";

  if ((NumberSensorsDS18B20 > 0) and (NumberSensorsDS18B20 < 5)) {
    myDS18B20.requestTemperatures();
    for(byte i=0 ;i < NumberSensorsDS18B20; i++) {
      if (i < myDS18B20.getDeviceCount()) {
        Temperatur[i] = myDS18B20.getTempCByIndex(i);
        // DEVICE_DISCONNECTED_C does not work for me
        if (Temperatur[i] == DEVICE_DISCONNECTED_C) {
            Temperatur[i] = NoVal;
            Serial.println("Error");
            digitalWrite(ledRed, 1);
        } else {
              dataString += Temperatur[i];
              dataString += "; " ;
        }
      }
    }
  }

  // open the file.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too and indicate it with green LED
    Serial.println(dataString);
    digitalWrite(ledGreen, 1);
  }
  // if the file isn't open, pop up an error and indicate it with red LED:
  else {
    Serial.println(dataString);
    Serial.println("error opening datalog.txt");
    digitalWrite(ledRed, 1);
  }

  // Wait a second, otherwise the state of the LEDs are changed too fast to see
  delay(1000);
  digitalWrite(ledGreen, 0);
  digitalWrite(ledRed, 0);
  delay(1000);
}
