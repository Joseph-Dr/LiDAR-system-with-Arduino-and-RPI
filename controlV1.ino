// Program pentru control motor dc
// Driver DRV8833
// 10.04.2021.
byte in1 = 2;              // Se declara pinul pentru intrare start.
byte m11 = 5;              // Se declara pinul 1 pentru motor 1
byte m12 = 6;              // Se declara pinul 2 pentru motor 2
byte en = 7;               // Se declara pinul pentru coditionare driver
int MAX_PWM_VOLTAGE = 40;
void setup() 
{
Serial.begin(9600);
pinMode(in1,INPUT);
pinMode(m11,OUTPUT);
pinMode(m12,OUTPUT);
digitalWrite(m11, LOW);
digitalWrite(m12,LOW);
pinMode(en, OUTPUT);


}

void loop() 
{
  if (digitalRead(in1)==HIGH)
  {
    Serial.print("START");
    digitalWrite(m12, LOW);
    analogWrite(m11, MAX_PWM_VOLTAGE);
    digitalWrite(en, HIGH); 
    delay(1000);
  }
  else
  {
    digitalWrite(m12,LOW);
    digitalWrite(m12,LOW);
    digitalWrite(en, LOW);
    Serial.print("Stop");
    delay(5);
  }

}
