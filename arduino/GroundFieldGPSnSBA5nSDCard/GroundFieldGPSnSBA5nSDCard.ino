
#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <SPI.h>
#include <SD.h>

//@Note ChatGPT helped generate some of the inital serial comm checks for the ublox GPS msgs

File logFile;
Adafruit_7segment matrix = Adafruit_7segment();
bool GPSfix;
const int timezoneOffset = -6; // UTC-6 (Mountain Daylight Time) Define the timezone offset in hours
String filename = "data";
String formattedDate = "";
String formattedTime = "";

void setup() {
  Serial.begin(19200);      // Initialize the serial monitormodule
  Serial1.begin(19200);
  Serial2.begin(9600);
  matrix.begin(0x70);
  if (!SD.begin(4)) {
    matrix.print("FaiL");
    matrix.writeDisplay();
    Serial.println("initialization failed!");
    delay(10);
    while (1);
  }

  //Serial1.print("!");  // measurement display off
  //Serial1.print("C2\r");  // Configure to 2 decimal places
  //Serial1.print("P1\r");  // turn on pump
  //Serial1.print("A2\r"); //Time [minutes] between zero operations
  //Serial1.print("Z"); // Perform a zero operation.
  //matrix.print(88.88);
  matrix.print(8888.11);
  matrix.writeDisplay();
  delay(10);
}

