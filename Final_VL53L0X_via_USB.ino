// PROGRAM FINAL CITIRE SENZOR VL530X
// 18,03,2021
// transmisie pachet de date pe serial USB

#include <Wire.h>                     // Definire biblioteca I2C 
#include <VL53L0X.h>                  // Definire biblioteca senzor VL53L0X
VL53L0X sensor;
#define HIGH_SPEED                    // Definire mod de functionare senzor
byte cond= 2;                         // Definire bit pentru conditionare citire distanta
String mesaj;
byte trig3 = 6;                       // Pinul pentru setare timp de sincronizare
float tsin=0;                         // variabila pentru stocarea tipului de referinta pentru sincronizare
bool fa=false;                        // fanion pentru sincronizare
//...................................................................................................................................
int ni=18;                            // Numar Iterati dorite
int i;                                // Contor pentru completarea tabloului
int j;                                // Contor pentru transmitrea datelor
int A[20];                            // Definire tablou pentru stocarea valoriilor de distanta
int B[20];                            // Definire tablou pentru stocarea valoriilor de
bool buffa=false;                     // Buffer activ
//________________________________________________________________________________________________________SETUP______________________
void setup()
{
  Serial.begin(250000);               // Initializare comunicare USB
  Wire.begin();                       // Initializare comunicare I2C
  pinMode(cond,INPUT);                // Setare mod pin conditionare
  pinMode (trig3, INPUT);
  //_____________________________________________________________________
  sensor.setTimeout(500);                        //                      |
  if (!sensor.init())                            //                      |
  {                                              //                      |
    Serial.println("Failed to detect and initialize sensor!");//         |
    while (1) {}                                              //         |
  }                                                           //         |>>>>>>>Setare conditi de functionare pentru senzor
  #if defined HIGH_SPEED                                      //         |
  // reduce timing budget to 20 ms (default is about 33 ms)   //         |
  sensor.setMeasurementTimingBudget(20000);                   //         |
#endif                                                        //         |
  //_____________________________________________________________________|
}
//________________________________________________________________________________________________________LOOP_______________________
void loop() 
{
if (digitalRead(trig3)==HIGH)
      {
        if (fa==false)
        {
          tsin=(millis());                           // inregistreaza valoarea timpului ,,zero,,
          String ts = String(tsin);                  // Transforma valoarea timpuli in string
          mesaj = String("tsincron="+ts+",");            // Se atribuie
          fa=true;
        }
         
      }
  //
if (digitalRead(cond)== HIGH)
  {
   for(i=0;i<ni;i++)
     {
      A[i]=(sensor.readRangeSingleMillimeters());
      B[i]=(abs(millis()));
     }
   buffa=true;                                            // Creaza conditia pentru citire buffer activ
  }

if (buffa==true)                                         // Verificare daca bufferul este activ
    {
      for (j=0;j<ni;j++)
        {
          String St = String(A[j]); 
          String Stc = String("d"+ St+",");
          mesaj = String (mesaj + Stc);
          St = String(B[j]); 
          Stc = String("t"+ St+",");
          mesaj = String(mesaj + Stc);
         
        }
      i=0;                                                // Resetare contor i
      j=0;                                                // Resetare contor j
      buffa=false;                                        // Dezactiveaza bufferul
      Serial.print(mesaj);
      Serial.println();
      mesaj = ("S/");
     // delay(5);
      //Serial.print("Sta/");
      //Serial.println();
     
    }

  

}
