#include "U8glib.h"
#include "DHT.h"
#define DHTPIN 7
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
U8GLIB_SH1106_128X64 u8g(13, 11, 10, 9, 8);  // D0=13, D1=11, CS=10, DC=9, Reset=8

float minTemp = 100;
float maxTemp = 0;

void draw(float f, float h, float hif, float minTemp, float maxTemp) {
//   u8g.setFont(u8g_font_unifont);  // select font
  u8g.setFont(u8g_font_helvB24);  // select font helveticaaaa
  u8g.setFontPosTop();
  u8g.setScale2x2();
  u8g.setPrintPos(0, 0);  // set position
  u8g.print(f, 1);  // display temperature from DHT22
  u8g.undoScale();
  
  u8g.setFont(u8g_font_helvR08);
//  u8g.setFont(u8g_font_6x10);
  u8g.setPrintPos(12, 63);
  u8g.print(h, 0);
  
  u8g.setPrintPos(57, 63);
  u8g.print(minTemp, 1);

  u8g.setPrintPos(105, 63);
  u8g.print(maxTemp, 1);
  
  u8g.drawStr(0, 63, "h:");
  u8g.drawStr(25, 63, "%");
  u8g.drawStr(37, 63, "min:");
  u8g.drawStr(82, 63, "max:"); 

}

void setup(void) {
  Serial.begin(9600);
  dht.begin();
}

void loop(void) {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float c = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(c) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  
  if (f < minTemp) {
    minTemp = f;
  }
  
  if (f >= maxTemp) {
    maxTemp = f;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(c, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(c);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
  
  u8g.firstPage();
  do {
    draw(f, h, hif, minTemp, maxTemp);
  } while( u8g.nextPage() );
}



// // Example testing sketch for various DHT humidity/temperature sensors written by ladyada
// // REQUIRES the following Arduino libraries:
// // - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// // - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

// #include "DHT.h"

// #define DHTPIN 4     // Digital pin connected to the DHT sensor
// // Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// // Pin 15 can work but DHT must be disconnected during program upload.

// // Uncomment whatever type you're using!
// //#define DHTTYPE DHT11   // DHT 11
// #define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
// //#define DHTTYPE DHT21   // DHT 21 (AM2301)

// // Connect pin 1 (on the left) of the sensor to +5V
// // NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// // to 3.3V instead of 5V!
// // Connect pin 2 of the sensor to whatever your DHTPIN is
// // Connect pin 4 (on the right) of the sensor to GROUND
// // Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// // Initialize DHT sensor.
// // Note that older versions of this library took an optional third parameter to
// // tweak the timings for faster processors.  This parameter is no longer needed
// // as the current DHT reading algorithm adjusts itself to work on faster procs.
// DHT dht(DHTPIN, DHTTYPE);

// void setup() {
//   Serial.begin(9600);
//   Serial.println(F("DHTxx test!"));

//   dht.begin();
// }

// void loop() {
//   // Wait a few seconds between measurements.
//   delay(2000);

//   // Reading temperature or humidity takes about 250 milliseconds!
//   // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
//   float h = dht.readHumidity();
//   // Read temperature as Celsius (the default)
//   float t = dht.readTemperature();
//   // Read temperature as Fahrenheit (isFahrenheit = true)
//   float f = dht.readTemperature(true);

//   // Check if any reads failed and exit early (to try again).
//   if (isnan(h) || isnan(t) || isnan(f)) {
//     Serial.println(F("Failed to read from DHT sensor!"));
//     return;
//   }

//   // Compute heat index in Fahrenheit (the default)
//   float hif = dht.computeHeatIndex(f, h);
//   // Compute heat index in Celsius (isFahreheit = false)
//   float hic = dht.computeHeatIndex(t, h, false);

//   Serial.print(F("Humidity: "));
//   Serial.print(h);
//   Serial.print(F("%  Temperature: "));
//   Serial.print(t);
//   Serial.print(F("°C "));
//   Serial.print(f);
//   Serial.print(F("°F  Heat index: "));
//   Serial.print(hic);
//   Serial.print(F("°C "));
//   Serial.print(hif);
//   Serial.println(F("°F"));
// }
