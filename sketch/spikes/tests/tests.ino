void setup()
{
  Serial.begin(9600);  
}

void loop() {
  int x1 = m(-2,16);
  int x2 = m(-1,16);
  int x3 = m(0,16);
  int x4 = m(1,16);
   
  Serial.print(x1);
  Serial.print(" ");
  Serial.print(x2);
  Serial.print(" ");
  Serial.print(x3);
  Serial.print(" ");
  Serial.print(x4);
  Serial.println(" ");
  delay(1000);
}

int m(int a, int b)
{
  int c = a % b;
  if (c < 0)  { c += b; }
  return c;
}
