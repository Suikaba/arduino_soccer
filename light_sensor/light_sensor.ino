
const int PIN_NUM = 2;
//int pins1[4] = {43, 41, 39, 37};
//int pins2[4] = {36, 38, 40, 42};
//int pins3[4] = {44, 46, 47, 45};
int pins1[PIN_NUM] = {9, 10};
int pins2[PIN_NUM] = {11, 12};
int pins3[PIN_NUM] = {13, 14};

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    Serial.print("{");
    for(int i=0; i<PIN_NUM; ++i) {
        Serial.print(analogRead(pins1[i]));
        if(i!=1) { Serial.print(","); }
    }
    Serial.print("} {");
    for(int i=0; i<PIN_NUM; ++i) {
        Serial.print(analogRead(pins2[i]));
        if(i!=1) { Serial.print(","); }
    }
    Serial.print("} {");
    for(int i=0; i<PIN_NUM; ++i) {
        Serial.print(analogRead(pins3[i]));
        if(i!=1) { Serial.print(","); }
    }
    Serial.println("}");
}
