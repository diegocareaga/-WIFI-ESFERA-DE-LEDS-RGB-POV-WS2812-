#include <Arduino.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <Adafruit_NeoPixel.h>

// Pines utilizados
#define PIN        4 
#define SENSOR     14

//Cantidad de leds
#define NUMPIXELS 31

//Para hacer pausas en el program, no lo ocupe al final
#define delay_1    900

// Variables y constantes

const char* ssid = "NAME";        //Remplaza con EL NOMBRE DE TU RED WIFI
const char* password = "PASSWORD";  //Remplaza con LA CONTRASENA DE TU RED WIFI

volatile boolean sensorStatus = false;
String mensaje = "hola";

//Puerto para servidor que crea el Modulo de WI-FI
AsyncWebServer server(80);

//Si la URL esta mal, se ejecuta esta funcion
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

/*
 * Aqui estan algunos caracteres del ascii, ustedes pueden agregar mas con el Excel de este proyecto
 */
int ascii[135][11]  = {
    {1,1,1,1,1,1,1,1,1,0,0},{0,0,0,0,1,0,0,0,0,1,1},{0,0,0,0,1,0,0,0,0,1,1},{0,0,0,0,1,0,0,0,0,1,1},{1,1,1,1,1,1,1,1,1,0,0},  //A 0-4
    {1,1,1,1,1,1,1,1,1,1,1},{1,0,0,0,0,1,0,0,0,0,1},{1,0,0,0,0,1,0,0,0,0,1},{1,0,0,0,0,1,0,0,0,0,1},{0,1,1,1,1,0,1,1,1,1,0},  //B 5-9
    {0,1,1,1,1,1,1,1,1,1,0},{1,1,0,0,0,0,0,0,0,1,1},{1,1,0,0,0,0,0,0,0,1,1},{1,1,0,0,0,0,0,0,0,1,1},{0,0,0,0,0,0,0,0,0,0,0},  //C 10-14
    {1,1,1,1,1,1,1,1,1,1,1},{1,1,0,0,0,0,0,0,0,1,1},{1,1,0,0,0,0,0,0,0,1,1},{0,1,0,0,0,0,0,0,0,1,0},{0,0,1,1,1,1,1,1,1,0,0},  //D 15-19
    {1,1,1,1,1,1,1,1,1,1,1},{1,1,0,0,0,1,1,0,0,1,1},{1,1,0,0,0,1,1,0,0,1,1},{1,1,0,0,0,0,0,0,0,1,1},{0,0,0,0,0,0,0,0,0,0,0},  //E 20-24
    {1,1,1,1,1,1,1,1,1,1,1},{0,0,0,0,0,1,1,0,0,1,1},{0,0,0,0,0,1,1,0,0,1,1},{0,0,0,0,0,0,0,0,0,1,1},{0,0,0,0,0,0,0,0,0,0,0},  //F 25-29
    {1,1,1,1,1,1,1,1,1,1,1},{1,0,0,0,0,0,0,0,0,1,1},{1,0,0,0,1,0,0,0,0,1,1},{1,0,0,0,1,0,0,0,0,1,1},{1,1,1,1,1,0,0,0,0,1,1},  //G 30-34
    {1,1,1,1,1,1,1,1,1,1,1},{0,0,0,0,1,1,0,0,0,0,0},{0,0,0,0,1,1,0,0,0,0,0},{0,0,0,0,1,1,0,0,0,0,0},{1,1,1,1,1,1,1,1,1,1,1},  //H 35-39
    {1,0,0,0,0,0,0,0,0,0,1},{1,0,0,0,0,0,0,0,0,0,1},{1,1,1,1,1,1,1,1,1,1,1},{1,0,0,0,0,0,0,0,0,0,1},{1,0,0,0,0,0,0,0,0,0,1},  //I 40-44
    {1,1,0,0,0,0,0,0,0,0,1},{1,0,0,0,0,0,0,0,0,0,1},{1,1,1,1,1,1,1,1,1,1,1},{0,0,0,0,0,0,0,0,0,0,1},{0,0,0,0,0,0,0,0,0,0,1},  //J 45-49
    {1,1,1,1,1,1,1,1,1,1,1},{0,0,0,0,0,1,1,0,0,0,0},{0,0,0,0,1,1,1,0,0,0,0},{0,0,0,1,1,0,0,1,1,0,0},{1,1,1,1,0,0,0,0,1,1,1},  //K 50-54
    {1,1,1,1,1,1,1,1,1,1,1},{1,0,0,0,0,0,0,0,0,0,0},{1,0,0,0,0,0,0,0,0,0,0},{1,1,1,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0},  //L 55-59
    {1,1,1,1,1,1,1,1,1,1,1},{0,0,0,0,0,0,0,0,1,1,0},{0,0,0,0,0,0,1,1,0,0,0},{0,0,0,0,0,0,0,0,1,1,0},{1,1,1,1,1,1,1,1,1,1,1},  //M 60-64
    {1,1,1,1,1,1,1,1,1,1,1},{0,0,0,0,0,0,0,0,1,1,0},{0,0,0,0,0,0,1,1,0,0,0},{0,0,0,0,1,1,0,0,0,0,0},{1,1,1,1,1,1,1,1,1,1,1},  //N 65-69
    {0,1,1,1,1,1,1,1,1,1,0},{1,0,0,0,0,0,0,0,0,0,1},{1,0,0,0,0,0,0,0,0,0,1},{1,0,0,0,0,0,0,0,0,0,1},{0,1,1,1,1,1,1,1,1,1,0},  //O 70-74
    {1,1,1,1,1,1,1,1,1,1,1},{0,0,0,0,0,1,0,0,0,0,1},{0,0,0,0,0,1,0,0,0,0,1},{0,0,0,0,0,1,0,0,0,0,1},{0,0,0,0,0,0,1,1,1,1,0},  //P 75-79
    {0,1,1,1,1,1,1,1,1,1,0},{1,0,0,0,0,0,0,0,0,0,1},{1,0,1,0,0,0,0,0,0,0,1},{1,1,0,0,0,0,0,0,0,0,1},{1,1,1,1,1,1,1,1,1,1,0},  //Q 80-84
    {1,1,1,1,1,1,1,1,1,1,1},{0,0,0,1,1,1,0,0,0,0,1},{0,0,1,1,0,1,0,0,0,0,1},{0,1,1,0,0,1,0,0,0,0,1},{1,1,0,0,0,0,1,1,1,1,0},  //R 85-89
    {0,1,0,0,0,0,0,1,1,1,0},{1,0,0,0,0,0,1,0,0,0,1},{1,0,0,0,0,0,1,0,0,0,1},{1,0,0,0,0,0,1,0,0,0,1},{0,1,1,1,1,1,0,0,0,1,0},  //S 90-94
    {0,0,0,0,0,0,0,0,0,1,1},{0,0,0,0,0,0,0,0,0,0,1},{1,1,1,1,1,1,1,1,1,1,1},{0,0,0,0,0,0,0,0,0,0,1},{0,0,0,0,0,0,0,0,0,1,1},  //T 95-99
    {0,1,1,1,1,1,1,1,1,1,1},{1,0,0,0,0,0,0,0,0,0,0},{1,0,0,0,0,0,0,0,0,0,0},{1,0,0,0,0,0,0,0,0,0,0},{0,1,1,1,1,1,1,1,1,1,1},  //U 100-104
    {0,0,1,1,1,1,1,1,1,1,1},{0,1,1,0,0,0,0,0,0,0,0},{1,1,0,0,0,0,0,0,0,0,0},{0,1,1,0,0,0,0,0,0,0,0},{0,0,1,1,1,1,1,1,1,1,1},  //V 105-109
    {1,1,1,1,1,1,1,1,1,1,1},{0,1,0,0,0,0,0,0,0,0,0},{0,0,1,1,0,0,0,0,0,0,0},{0,1,0,0,0,0,0,0,0,0,0},{1,1,1,1,1,1,1,1,1,1,1},  //W 110-114
    {1,1,0,0,0,0,0,0,1,1,1},{0,0,1,1,0,0,1,1,0,0,0},{0,0,0,0,1,1,0,0,0,0,0},{0,0,1,1,0,0,1,1,0,0,0},{1,1,0,0,0,0,0,0,1,1,1},  //X 115-119
    {0,0,0,0,0,0,0,1,1,1,1},{0,0,0,0,0,0,1,1,0,0,0},{1,1,1,1,1,1,1,0,0,0,0},{0,0,0,0,0,0,1,1,0,0,0},{0,0,0,0,0,0,0,0,1,1,1},  //Y 120-124
    {1,1,1,1,0,0,0,0,0,0,1},{1,0,0,1,1,0,0,0,0,0,1},{1,0,0,0,1,1,1,0,0,0,1},{1,0,0,0,0,0,0,1,1,0,1},{1,0,0,0,0,0,0,0,1,1,1},  //Z 125-129
    {0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0}   //ESPACIO 130-134
  };

