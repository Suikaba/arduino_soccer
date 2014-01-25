
const int SWITCH1_PIN = 48;
const int SWITCH2_PIN = 50;

void setup() {
    Serial.begin(9600);
    pinMode(SWITCH1_PIN, INPUT);
    pinMode(SWITCH2_PIN, INPUT);
}

void loop() {
    int switch1 = digitalRead(SWITCH1_PIN);
    int switch2 = digitalRead(SWITCH2_PIN);
    Serial.print("SW1:");
    Serial.print(switch1);
    Serial.print(" SW2:");
    Serial.println(switch2);
    delay(200);
}
