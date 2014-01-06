
int pins1[4] = {43, 41, 39, 37};
int pins2[4] = {36, 38, 40, 42};
int pins3[4] = {44, 46, 47, 45};

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  Serial.print("{");
  for(int i=0; i<4; ++i) {
    Serial.print(digitalRead(pins1[i]));
    if(i!=3) { Serial.print(","); }
  }
  Serial.print("} {");
  for(int i=0; i<4; ++i) {
    Serial.print(digitalRead(pins2[i]));
    if(i!=3) { Serial.print(","); }
  }
  Serial.print("} {");
  for(int i=0; i<4; ++i) {
    Serial.print(digitalRead(pins3[i]));
    if(i!=3) { Serial.print(","); }
  }
  Serial.println("}");
}
