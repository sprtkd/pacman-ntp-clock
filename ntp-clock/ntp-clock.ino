#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN 12
#define DATA_PIN 15
#define CS_PIN 13

// =======================================================================
const char *ssid = "abc";              // SSID of local network
const char *password = "abc";          // Password on network
const long utcOffsetInSeconds = 19800; // UTC offset in seconds

String weatherKey = "abc"; // openweather api key
String weatherLang = "&lang=en";
String cityID = "1275004"; // Cityid from open weather
const char *weatherHost = "api.openweathermap.org";
// =======================================================================

unsigned long epochTime;

String deg = String(char('~' + 25));

String weatherMain = "";
String weatherDescription = "";
String weatherLocation = "";
int humidity;
int pressure;
float temp;
float tempRealFeel;
float tempMin, tempMax;
int clouds;
float windSpeed;

int date;
String month;
int year;
String dayOfWeek;
int hour;
int minute;
int second;
String meridiem;

MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char monthsOfTheYear[12][12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds); // create object of NTP client

struct sCatalog
{
    textEffect_t effect;
    const char *psz;
    uint16_t speed;
    uint16_t pause;
};

sCatalog catalog[] =
    {
        {PA_PRINT, "", 80, 0},
        {PA_SCROLL_UP, "", 80, 0},
        {PA_SCROLL_DOWN, "", 80, 0},
        {PA_SCROLL_LEFT, "", 80, 0},
        {PA_SCROLL_RIGHT, "", 80, 0},
        {PA_SPRITE, "", 80, 0},
        {PA_SLICE, "", 15, 0},
        {PA_MESH, "", 150, 0},
        {PA_FADE, "", 250, 0},
        {PA_DISSOLVE, "", 500, 0},
        {PA_BLINDS, "", 120, 0},
        {PA_RANDOM, "", 50, 0},
        {PA_WIPE, "", 80, 0},
        {PA_WIPE_CURSOR, "", 80, 0},
        {PA_SCAN_HORIZ, "", 80, 0},
        {PA_SCAN_HORIZX, "", 80, 0},
        {PA_SCAN_VERT, "", 80, 0},
        {PA_SCAN_VERTX, "", 80, 0},
        {PA_OPENING, "", 80, 0},
        {PA_OPENING_CURSOR, "", 80, 0},
        {PA_CLOSING, "", 80, 0},
        {PA_CLOSING_CURSOR, "", 80, 0},
        {PA_SCROLL_UP_LEFT, "", 80, 0},
        {PA_SCROLL_UP_RIGHT, "", 80, 0},
        {PA_SCROLL_DOWN_LEFT, "", 80, 0},
        {PA_SCROLL_DOWN_RIGHT, "", 80, 0},
        {PA_GROW_UP, "", 80, 0},
        {PA_GROW_DOWN, "", 80, 0},
};

// Sprite Definitions
const uint8_t F_PMAN1 = 6;
const uint8_t W_PMAN1 = 8;
static const uint8_t PROGMEM pacman1[F_PMAN1 * W_PMAN1] = // gobbling pacman animation
    {
        0x00,
        0x81,
        0xc3,
        0xe7,
        0xff,
        0x7e,
        0x7e,
        0x3c,
        0x00,
        0x42,
        0xe7,
        0xe7,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x24,
        0x66,
        0xe7,
        0xff,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x3c,
        0x7e,
        0xff,
        0xff,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x24,
        0x66,
        0xe7,
        0xff,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x00,
        0x42,
        0xe7,
        0xe7,
        0xff,
        0xff,
        0x7e,
        0x3c,
};

const uint8_t F_PMAN2 = 6;
const uint8_t W_PMAN2 = 18;
static const uint8_t PROGMEM pacman2[F_PMAN2 * W_PMAN2] = // ghost pursued by a pacman
    {
        0x00,
        0x81,
        0xc3,
        0xe7,
        0xff,
        0x7e,
        0x7e,
        0x3c,
        0x00,
        0x00,
        0x00,
        0xfe,
        0x7b,
        0xf3,
        0x7f,
        0xfb,
        0x73,
        0xfe,
        0x00,
        0x42,
        0xe7,
        0xe7,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x00,
        0x00,
        0x00,
        0xfe,
        0x7b,
        0xf3,
        0x7f,
        0xfb,
        0x73,
        0xfe,
        0x24,
        0x66,
        0xe7,
        0xff,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x00,
        0x00,
        0x00,
        0xfe,
        0x7b,
        0xf3,
        0x7f,
        0xfb,
        0x73,
        0xfe,
        0x3c,
        0x7e,
        0xff,
        0xff,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x00,
        0x00,
        0x00,
        0xfe,
        0x73,
        0xfb,
        0x7f,
        0xf3,
        0x7b,
        0xfe,
        0x24,
        0x66,
        0xe7,
        0xff,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x00,
        0x00,
        0x00,
        0xfe,
        0x73,
        0xfb,
        0x7f,
        0xf3,
        0x7b,
        0xfe,
        0x00,
        0x42,
        0xe7,
        0xe7,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x00,
        0x00,
        0x00,
        0xfe,
        0x73,
        0xfb,
        0x7f,
        0xf3,
        0x7b,
        0xfe,
};

