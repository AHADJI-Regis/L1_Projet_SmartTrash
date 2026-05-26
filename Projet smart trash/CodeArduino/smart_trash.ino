/************************************************
 * SMART BIN + LEDS + TTN ABP
 ************************************************/

#define CFG_EU 1

#include <SPI.h>
#include <lmic.h>
#include <hal/hal.h>
#include <FastLED.h>

#define LED_PIN       4
#define NUM_LEDS      21
#define BRIGHTNESS    64
#define LED_TYPE      WS2811
#define COLOR_ORDER   GRB

CRGB leds[NUM_LEDS];

const int trigPin = A3;
const int echoPin = A2;

const float DISTANCE_VIDE  = 35.0;
const float DISTANCE_PLEIN = 10.0;

static const u4_t DEVADDR = 0x260B37CA;

static const PROGMEM u1_t NWKSKEY[16] = {
  0xEB, 0xF2, 0x50, 0x2E,
  0x60, 0xBE, 0xED, 0x10,
  0x05, 0xCF, 0xAE, 0xB0,
  0x65, 0x0E, 0x6F, 0x3B
};

static const u1_t PROGMEM APPSKEY[16] = {
  0x27, 0xB3, 0x24, 0xE4,
  0x61, 0x8F, 0xF2, 0xD0,
  0x8A, 0x11, 0x25, 0x30,
  0x39, 0xA5, 0xCB, 0x48
};

void os_getArtEui (u1_t* buf) {}
void os_getDevEui (u1_t* buf) {}
void os_getDevKey (u1_t* buf) {}

static osjob_t sendjob;
const unsigned TX_INTERVAL = 30;

const lmic_pinmap lmic_pins = {
  .nss  = 10,
  .rxtx = LMIC_UNUSED_PIN,
  .rst  = 8,
  .dio  = {6, 7, LMIC_UNUSED_PIN},
};

// Variable globale — dernière distance valide
float derniereDistance = DISTANCE_VIDE;

// ======================================================
// MESURE DISTANCE
// ======================================================

float lireDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return -1;

  float d = (duration * 0.0343) / 2.0;
  if (d < 2.0 || d > 400.0) return -1;
  return d;
}

// ======================================================
// CALCUL NIVEAU
// ======================================================

int calculerNiveau(float distance) {
  if (distance >= DISTANCE_VIDE)  return 0;
  if (distance <= DISTANCE_PLEIN) return 100;
  int n = (int)((DISTANCE_VIDE - distance) / (DISTANCE_VIDE - DISTANCE_PLEIN) * 100);
  return constrain(n, 0, 100);
}

// ======================================================
// LEDS
// ======================================================

void gererLEDs(int niveau) {
  if (niveau >= 90) {
    fill_solid(leds, NUM_LEDS, CRGB::Red);
  } else if (niveau >= 50) {
    fill_solid(leds, NUM_LEDS, CRGB(255, 140, 0));
  } else {
    fill_solid(leds, NUM_LEDS, CRGB::Green);
  }
  FastLED.show();
}

// ======================================================
// EVENEMENTS TTN
// ======================================================

void onEvent(ev_t ev) {
  switch(ev) {
    case EV_TXCOMPLETE:
      Serial.println("Message envoye vers TTN");
      if (LMIC.txrxFlags & TXRX_ACK) {
        Serial.println("ACK recu");
      }
      os_setTimedCallback(
        &sendjob,
        os_getTime() + sec2osticks(TX_INTERVAL),
        do_send
      );
      break;
    default:
      break;
  }
}

// ======================================================
// ENVOI TTN
// ======================================================

void do_send(osjob_t* j) {
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println("Transmission en cours");
    return;
  }

  // Utilise la dernière distance valide stockée par le loop
  float distance = derniereDistance;
  int niveau = calculerNiveau(distance);

  Serial.println("====================");
  Serial.print("Distance : ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.print("Niveau   : ");
  Serial.print(niveau);
  Serial.println(" %");

  uint16_t dist = (uint16_t)(distance * 100);
  byte payload[3];
  payload[0] = highByte(dist);
  payload[1] = lowByte(dist);
  payload[2] = (uint8_t)niveau;

  LMIC_setTxData2(1, payload, sizeof(payload), 0);
  Serial.println("Payload envoye");
}

// ======================================================
// SETUP
// ======================================================

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println("SMART BIN TTN ABP");

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  fill_solid(leds, NUM_LEDS, CRGB::Blue);
  FastLED.show();
  delay(2000);

  os_init();
  LMIC_reset();

  // Clock error plus grand pour tolérer les delays du loop
  LMIC_setClockError(MAX_CLOCK_ERROR * 10 / 100);

  #ifdef PROGMEM
    uint8_t appskey[sizeof(APPSKEY)];
    uint8_t nwkskey[sizeof(NWKSKEY)];
    memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
    memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
    LMIC_setSession(0x1, DEVADDR, nwkskey, appskey);
  #else
    LMIC_setSession(0x1, DEVADDR, NWKSKEY, APPSKEY);
  #endif

  LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);
  LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);
  LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7), BAND_CENTI);

  LMIC_setLinkCheckMode(0);
  LMIC.dn2Dr = DR_SF9;
  LMIC_setDrTxpow(DR_SF7, 14);

  do_send(&sendjob);
}

// ======================================================
// LOOP 
// ======================================================

void loop() {

  // Mesure distance
  float d = lireDistance();

  // On garde seulement les valeurs valides
  if (d > 0) {
    derniereDistance = d;
  }

  // LEDs basées sur dernière valeur valide
  int niveau = calculerNiveau(derniereDistance);
  gererLEDs(niveau);

  // LoRaWAN
  os_runloop_once();

  // Délai
  delay(100);
}