/**
 *Se configura la tira de LEDS, Podrias ocupar incluso LEDS diferentes al WS2812 
 *Verifica que este el modelo y remplazalo por el actual.
 */
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
// Argumento 1 = Número de pixeles encadenados
// Argumento 2 = Número del pin de Arduino utilizado con pin de datos
// Argumento 3 = Banderas de tipo de pixel:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)


//Interrupcion del ESP cuando detecta el sensor
ICACHE_RAM_ATTR void evento() {
 sensorStatus = true;
}

//Se inicializa y configura todo
void setup() {
  Serial.begin(115200);

  //Intenta conectarse al Wi-Fi y hasta que lo logro pasa a la siguiente instruccion
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Se crea un listener en la URI mensajeria. Y se verifica que tenga el parametro mensaje, si es así lo asigna a la variable mensaje.
  server.on("/mensajeria",  [] (AsyncWebServerRequest *request) {
    if (request->getParam("mensaje")->value()) {
      mensaje = request->getParam("mensaje")->value();
    }
    Serial.println("(APP) Recibi el mensaje: " + mensaje);
    request->send(200, "text/html", "(ESP) Recibi el mensaje: " + mensaje);
  });
  server.onNotFound(notFound);
  server.begin();

  //Se configura la entrada del sensor y la interrupcion
  pinMode(SENSOR, INPUT);
  attachInterrupt(digitalPinToInterrupt(SENSOR), evento, RISING);
  
  // Inicializamos el objeto "pixeles"
  pixels.begin(); 
  pixels.setBrightness(30);
  pixels.show(); 
}

