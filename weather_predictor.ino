

#include <WiFi.h>
#include <LiquidCrystal.h>
#include <ArduinoJson.h>
int contrast = 145;
#define D0 16 // GPIO3
#define D1 5 // GPIO1
#define D2 4 // GPIO16
#define D3 0 // GPIO5
#define D4 2 // GPIO4
#define D5 14 // GPIO14

 char ssid []    = "iPhone";      // SSID of local network
const char* password = "fearless";   // Password on network
String APIKEY = "45250cc44427dc71bd4dcd4c59024a70";
String CityID = "1272175"; //dhaka,BD


WiFiClient client;
char servername[] = "api.openweathermap.org"; // remote server we will connect to
String result;

int  counter = 60;

String weatherDescription = "";
String weatherLocation = "";
String Country;
float Temperature;
float Humidity;
float Pressure;
float Wind;
float Clouds;

LiquidCrystal lcd(D0, D1, D2, D3, D4, D5);

void setup() {
  analogWrite(A0,contrast);
  Serial.begin(115200);
  int cursorPosition = 0;
  lcd.begin(16, 2);
  lcd.print("   Connecting");
  Serial.println("Connecting");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.setCursor(cursorPosition, 2);
    lcd.print(".");
    cursorPosition++;
  }
  lcd.clear();
  lcd.print("   Connected!");
  Serial.println("Connected");
  delay(1000);

}

void loop() {
  if (counter == 60) //Get new data every 10 minutes
  {
    counter = 0;
    displayGettingData();
    delay(1000);
    getWeatherData();
  } else
  {
    counter++;
    displayWeather(weatherLocation, weatherDescription);
    delay(5000);
    serialMonitorData();
    displayConditions(Temperature, Humidity, Pressure, Wind, Clouds);
    delay(5000);
  }
}

void getWeatherData() //client function to send/receive GET request data.
{
  if (client.connect(servername, 80)) {  //starts client connection, checks for connection
    client.println("GET /data/2.5/weather?id=" + CityID + "&units=metric&APPID=" + APIKEY);
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  }
  else {
    Serial.println("connection failed"); //error message if no client connect
    Serial.println();
  }

  while (client.connected() && !client.available()) delay(1); //waits for data
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
    result = result + c;
  }

  client.stop(); //stop client
  result.replace('[', ' ');
  result.replace(']', ' ');
  Serial.println(result);

  char jsonArray [result.length() + 1];
  result.toCharArray(jsonArray, sizeof(jsonArray));
  jsonArray[result.length() + 1] = '\0';

  StaticJsonDocument<1024> json_doc;
 deserializeJson(json_doc, jsonArray);
  auto root = deserializeJson(json_doc, jsonArray );
  if (root)
  {
    Serial.print(F("deserializeJson() failed with code"));
    Serial.println(root.c_str());
    
  }
 
  

  String location = json_doc ["name"];
  String country = json_doc ["sys"]["country"];
  float temperature = json_doc["main"]["temp"];
  float humidity = json_doc ["main"]["humidity"];
  String weather = json_doc ["weather"]["main"];
  String description = json_doc ["weather"]["description"];
  float pressure = json_doc ["main"]["pressure"];
  float wind = json_doc["wind"]["speed"];
  float clouds = json_doc ["clouds"]["all"];

  weatherDescription = description;
  weatherLocation = location;
  Country = country;
  Temperature = temperature;
  Humidity = humidity;
  Pressure = pressure;
  Wind = wind;
  Clouds = clouds;
}

void displayWeather(String location, String description)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(location);
  lcd.print(", ");
  lcd.print(Country);
  lcd.setCursor(0, 1);
  lcd.print(description);

}

void displayConditions(float Temperature, float Humidity, float Pressure, float Wind, float Clouds)
{
  lcd.clear();
  lcd.print("Temp : ");
  lcd.print(Temperature, 1);
  lcd.print((char)223);
  lcd.print("C ");

  //Printing Humidity
  lcd.setCursor(0, 1);
  lcd.print("Humidty : ");
  lcd.print(Humidity, 1);
  lcd.print("%");

  delay(5000);
  lcd.clear();

  //Printing Pressure
  lcd.setCursor(0, 0);
  lcd.print("Pressure : ");
  lcd.print(Pressure, 1);
  lcd.print(" hPa");


  //Printing Wind
  lcd.setCursor(0, 1);
  lcd.print("Wind : ");
  lcd.print(Wind, 1);
  lcd.print(" m/s");

  delay(5000);
  lcd.clear();

  //Printing clouds
  lcd.setCursor(0, 0);
  lcd.print("Clouds : ");
  lcd.print(Clouds, 1);
  lcd.print(" %");

}

void displayGettingData()
{
  lcd.clear();
  lcd.print("Getting data");
}

void serialMonitorData()
{
  Serial.print("Temp : ");
  Serial.print(Temperature, 1);
  Serial.print("*C ");

  Serial.print("  Hum  : ");
  Serial.print(Humidity, 1);
  Serial.print("%");

  Serial.print(" Pres : ");
  Serial.print(Pressure, 1);
  Serial.print(" hPa");

  Serial.print("  Wind : ");
  Serial.print(Wind, 1);
  Serial.print(" m/s");

  Serial.print("  Clouds : ");
  Serial.print(Clouds, 1);
  Serial.println(" %");
}