void loop() {
  
  if (Serial1.available()) {
    //Serial1.print("M");
    String sba5Message = Serial1.readStringUntil('\n');
    Serial.println(sba5Message);
    if (sba5Message[0] == 'W'){
      float warm = sba5Message.substring(2).toFloat();
      Serial.print("Warming: ");
      Serial.println(warm);

      // This will blink the display when in the warming state
      matrix.print(warm);
      matrix.writeDisplay();
      delay(100);
      matrix.clear();
      matrix.writeDisplay();
      delay(100);
      matrix.print(warm);
      matrix.writeDisplay();
      }
    if (sba5Message[0] == 'Z'){
        int zeroCount = sba5Message.substring(2,2).toInt();
        if (zeroCount==1){
          logFile = SD.open(filename, O_WRITE | O_CREAT | O_APPEND);
          logFile.println(formattedDate + " " + formattedTime + " Zeroed");
          logFile.close();
        }
        
        matrix.print(zeroCount);
        matrix.writeDisplay();
        delay(100);
        matrix.print(0);
        matrix.writeDisplay();
        delay(100);
        matrix.print(zeroCount);
        matrix.writeDisplay();
        
    }
    if (sba5Message[0] == 'M'){
      // Find the indices of the third and fourth spaces
      int thirdSpaceIndex = sba5Message.indexOf(' ', sba5Message.indexOf(' ', 2) + 1);
      
      // Extract the fourth element using substring
      String fourthElement = sba5Message.substring(thirdSpaceIndex + 1, thirdSpaceIndex + 6);
      Serial.println(fourthElement);
      
      // Convert the fourth element to a float
      float value = fourthElement.toFloat();
      logFile = SD.open(filename, O_WRITE | O_CREAT | O_APPEND);
      logFile.println(formattedDate + " " + formattedTime + sba5Message);
      logFile.close();
      
      // Limit the float value to 4 digits
      //String formattedValue = String(value, 4);
      //value = fourthElement.toFloat();

      matrix.print(value);
      if(GPSfix){matrix.drawColon(true);}
      matrix.writeDisplay();
      }
  }

  Serial.println("Between Serial1 & Serial2");
  if (Serial2.available()) {
    String nmeaMessage = Serial2.readStringUntil('\n'); // Read the NMEA message until newline character
 
    // Check if the message starts with "$GPRMC" (for date and time)
    if (nmeaMessage.startsWith("$GPRMC")) {
      // Split the message into fields using comma as the delimiter
      String fields[12]; // Assuming there are 12 fields in $GPRMC message
      int fieldCount = 0;
      int startIndex = 0;
      
      for (int i = 0; i < nmeaMessage.length(); i++) {
        if (nmeaMessage[i] == ',') {
          fields[fieldCount++] = nmeaMessage.substring(startIndex, i);
          startIndex = i + 1;
        }
      }
      
      // Extract the date and time from the fields
      String date = fields[9];
      String theTime = fields[1];
      
      // Format the date
      formattedDate = date.substring(4, 6) + "/" + date.substring(2, 4) + "/" +date.substring(0, 2);
      
      // Format the time as "HH:MM:SS"
      formattedTime = theTime.substring(0, 2) + ":" + theTime.substring(2, 4) + ":" + theTime.substring(4, 6);
      // Apply timezone offset
      int hours = formattedTime.substring(0, 2).toInt();
      hours += timezoneOffset;
      // Ensure the hours remain within the valid range (0-23)
      hours = (hours + 24) % 24;
      
      // Update the formatted time with the adjusted hours
      formattedTime = String(hours) + formattedTime.substring(2);
      
      if((filename.length() <= 4) && (formattedDate.length() > 4)){
        // 8 character limit
        filename = date.substring(2, 4)+"."+date.substring(0, 2);
        matrix.print("LoG");
        matrix.writeDisplay();
        delay(1000);
        matrix.clear();
        matrix.writeDisplay();
        delay(10);
        matrix.print(date.substring(2, 4) + "." + date.substring(0, 2));
        matrix.writeDisplay();
        delay(3000);
        logFile = SD.open("log.txt", O_WRITE | O_CREAT | O_APPEND);
        logFile.println("Log Starting:" + formattedDate + "at" + formattedTime);
        logFile.close(); 
        }
      
      
      // Print the date and time
      Serial.print("Date: ");
      Serial.println(formattedDate);
      
      Serial.print("Time: ");
      Serial.println(formattedTime);
    }
    
    
    if (nmeaMessage.startsWith("$GPGSV")) {
      // Split the message into fields using comma as the delimiter
      String fields[18]; // Assuming there are 18 fields in $GPGSV message
      int fieldCount = 0;
      int startIndex = 0;
      
      for (int i = 0; i < nmeaMessage.length(); i++) {
        if (nmeaMessage[i] == ',') {
          fields[fieldCount++] = nmeaMessage.substring(startIndex, i);
          startIndex = i + 1;
        }
      }
      
      // Extract the count of satellites in view from the fields
      int satelliteCount = fields[3].toInt();
      if(satelliteCount > 6){ GPSfix = true; }
      else{ GPSfix = false; }
      // Print the count of satellites in view
      Serial.print("Satellites in View: ");
      Serial.println(satelliteCount);
    }
   
    // if (nmeaMessage.startsWith("$GPGLL")) {Serial.println(nmeaMessage);}
    if (nmeaMessage.startsWith("$GPGGA")) {
      // Split the message into fields using comma as the delimiter
      String fields[15]; // Assuming there are 15 fields in $GPGGA message
      int fieldCount = 0;
      int startIndex = 0;
      //Serial.println(nmeaMessage);
      for (int i = 0; i < nmeaMessage.length(); i++) {
        if (nmeaMessage[i] == ',') {
          fields[fieldCount++] = nmeaMessage.substring(startIndex, i);
          startIndex = i + 1;
        }
      }
      
      // Extract latitude, longitude, and altitude from the fields
      String latitude = fields[2];
      String longitude = fields[4];
      String altitude = fields[9];     
      
      // Print the values
      Serial.print("Latitude: ");
      Serial.println(latitude);
      
      Serial.print("Longitude: ");
      Serial.println(longitude);
      
      Serial.print("Altitude: ");
      Serial.println(altitude);
    }
  }
  // Check if the message starts with "$GPGGA" (for altitude, latitude, longitude)
}



// Function to get the number of days in a month
int daysInMonth(int month, int year) {
  switch (month) {
    case 1: // January
    case 3: // March
    case 5: // May
    case 7: // July
    case 8: // August
    case 10: // October
    case 12: // December
      return 31;
    case 4: // April
    case 6: // June
    case 9: // September
    case 11: // November
      return 30;
    case 2: // February
      if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
        // Leap year
        return 29;
      } else {
        return 28;
      }
  }
  return 0; // Invalid month
}
