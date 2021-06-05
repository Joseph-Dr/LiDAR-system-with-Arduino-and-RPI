
//02.20.2021.
// program care citeste encoderul cu 2 canale
// transforma in grade
// include timpul
// Output pentru start masurare
// Comunicare prin i2c catre raspberry
byte trig  = 2;             // Pinul pentru trigger
byte trig2 = 5;             // Pinul pentru start citire distanta
byte trig3 = 6;             // Pinul pentru setare timp de sincronizare
byte chA = 3;               // Pinul pentru canal A
//byte chB = 4;               // Pinul pentru canal B++++++++++++++++++
bool fa=false;
int ni=12;                  // Numar iteratii
int i=0;                    // Contor pentru iteratii
int inp = 0;                // Contorul de impuls
String mesaj;
bool lastA = false;         // Flag pentru starea anterioara canal A
bool lastB = false;         // Flag pentru starea anterioara canal B
bool pdp = false;           // pachet de date pregatit de trimis
int A[20];                  // Tablou pentru unghi
int B[20];                  // Tablou pentru timp
float u=0;                  // variabila pentru stocarea unghiului curent
float tsin=0;               // variabila pentru stocarea tipului de referinta pentru sincronizare
int U1=0;                   // Valoarea unghiului de unde incepe citirea distantei si completarea tablului cu valoarea unghiului si a timpului
int U2=45;                  // Valoarea unghiului pana la care se face citirea
float cu=3.6;               // Constanta care tetermina rezolutia encoderului (grade/inpuls)
void setup()
{
  pinMode (chA, INPUT);
  // pinMode (chB, INPUT);
  pinMode (trig, INPUT);
  pinMode (trig2, OUTPUT);
  pinMode (trig3, INPUT);
  Serial.begin (250000); 

}
////////////////////////////loop/////////////////////////////////////////////////////////////////////////////
void loop() 
   {
    if (digitalRead(trig3)==HIGH)
      {
        if (fa==false)
        {
          tsin=(millis());                           // inregistreaza valoarea timpului ,,zero,,
          String ts = String(tsin);                  // Transforma valoarea timpuli in string
          mesaj = String("tsincron="+ts);            // Se atribuie 
          fa=true;
        }
        
      }
    if (pdp==false)                     // conditie daca pachetul de date a fost trimis
      {
        if (digitalRead(trig)==HIGH)    // conditie daca exista triger pentru inregistararea unghiului
          {
            digitalWrite(trig2, HIGH);
           for (i=0;i<ni;) //=====================bucla care ruleaza cu maxima viteza========================================================================================
             { 
              //************************************************  CANAL  -A- detectie front******************************************
              if ((digitalRead(chA) == HIGH ) && (lastA==false))
                {
                 A[i]=i;
                 B[i]=(millis());
                 lastA = true;
                 i++;
                }
             
              if ((digitalRead(chA)==LOW) && (lastA==true))  // resetare canal A
                {
                  lastA=false;
                }
          
             }
            
          }
          if (i>8)
            {
              pdp=true;                     // Pachetul de date este pregatit de transmisie
              digitalWrite(trig2, LOW);     // Inceteaza citirea
            }
      }
       //===============================================================================================================================================================
    if(pdp==true)                            // daca sa parcurs bucla complet pentru registrare unghi
      {
        i=0;                                 // Reseteaza contorul
        
        for (i=0; i<ni; i++)                 // parcurge tabloul completat
           {
            u=cu*A[i];                       // Calculeaza valoarea unghiului
            A[i]=0;                          // completeaza tabelul cu valori nule
            String St = String (u);          // Transofma valoarea unghiului in string
            String Stc= String ("u"+St);     // Concateneaza sirul de caractere
            mesaj = String(mesaj +Stc);
            St = String(B[i]); 
            B[i]=0;                          // completeaza tabelul cu valori nule
            Stc = String("t"+ St);
            mesaj = String(mesaj + Stc);
           }
           i=0;
           
           Serial.print(mesaj);
           Serial.println();
           mesaj=("Start/"); 
         
     // Resetare continut mesaj / pregatire cuvant cheie pentru un mesaj nou
        pdp=false;                           // pachetul de date a fost trimis
     }   
              
 
  }       //--loop
  
