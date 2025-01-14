#include <Arduino.h>

#include <SPI.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

#include <ACS712.h>

SoftwareSerial mySerial(9, 10);

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

ACS712 sensor(ACS712_20A, A1);

float readVal1 = 0; // for voltage
float readVal2 = 0; // for current
float voltage = 0.0;
String user = "test1";
float currentValue = 0;

float avgamps = 0;
float amphr = 0;

float power = 0.0;
float energy = 0.0;
int send_flag = 1;
//int mVperAmp = 186;

int i = 0;
int j = 0;
int k = 0;
int delayTime2 = 350; // Delay between shifts
bool p13 = true;
int bill_time = 0;

void SendMessage()
{
    int bill_send = 0;
    if (bill_time % 3 == 0)
        bill_send = 1;
    mySerial.println("AT+CMGF=1");                   //Sets the GSM Module in Text Mode
    delay(1000);                                     // Delay of 1000 milli seconds or 1 second
    mySerial.println("AT+CMGS=\"+919537262339\"\r"); // Replace x with mobile number
    delay(1000);
    mySerial.println("start");
    mySerial.println(user);
    mySerial.println(voltage);      // The SMS text you want to send
    mySerial.println(currentValue); // The SMS text you want to send
    mySerial.println(power);        // The SMS text you want to send
    mySerial.println(energy);       // The SMS text you want to send
    mySerial.println(bill_send);
    mySerial.println(int(p13));
    delay(100);
    mySerial.println((char)26); // ASCII code of CTRL+Z
    delay(1000);
    bill_time += 1;
}

void RecieveMessage()
{
    mySerial.println("AT+CNMI=2,2,0,0,0"); // AT Command to receive a live SMS
    delay(3000);
}

void check_messages()
{
    Serial.println("IN");
    String c;
    delay(500);
    while (mySerial.available() > 0)
    {
        c = mySerial.readString();
        Serial.print(c);
        // c.indexOf("AT+CMGS=\"+919537262339\"") > 0 ||
        if (c.indexOf("+CMT") > 0 || c.indexOf("toggle") > 0)
        {
            Serial.println("\nToggle time");
            if (p13)
            {
                digitalWrite(13, LOW);
                p13 = false;
                lcd.setCursor(1, 1);
                lcd.print("POW.STATUS:");
                lcd.print("OFF");
            }
            else
            {
                digitalWrite(13, HIGH);
                p13 = true;
                lcd.setCursor(1, 1);
                lcd.print("POW.STATUS:");
                lcd.print("ONN");
            }
        }
    }
    Serial.println("Out");
}
void cleanSerial()
{
    while (mySerial.available() > 0)
        mySerial.read();
    Serial.println("Serial Cleaned");
}

void setup()
{

    sensor.calibrate();
    mySerial.begin(15200); // Setting the baud rate of GSM Module
    Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
    Serial.println("Starting test ...");
    lcd.begin(16, 2);
    lcd.clear();
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
}

void loop()
{

    float currentValue = sensorReading();
    lcd.setCursor(16, 1); // set the cursor outside the display count
    lcd.print(" ");       // print empty character

    delay(600);

    //long milisec = millis(); // calculate time in milliseconds
    //long time= -(milisec/1000); // convert milliseconds to seconds
    long time = 60;

    int readVal1 = analogRead(A0);
    Serial.print(readVal1);
    voltage = (readVal1) * (0.228);

    Serial.print("AC Voltage: ");
    Serial.print(voltage);
    Serial.println(" Volts");
    //delay(1000);

    //Variable = getVPP();
    //VRMS = (Variable/2.0) *0.707;  //root 2 is 0.707
    //AmpsRMS = (VRMS * 1000)/mVperAmp;
    //Serial.print(AmpsRMS);
    //Serial.println(" Amps RMS");

    //amps = actualval * 10;                    // 100mv/A from data sheet
    //totamps = totamps + amps;                // total amps
    //avgamps = totamps / time;                // average amps
    //amphr = (avgamps * time) / 3600;        // amphour
    //watt = voltage * amps;                    // power=voltage*current
    //energy = (watt * time) / (1000 * 3600); // energy in kWh

    avgamps = currentValue / time; // average amps
    //amphr=(avgamps*time)/3600; // amp-hour
    power = (voltage * currentValue);
    //energy=(watt*time)/3600; Watt-sec is again convert to Watt-Hr by dividing 1hr(3600sec)
    energy = (power) / (1000); //for reading in kWh

    lcd.setCursor(1, 0); // set the cursor at 1st col and 1st row
    lcd.print(energy);
    lcd.print("KWm");

    //lcd.print(voltage );
    //lcd.print("v");

    lcd.setCursor(1, 1);

    lcd.print("POW.STATUS:");
    lcd.print("ONN");

    /*
      mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
      delay(1000);  // Delay of 1000 milli seconds or 1 second
      mySerial.println("AT+CMGS=\"+919428217561\"\r"); // Replace x with mobile number
      delay(1000);
      //mySerial.print(watt, voltage, energy, amps);
      mySerial.println(watt);
      mySerial.println(voltage);
      mySerial.println(energy);
      mySerial.println(amps);
      //mySerial.println("sim900a sms");// The SMS text you want to send
      delay(100);
      mySerial.println((char)26);// ASCII code of CTRL+Z
      //delay(1000);
      delay(60000);
      */
    Serial.println(send_flag);
    if (send_flag % 2 == 0)
    {
        send_flag = 1;
        Serial.println("Sending");
        SendMessage();
        Serial.println("Sent");
        delay(500);
    }
    else
    {
        send_flag += 1;
    }
    check_messages();
    RecieveMessage();
    check_messages();
    delay(3000);
}

float sensorReading()
{
    float I = 0;
    for (int i = 0; i < 10; i++)
    {
        I = I + sensor.getCurrentAC();
    }
    return (I / 10);
}
