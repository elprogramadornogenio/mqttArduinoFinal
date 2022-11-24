#include <SPI.h>
#include <FastLED.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>



/***************VALORES DE HARDWARE********************/
#define MQ7Pin                  (A0)
#define RL_VALOR_MQ7            (1)
#define RO_AIRE_LIMPIO_MQ7      (25.93)
/***************VALORES DE SOFTWARE********************/
#define CALIBRACION_TIEMPO      (50)
#define CALIBRACION_INTERVALOS  (500)

#define LECTURA_TIEMPO          (5)
#define LECTURA_INTERVALOS      (50)

/***************VARIABLES GLOBALES**********************/
float Ro = 10;
int ppm = 0;

//LED MATRIX RGB
#define LED_PIN     7
#define NUM_LEDS    64
#define BRIGHTNESS  10
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
//BUZZER
int buzzer = 6;
//LCD I2C
LiquidCrystal_I2C lcd(0x27,16,2);

void setup() {
    Serial.begin(115200);
    //LED MATRIX RGB
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(  BRIGHTNESS );
    //BUZZER
    pinMode(buzzer, OUTPUT);
    //LCD I2C
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0,0);
    //SENSOR MQ7
    Serial.print("Calibrando... \n");
    lcd.print("Calibrando...");
    Ro = MQCalibracion(MQ7Pin);
    lcd.clear();
    Serial.print("Calibracion ha terminado.... \n");
    Serial.print("Ro=");
    Serial.print(Ro);
    Serial.println(" Kohm");
}


void loop()
{
  Serial.print("Ratio: ");
  Serial.print(LecturaMQ(MQ7Pin)/Ro);
  Serial.print(" ");
  Serial.print("Monoxido de carbono: ");
  ppm = GetMQ(LecturaMQ(MQ7Pin)/Ro);
  Serial.print(ppm);
  Serial.println(" ppm");
  //LCD I2C////////////////////
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CO: ");
  lcd.print(ppm);
  lcd.print(" ppm");
  Serial.write(ppm);
  if(ppm>=0 && ppm <=9) {
    //Rojo
    Color(0,255,0);
    FastLED.delay(2000);
    FastLED.clear();
    
  } else if (ppm > 9 && ppm <= 35) {

    //Amarillo
    Color(255,255,0);
    FastLED.delay(2000);
    FastLED.clear();
    
  } else if (ppm > 35 && ppm <= 800) {
    tone(buzzer,1535,500);
    //Rojo
    Color(255,0,0);
    FastLED.delay(2000);
    FastLED.clear();
  } else {
    tone(buzzer,1535,500);
    //Rojo
    Color(255,0,0);
    FastLED.delay(1000);
    FastLED.clear();
  }
  delay (200);

}

///////////////////////////////////////////////////////////////////////

void Color(int R, int G, int B){
  for(int i=0; i<64; i++) {
    leds[i] = CRGB(R, G, B);
  }
  FastLED.show();
}

///////////////////////////////////////////////////////////////////////
float ResistenciaCalculoMQ (int adc){
  return ((float)(RL_VALOR_MQ7*(1023-adc)/adc));
}

float MQCalibracion (int mq_pin){
  
  float RS_AIR=0,r0;
  for (int i = 0; i<CALIBRACION_TIEMPO; i++){
    RS_AIR += ResistenciaCalculoMQ(analogRead(mq_pin));
    delay(CALIBRACION_INTERVALOS);
  }
  RS_AIR = RS_AIR/CALIBRACION_TIEMPO;
  r0 = RS_AIR/RO_AIRE_LIMPIO_MQ7;
  return r0; 
}

float LecturaMQ (int mq_pin){
  float RS = 0;
  for (int i=0; i<LECTURA_TIEMPO; i++){
    RS += ResistenciaCalculoMQ(analogRead(mq_pin));
    delay(LECTURA_INTERVALOS);
  }
  RS = RS/LECTURA_TIEMPO;
  return RS;
}

int GetMQ (float ratio) {
  return (pow(10,((-1.523*(log10(ratio)))+1,99)));  
}
