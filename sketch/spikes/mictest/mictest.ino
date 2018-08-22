
int Analog_Eingang = A5;

void setup ()
{
 pinMode(Analog_Eingang, INPUT);

 Serial.begin(9600);
}

void loop ()
{
   float analog;
  
   analog = analogRead(Analog_Eingang);
   
   Serial.print(analog);
   Serial.print(" (");
  
   int lvl = analog / 64;
  
   for(int i=0; i<=lvl; i++)
   {
      Serial.print("#");
   }
   
   Serial.println();   
}