//Ciclo infinito hasta que se apague
void loop() {
  if(sensorStatus==true)
  {
    stringToChar(mensaje);
    sensorStatus = false;
  }
}

//Muestra el caracter correspondiente
void showChar(int x){
  Serial.println(x);
  for(int column=x;column < x+5; column++){
      pixels.clear();
      for(int row=9; row<20; row++) {
        if(ascii[column][row-9] == 1){
          pixels.setPixelColor(row, pixels.Color(255, 0, 0));
        }
      }
      pixels.show(); 
      //delayMicroseconds(delay_1); //Habiliten este delay si ven los caracteres muy anchos y jueguen con los valores de delay_1 declarado al inicio
  }
  pixels.clear();
  pixels.show(); 
}

//Convierte un string a un charArray y posteriormente llama a la función ShowChar
void stringToChar(String msg){  
  int len = msg.length()+1;
  char charBuf[len];
  msg.toUpperCase();
  msg.toCharArray(charBuf, len);

  //Serial.println(charBuf);
  for(int i=0;i < len-1; i++){
    switch(charBuf[i]){
      case 'A':
        showChar(0);
        break;
      case 'B':
        showChar(5);
        break;
      case 'C':
        showChar(10);
        break;
      case 'D':
        showChar(15);
        break;
      case 'E':
        showChar(20);
        break;
      case 'F':
        showChar(25);
        break;
      case 'G':
        showChar(30);
        break;
      case 'H':
        showChar(35);
        break;
      case 'I':
        showChar(40);
        break;
      case 'J':
        showChar(45);
        break;
      case 'K':
        showChar(50);
        break;
      case 'L':
        showChar(55);
        break;
      case 'M':
        showChar(60);
        break;
      case 'N':
        showChar(65);
        break;
      case 'O':
        showChar(70);
        break;
      case 'P':
        showChar(75);
        break;
      case 'Q':
        showChar(80);
        break;
      case 'R':
        showChar(85);
        break;
      case 'S':
        showChar(90);
        break;
      case 'T':
        showChar(95);
        break;
        case 'U':
        showChar(100);
        break;
      case 'V':
        showChar(105);
        break;
      case 'W':
        showChar(110);
        break;
      case 'X':
        showChar(115);
        break;
      case 'Y':
        showChar(120);
        break;
      case 'Z':
        showChar(125);
        break;
      case ' ':
        showChar(130);
        break;
    }
  }
}
