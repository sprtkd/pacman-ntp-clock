#include <MD_Parola.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN 12
#define DATA_PIN 15
#define CS_PIN 13

// =======================================================================
const char *ssid = "abc";              // SSID of local network
const char *password = "abc";          // Password on network
const long utcOffsetInSeconds = 19800; // UTC offset in seconds
// =======================================================================

unsigned long epochTime;

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

MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

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
        {PA_PRINT, "", 80, 3000},
        {PA_SCROLL_UP, "", 80, 3000},
        {PA_SCROLL_DOWN, "", 80, 3000},
        {PA_SCROLL_LEFT, "", 80, 3000},
        {PA_SCROLL_RIGHT, "", 80, 3000},
        {PA_SPRITE, "", 80, 3000},
        {PA_SLICE, "", 15, 3000},
        {PA_MESH, "", 150, 3000},
        {PA_FADE, "", 250, 3000},
        {PA_DISSOLVE, "", 500, 3000},
        {PA_BLINDS, "", 120, 3000},
        {PA_RANDOM, "", 50, 3000},
        {PA_WIPE, "", 80, 3000},
        {PA_WIPE_CURSOR, "", 80, 3000},
        {PA_SCAN_HORIZ, "", 80, 3000},
        {PA_SCAN_HORIZX, "", 80, 3000},
        {PA_SCAN_VERT, "", 80, 3000},
        {PA_SCAN_VERTX, "", 80, 3000},
        {PA_OPENING, "", 80, 3000},
        {PA_OPENING_CURSOR, "", 80, 3000},
        {PA_CLOSING, "", 80, 3000},
        {PA_CLOSING_CURSOR, "", 80, 3000},
        {PA_SCROLL_UP_LEFT, "", 80, 3000},
        {PA_SCROLL_UP_RIGHT, "", 80, 3000},
        {PA_SCROLL_DOWN_LEFT, "", 80, 3000},
        {PA_SCROLL_DOWN_RIGHT, "", 80, 3000},
        {PA_GROW_UP, "", 80, 3000},
        {PA_GROW_DOWN, "", 80, 3000},
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
    timeClient.begin();
#if ENA_SPRITE
    P.setSpriteData(pacman1, W_PMAN1, F_PMAN1, pacman2, W_PMAN2, F_PMAN2);
#endif
    P.displayText("Digital clock by Joydada & Rohit", PA_CENTER, 25, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    while (!P.displayAnimate())
        ;
}

void loop()
{
    updateTimeDate();
    animateScreenAndShow();
}

void updateTimeDate()
{
    timeClient.update();
    epochTime = timeClient.getEpochTime();
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
        int currAnim = random(1, 29);
        P.displayText(getDataToShow(stage).c_str(), PA_CENTER, catalog[currAnim].speed, catalog[currAnim].pause, catalog[currAnim].effect, catalog[currAnim].effect);
        while (!P.displayAnimate());

        stage = stage - 1;
        if (stage <= 0)
        {
            stage = 0;
        };
    }
}

String getDataToShow(int currStage)
{
    switch (currStage)
    {
    case 9:
        return "1";
    case 8:
        return "1";
    case 7:
        return "1";
    case 6:
        return "1";
    case 5:
        return "1";
    case 4:
        return "1";
    case 3:
        return "1";
    case 2:
        return "1";
    case 1:
        return "1";
    default:
        return "1";
    }
}
