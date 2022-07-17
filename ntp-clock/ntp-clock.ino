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
#define WEATHER_TIMEOUT 200
// =======================================================================

unsigned long epochTime;

String deg = "°";

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
float windDeg;

String sunrise;
String sunset;

String pollution;

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
    // P.setZoneEffect(0, true, PA_FLIP_LR);
    // P.setZoneEffect(0, true, PA_FLIP_UD);
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
    getPollutionData();
}

void loop()
{
    updateTimeDate();
    delay(1);
    animateScreenAndShow();
    delay(1);
    getWeatherData();
    getPollutionData();
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
    int stage = 21;
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
    case 21:
        return 3;
    case 20:
        return 19;
    case 19:
        return 5;
    case 18:
        return 1;
    case 17:
        return 12;
    case 16:
        return 26;
    case 15:
        return 3;
    case 14:
        return 12;
    case 13:
        return 8;
    case 12:
        return 3;
    case 11:
        return 3;
    case 10:
        return 3;

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
    case 21:
        return dayOfWeek + " " + String(date) + " " + month + ", " + String(year);
    case 20:
        return getFormattedTime(hour, minute);
    case 19:
        return meridiem;
    case 18:
        return "Temp: ";
    case 17:
        return String(temp, 1) + deg + "C";
    case 16:
        return "Range: ";
    case 15:
        return String(tempMin, 1) + deg + "C - " + String(tempMax, 1) + deg + "C";
    case 14:
        return "Realfeel: ";

    case 13:
        return String(tempRealFeel, 1) + deg + "C";
    case 12:
        return "Weather: ";
    case 11:
        return weatherDescription;
    case 10:
        return "Humidity: ";
    case 9:
        return String(humidity) + "%";
    case 8:
        return "Pressure: ";
    case 7:
        return String(pressure) + " hPa";
    case 6:
        return "Clouds: ";
    case 5:
        return String(clouds) + "%";
    case 4:
        return "Wind: ";
    case 3:
        return getWindWithCardinalDir();
    case 2:
        return "Sunrise: " + sunrise + " | Sunset: " + sunset;
    case 1:
        return pollution;
    default:
        return "Thanks";
    }
}

String getWindWithCardinalDir()
{
    String directions[] = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};
    return String(windSpeed, 1) + " m/s " + directions[int((windDeg + 11.25) / 22.5) % 16];
}

String getFormattedTime(int hr, int mins)
{
    String strHour, strMin;
    if (hr / 10 > 0)
    {
        strHour = String(hr);
    }
    else
    {
        strHour = "0" + String(hr);
    }

    if (mins / 10 > 0)
    {
        strMin = String(mins);
    }
    else
    {
        strMin = "0" + String(mins);
    }
    return strHour + ":" + strMin;
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
    unsigned long oldTime = millis();
    while (!client.available() && (millis() - oldTime) < WEATHER_TIMEOUT)
    {
        delay(2);
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
    windDeg = root["wind"]["deg"];
    clouds = root["clouds"]["all"];
    sunrise = convertEpochToTimeString(root["sys"]["sunrise"].as<unsigned long>() + utcOffsetInSeconds);
    sunset = convertEpochToTimeString(root["sys"]["sunset"].as<unsigned long>() + utcOffsetInSeconds);
}

void getPollutionData()
{
    String payload = String("GET http://api.openweathermap.org/data/2.5/air_pollution?lat=22.5726&lon=88.3639&appid=") + weatherKey + " HTTP/1.1\r\n" +
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
    unsigned long oldTime = millis();
    while (!client.available() && (millis() - oldTime) < WEATHER_TIMEOUT)
    {
        delay(2);
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

    pollution = "AQI: ";

    String aqi_states[] = {"Good", "Fair", "Moderate", "Poor", "Very Poor"};
    pollution = pollution + aqi_states[(root["list"][0]["main"]["aqi"].as<int>()) - 1] + " | (in ug/m3) - ";

    JsonObject &documentRoot = root["list"][0]["components"];

    for (JsonPair keyValue : documentRoot)
    {
        pollution = pollution + keyValue.key;
        pollution = pollution + ": ";
        pollution = pollution + String(keyValue.value.as<float>(), 1);
        pollution = pollution + ", ";
    }
}

String convertEpochToTimeString(unsigned long epch)
{
    struct tm *ptm = gmtime((time_t *)&epch);

    String local_meridian;
    int local_hour, local_min;
    local_hour = ptm->tm_hour;
    local_min = ptm->tm_min;

    if (local_hour >= 12)
    {
        local_meridian = "PM";
        local_hour = local_hour - 12;
    }
    else
    {
        local_meridian = "AM";
    }
    if (local_hour == 0)
    {
        local_hour = 12;
    }

    return getFormattedTime(local_hour, local_min) + " " + local_meridian;
}
