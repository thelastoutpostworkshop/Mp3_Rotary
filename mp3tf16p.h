#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#define MP3_ERROR_ONLY 1
#define MP3_ALL_MESSAGE 2

class MP3Player
{
private:
    SoftwareSerial *mySoftwareSerial;
    void statusOnSerial(uint8_t type, int value);
    void waitPlayIsTerminated(void);
    int p_RX;
    int p_TX;

public:
    DFRobotDFPlayerMini player;
    MP3Player(int RX, int TX);
    ~MP3Player();
    void playTrackNumber(int trackNumber, int volume, boolean waitPlayTerminated = true);
    boolean playCompleted(void);
    void initialize(void);
    int serialPrintStatus(int errorOnly);
};

MP3Player::MP3Player(int RX, int TX)
{
    p_TX = TX;
    p_RX = RX;
}

MP3Player::~MP3Player()
{
}

void MP3Player::initialize(void)
{
    mySoftwareSerial = new SoftwareSerial(p_RX, p_TX);

    mySoftwareSerial->begin(9600);
    Serial.println(F("Initializing MP3Player ..."));

    if (!player.begin(*mySoftwareSerial,true,false))
    {
        Serial.println(F("Unable to begin:"));
        Serial.println(F("1.Please recheck the connection!"));
        Serial.println(F("2.Please insert the SD card!"));
        while (true)
            ;
    }
    player.volume(10);
    Serial.println(F("MP3Player online."));
}

void MP3Player::playTrackNumber(int trackNumber, int volume, boolean waitPlayTerminated)
{
    player.volume(volume);
    player.play(trackNumber);
    if (waitPlayTerminated)
    {
        waitPlayIsTerminated();
    }
}

void MP3Player::waitPlayIsTerminated(void)
{
    while (!playCompleted())
    {
    }
}

boolean MP3Player::playCompleted(void)
{
    if (player.available())
    {
        return player.readType() == DFPlayerPlayFinished;
    }
    return false;
}

// Print the detail message from DFPlayer to handle different errors and states.
// 
int MP3Player::serialPrintStatus(int verbose)
{
    if (player.available())
    {
        uint8_t type = player.readType();
        int value = player.read();
        if (verbose == MP3_ERROR_ONLY)
        {
            if (type == DFPlayerError)
            {
                statusOnSerial(type, value);
            }
        }
        else
        {
            statusOnSerial(type, value);
        }
        if(type == DFPlayerError) {
            return value;
        } else {
            return 0;
        }
    }
}

void MP3Player::statusOnSerial(uint8_t type, int value)
{
    switch (type)
    {
    case TimeOut:
        Serial.println(F("Time Out!"));
        break;
    case WrongStack:
        Serial.println(F("Stack Wrong!"));
        break;
    case DFPlayerCardInserted:
        Serial.println(F("Card Inserted!"));
        break;
    case DFPlayerCardRemoved:
        Serial.println(F("Card Removed!"));
        break;
    case DFPlayerCardOnline:
        Serial.println(F("Card Online!"));
        break;
    case DFPlayerPlayFinished:
        Serial.print(F("Number:"));
        Serial.print(value);
        Serial.println(F(" Play Finished!"));
        break;
    case DFPlayerError:
        Serial.print(F("DFPlayerError:"));
        switch (value)
        {
        case Busy:
            Serial.println(F("Card not found"));
            break;
        case Sleeping:
            Serial.println(F("Sleeping"));
            break;
        case SerialWrongStack:
            Serial.println(F("Get Wrong Stack"));
            break;
        case CheckSumNotMatch:
            Serial.println(F("Check Sum Not Match"));
            break;
        case FileIndexOut:
            Serial.println(F("File Index Out of Bound"));
            break;
        case FileMismatch:
            Serial.println(F("Cannot Find File"));
            break;
        case Advertise:
            Serial.println(F("In Advertise"));
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}
