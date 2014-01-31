
const int PIN_NUM = 2;
//int pins1[4] = {43, 41, 39, 37};
//int pins2[4] = {36, 38, 40, 42};
//int pins3[4] = {44, 46, 47, 45};
const int LIGHT_PIN[3][2] = {{9, 10}, {11, 12}, {13, 14}};

bool is_on_line[3] = {false};
int light_reference[3][2] = {0};

void read_light() {
    for(int i=0; i<3; ++i) {
        is_on_line[i] = false;
        for(int j=0; j<2; ++j) {
            const int tmp = analogRead(LIGHT_PIN[i][j]);
            if(tmp - light_reference[i][j] > 150) {
                is_on_line[i] = true;
            }
        }
    } 
}

void setup() {
    Serial.begin(9600);
    for(int i=0; i<3; ++i) {
        for(int j=0; j<2; ++j) {
            pinMode(LIGHT_PIN[i][j], INPUT);
        }
    }
    delay(100);
    for(int i=0; i<3; ++i) {
        for(int j=0; j<2; ++j) {
            light_reference[i][j] = analogRead(LIGHT_PIN[i][j]);
        }
    }
}

void loop() {
    read_light();
    for(int i=0; i<3; ++i) {
        Serial.print(is_on_line[i]);
        Serial.print(" ");
    }
    Serial.println();
}