void setup()
{
    Serial.begin(115200);
    P.begin();
    P.setInvert(false);
    P.setIntensity(1);
    Serial.print("Connecting WiFi ");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected: ");
    Serial.println(WiFi.localIP());
    P.setZoneEffect(0, true, PA_FLIP_LR);
    P.setZoneEffect(0, true, PA_FLIP_UD);
    timeClient.begin();
#if ENA_SPRITE
    P.setSpriteData(pacman1, W_PMAN1, F_PMAN1, pacman2, W_PMAN2, F_PMAN2);
#endif
    P.displayText("Digital clock by Joydada & Rohit", PA_CENTER, catalog[3].speed, catalog[3].pause, catalog[3].effect, catalog[3].effect);
    while (!P.displayAnimate())
    {
        delay(1);
    }
    getWeatherData();
}

void loop()
{
    updateTimeDate();
    delay(1);
    animateScreenAndShow();
    delay(1);
    getWeatherData();
}

void updateTimeDate()
{
    timeClient.update();
    epochTime = timeClient.getEpochTime();
    Serial.println("epoch");
    Serial.println(epochTime);
    struct tm *ptm = gmtime((time_t *)&epochTime);
    date = ptm->tm_mday;
    int monthNum = ptm->tm_mon;
    month = monthsOfTheYear[monthNum];
    year = ptm->tm_year + 1900;
    dayOfWeek = daysOfTheWeek[timeClient.getDay()];
    minute = timeClient.getMinutes();
    second = timeClient.getSeconds();
    /*Convert 24hr format to 12hr format*/
    if (timeClient.getHours() >= 12)
    {
        meridiem = "PM";
        hour = timeClient.getHours() - 12;
    }
    else
    {
        meridiem = "AM";
        hour = timeClient.getHours();
    }
    if (hour == 0)
    {
        hour = 12;
    }
}

void animateScreenAndShow()
{
    int stage = 9;
    while (stage > 0)
    {
        animateCurr(getDataToShow(stage), getCurrAnim(stage));
        stage = stage - 1;
        delay(5);
    }
}

void animateCurr(String data, int currAnim)
{
    P.displayZoneText(0, data.c_str(), PA_CENTER, catalog[currAnim].speed, catalog[currAnim].pause, catalog[currAnim].effect, catalog[currAnim].effect);
    while (!P.displayAnimate())
    {
        delay(1);
    }
}

int getCurrAnim(int currStage)
{
    switch (currStage)
    {
    case 9:
        return 19;
    case 8:
        return 3;
    case 7:
        return 5;
    case 6:
        return 3;
    case 5:
        return 3;
    case 4:
        return 3;
    case 3:
        return 3;
    case 2:
        return 3;
    case 1:
        return 3;
    default:
        return 3;
    }
}

String getDataToShow(int currStage)
{
    switch (currStage)
    {
    case 9:
        return dayOfWeek;
    case 8:
        return String(date) + " " + month + ", " + String(year);
    case 7:
        return String(hour) + ":" + String(minute) + " " + meridiem;
    case 6:
        return "Temp: " + String(temp, 1) + deg + "C";
    case 5:
        return "Realfeel: " + String(tempRealFeel, 1) + deg + "C";
    case 4:
        return "Weather: " + weatherDescription;
    case 3:
        return "Humidity: " + String(humidity) + "%";
    case 2:
        return "Pressure: " + String(pressure) + " hPa  ";
    case 1:
        return "Clouds: " + String(clouds) + "%";
    default:
        return "Wind: " + String(windSpeed, 1) + " m/s";
    }
}

void getWeatherData()
{
    String payload = String("GET /data/2.5/weather?id=") + cityID + "&units=metric&appid=" + weatherKey + weatherLang + " HTTP/1.1\r\n" +
                     "Host: " + weatherHost + "\r\nUser-Agent: ArduinoWiFi/1.1\r\n" +
                     "Connection: close\r\n\r\n";
    WiFiClient client;
    Serial.print("connecting to ");
    Serial.println(weatherHost);
    Serial.println(payload);
    if (client.connect(weatherHost, 80))
    {
        client.println(payload);
    }
    else
    {
        Serial.println("connection failed");
        return;
    }
    String line;
    int repeatCounter = 0;
    while (!client.available() && repeatCounter < 20)
    {
        delay(5);
        repeatCounter++;
    }
    client.setNoDelay(false);
    bool jsonStarted = false;
    while (client.connected())
    {
        String currline = client.readStringUntil('\n');
        if (jsonStarted == false && currline.startsWith("{"))
        {
            jsonStarted = true;
        }
        if (jsonStarted)
        {
            Serial.print(currline);
            line += currline;
        }
    }
    client.stop();

    DynamicJsonBuffer jsonBuf;
    JsonObject &root = jsonBuf.parseObject(line);
    if (!root.success())
    {
        Serial.println("parseObject() failed");
        Serial.println(line);
        return;
    }
    weatherDescription = root["weather"][0]["main"].as<String>();
    temp = root["main"]["temp"];
    humidity = root["main"]["humidity"];
    pressure = root["main"]["pressure"];
    tempMin = root["main"]["temp_min"];
    tempMax = root["main"]["temp_max"];
    tempRealFeel = root["main"]["feels_like"];
    windSpeed = root["wind"]["speed"];
    clouds = root["clouds"]["all"];
}