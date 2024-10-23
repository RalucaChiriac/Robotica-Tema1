#include <Arduino.h>

const int ledRGB_R = 6;
const int ledRGB_G = 5;
const int ledRGB_B = 4;
const int ledL1 = 10;
const int ledL2 = 9;
const int ledL3 = 8;
const int ledL4 = 7;
const int butonStart = 3;
const int butonStop = 2;

bool incarcareInProgres = false;
bool statieLibera = true; 
int nivelIncarcare = 0;

// Variabile pentru debounce la butonul de stop
unsigned long startPressTime = 0;
bool isPressingStop = false;
const unsigned long debounceDelay = 1000; // o secunda

unsigned long timpUltimaSchimbare = 0;
unsigned long intervalSchimbare = 2000;
unsigned long ledBlinkInterval = 500; // Intervalul de clipire al LED-urilor

// Timpuri pentru clipirea independentă a LED-urilor
unsigned long led1LastBlink = 0;
unsigned long led2LastBlink = 0;
unsigned long led3LastBlink = 0;
unsigned long led4LastBlink = 0;

void pornesteIncarcarea();
void opresteIncarcarea();
void finalizeazaIncarcarea();
void actualizeazaLEDuriIncarcare();
void resetareLEDuriIncarcare();

// Inițializarea pinilor
void setup() {
    pinMode(ledRGB_R, OUTPUT);
    pinMode(ledRGB_G, OUTPUT);
    pinMode(ledRGB_B, OUTPUT);
    pinMode(ledL1, OUTPUT);
    pinMode(ledL2, OUTPUT);
    pinMode(ledL3, OUTPUT);
    pinMode(ledL4, OUTPUT);
    pinMode(butonStart, INPUT_PULLUP);
    pinMode(butonStop, INPUT_PULLUP);

    digitalWrite(ledRGB_G, HIGH); // starea initiala a LED-ului de culoare verde care indica statia libera
}

void loop() {
    unsigned long timpulCurent = millis();

    // butonul start
    if (digitalRead(butonStart) == LOW && statieLibera) {
        pornesteIncarcarea();
    }

    // butonul stop
    if (digitalRead(butonStop) == LOW) {
        if (!isPressingStop) { // Se apasa butonul pentru prima data
            startPressTime = timpulCurent; // tinem minte momentul cand s-a apasat butonul
            isPressingStop = true; // setam flag-ul ca butonul este apasat
        } else if (timpulCurent - startPressTime >= debounceDelay && incarcareInProgres) {
            opresteIncarcarea(); // actionam oprirea fortata a flow-ului de incarcare
        }
    } else {
        isPressingStop = false; // se reia logica dupa ce se elibereaza butonul
    }

    // parcurgerea stadiilor de incarcare
    if (incarcareInProgres && timpulCurent - timpUltimaSchimbare >= intervalSchimbare) {
        timpUltimaSchimbare = timpulCurent;
        nivelIncarcare++;
        if (nivelIncarcare > 4) {
            finalizeazaIncarcarea();
        }
    }

    // daca incarcarea este in progres, actualizeaza LED-urile
    if (incarcareInProgres) {
        actualizeazaLEDuriIncarcare();
    }
}

// Resetează toate LED-urile
void resetareLEDuriIncarcare() {
  digitalWrite(ledL1, LOW);
  digitalWrite(ledL2, LOW);
  digitalWrite(ledL3, LOW);
  digitalWrite(ledL4, LOW);
}


void pornesteIncarcarea() {
    statieLibera = false;
    incarcareInProgres = true;
    nivelIncarcare = 1;
    timpUltimaSchimbare = millis();
    digitalWrite(ledRGB_G, LOW);
    digitalWrite(ledRGB_R, HIGH);
}

// Functia de oprire fortata a flow-ului de incarcare
void opresteIncarcarea() {
    incarcareInProgres = false;
    statieLibera = true;
    nivelIncarcare = 0;
    resetareLEDuriIncarcare();
    for (int i = 0; i < 3; i++) {
        digitalWrite(ledL1, HIGH);
        digitalWrite(ledL2, HIGH);
        digitalWrite(ledL3, HIGH);
        digitalWrite(ledL4, HIGH);
        delay(500);
        digitalWrite(ledL1, LOW);
        digitalWrite(ledL2, LOW);
        digitalWrite(ledL3, LOW);
        digitalWrite(ledL4, LOW);
        delay(500);
    }
    digitalWrite(ledRGB_R, LOW);
    digitalWrite(ledRGB_G, HIGH);
}

// Functia de oprire normala a flow-ului de incarcare
void finalizeazaIncarcarea() {
    incarcareInProgres = false;
    statieLibera = true;
    nivelIncarcare = 0;
    resetareLEDuriIncarcare();
    for (int i = 0; i < 4; i++) {
        digitalWrite(ledL1, HIGH);
        digitalWrite(ledL2, HIGH);
        digitalWrite(ledL3, HIGH);
        digitalWrite(ledL4, HIGH);
        delay(500);
        digitalWrite(ledL1, LOW);
        digitalWrite(ledL2, LOW);
        digitalWrite(ledL3, LOW);
        digitalWrite(ledL4, LOW);
        delay(500);
    }
    digitalWrite(ledRGB_R, LOW);
    digitalWrite(ledRGB_G, HIGH);
}

// Funcția pentru a actualiza LED-urile în funcție de stadiul de încărcare
void actualizeazaLEDuriIncarcare() {
    unsigned long timpulCurent = millis();

    if (nivelIncarcare >= 1 && timpulCurent - led1LastBlink >= ledBlinkInterval) {
        led1LastBlink = timpulCurent;
        digitalWrite(ledL1, !digitalRead(ledL1));
    }

    if (nivelIncarcare >= 2) {
        digitalWrite(ledL1, HIGH);  // Keep LED 1 ON
        if (timpulCurent - led2LastBlink >= ledBlinkInterval) {
            led2LastBlink = timpulCurent;
            digitalWrite(ledL2, !digitalRead(ledL2));
        }
    }

    if (nivelIncarcare >= 3) {
        digitalWrite(ledL1, HIGH);  
        digitalWrite(ledL2, HIGH);
        if (timpulCurent - led3LastBlink >= ledBlinkInterval) {
            led3LastBlink = timpulCurent;
            digitalWrite(ledL3, !digitalRead(ledL3));
        }
    }

    if (nivelIncarcare >= 4) {
        digitalWrite(ledL1, HIGH);
        digitalWrite(ledL2, HIGH);
        digitalWrite(ledL3, HIGH);
        if (timpulCurent - led4LastBlink >= ledBlinkInterval) {
            led4LastBlink = timpulCurent;
            digitalWrite(ledL4, !digitalRead(ledL4));
        }
    }
}
