// Importer les bibliothèques requises
//#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
#include <WiFi.h>
#include <Wire.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <HX711.h>//https://github.com/bogde/HX711
#include <ESP32Servo.h>

//Parameters
const long LOADCELL_OFFSET = 340884;
const float LOADCELL_DIVIDER = 262.5F;
const int numberOfReadings = 10;
const int doutPin = 32;
const int sckPin = 25;
//Variables
HX711 scale;
float weight = 0;
float initWeight = 0;

// Déclarration des broches de l'ECG
float sensor = 15, ECG, e, f, t, h, Valeur_ECG;

#define shutdown_pin 10 
#define threshold 100 // to identify R peak
#define timer_value 10000 // 10 seconds timer to calculate hr
/*declaration broche du capteur*/
int c=12;
// Signale_Temoin
//int Led_rouge = 16, Led_vert = 19, Buzzer = 15;
Servo myservo;  // create servo object to control a servo

// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
int servoPin = 27;

// Remplacer par vos informations d'identification réseau
 /*char* ssid = "HUAWEI-135F";
 char* password = "lmntrix33";*/
char* ssid = "HUAWEI-MAVV";
char* password = "mavv1234@";

#define DHTPIN 22     // Broche numérique connectée au capteur DHT

// Décommentez le type de capteur utilisé :
#define DHTTYPE    DHT22     // DHT 11

DHT dht(DHTPIN, DHTTYPE);

////////////////////////////////////////////////////////////////
// Set to true to define bouton as Normally Open (NO)
#define BTP_NO    true

// Set number of boutons
#define NUM_BTP  5

// Assign each GPIO to a bouton
int boutonGPIOs[NUM_BTP] = {};
const char* PARAM_INPUT_1 = "bouton";  
const char* PARAM_INPUT_2 = "state";
String capteur(){
   float capt = analogRead(c);
    return String(capt);
  }
  String capteurpoids(){
    if (scale.is_ready()) {
       weight = scale.get_units(numberOfReadings), 10;
       if(weight<0){
        weight = weight * -1;
       }
        return String(weight);
  } }
/////////////////////////////////////////////////////////////////
// Créer un objet AsyncWebServer sur le port 80
AsyncWebServer server(80);

String readDHTTemperature() {
  float t = dht.readTemperature();
  if (isnan(t)) {    
    Serial.println("Échec de la lecture du capteur DHT !");
    return "--";
  }
  else {
    //Serial.println(t);
    return String(t);
  }
}
String readDHTHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Échec de la lecture du capteur DHT !");
    return "--";
  }
  else {
    //Serial.println(h);
    return String(h/10);
  }
}
String readECG(){
if((digitalRead(35) == 1) || (digitalRead(34) == 1)){ //check if leads are removed
Serial.println("leads off!");
}
else{
Valeur_ECG = analogRead(A0);
}
if(Valeur_ECG<0){
  Valeur_ECG = Valeur_ECG * -1;
}
delay(1);
  return String(Valeur_ECG);
}
String capteurSon(){
  capteur();
  return String(capteur());
}
String capteurPoids(){
  capteurpoids();
  return String(capteurpoids());
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-GLhlTQ8iRABdZLl6O3oVMWSktQOp6b7In1Zl3/Jr59b6EGGoI1aFkw7cmDA6j6gD" crossorigin="anonymous">
    <link rel="stylesheet" href="path/to/font-awesome/css/font-awesome.min.css">
    <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css" integrity="sha512-9usAa10IRO0HhonpyAIVpjrylPvoDwiPUiKdWk5t3PyolY1cOd4DSE0Ga+ri4AuTroPR5aQvXU9xC6qOPnzFeg==" crossorigin="anonymous" referrerpolicy="no-referrer" />
    <link rel='stylesheet' type='text/css' href='styles.css'> 
    <style>
      body { background-color: #fff; font-family: sans-serif; color: #333333; font: 14px Helvetica, sans-serif box-sizing: border-box;}
      html {display: inline-block;margin: 0px auto; text-align: center;}
      h2 { font-size: 3.0rem; font-family: "Times New Roman", Times, serif;}
      p { font-size: 3.0rem; font-family: "Times New Roman", Times, serif;}
      #page { font-family: "Times New Roman", Times, serif; margin: 20px; background-color: #fff;}
      .container { height: inherit; padding-bottom: 20px;}
      .header { padding: 20px;}
      .header h1 { font-family: "Times New Roman", Times, serif; padding-bottom: 0.3em; color: #008080; font-size: 45px; font-weight: bold; font-family: Garmond, 'sans-serif'; text-align: center;}
      h2 { padding-bottom: 0.2em; border-bottom: 1px solid #eee; margin: 2px; text-align: left;}
      .header h3 { font-weight: bold; font-family: Arial, 'sans-serif'; font-size: 17px; color: #b6b6b6; text-align: center;}
      .box-full { padding: 20px; border 1px solid #ddd; border-radius: 1em 1em 1em 1em; box-shadow: 1px 7px 7px 1px rgba(0,0,0,0.4); background: #fff; margin: 20px; width: 300px;}
      @media (max-width: 494px) { #page { width: inherit; margin: 5px auto; } #content { padding: 1px;} .box-full { margin: 8px 8px 12px 8px; padding: 10px; width: inherit;; float: none; } }
      @media (min-width: 494px) and (max-width: 980px) { #page { width: 465px; margin 0 auto; } .box-full { width: 380px; } }
      @media (min-width: 980px) { #page { width: 930px; margin: auto; } }
      .sensor {font-family: "Times New Roman", Times, serif; margin: 12px 0px; font-size: 2.5rem;}
      .sensor-labels { font-size: 1rem; vertical-align: middle; padding-bottom: 15px;}
      .units { font-size: 1.2rem;}
      hr { height: 1px; color: #eee; background-color: #eee; border: none;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
    </style>
  </head>
  <body>
  <i class="fa fa-tint" aria-hidden="true"></i>
  <i class="bi bi-water"></i>
  <i class="bi bi-moisture"></i>
    <div id="page">
      <div class="header">
        <h1><i>Système de controle denouveau né</h1></i>
      </div>
      
      <div id="content" align='center'>
        <div class="box-full" align='left'>
          <h2>Lecture des capteurs</h2>
          <div class="sensors-container">
        
          <div class="sensors">
            <p class="sensor">
              <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
              <span class="sensor-labels">Température</span> 
              <span id="temperature">%TEMPERATURE%</span>
              <sup class="units">&degC</sup>
            </p>
            <hr>
          </div>
        
          <div class="sensors">
            <p class="sensor">
              <i class="fas fa-tint" style="color:#00add6;"></i> 
              <span class="sensor-labels">Humidité</span>
              <span id="humidity">%HUMIDITY%</span>
              <sup class="units">&percnt;</sup>
            </p>
             <p class="sensor">
              <i class="fas fa-tint" style="color:#00add6;"></i> 
              <span class="sensor-labels">Poids de l'enfant</span>
              <span id="capteurPoids">%POIDS%</span>
              <sup class="units">Kg</sup>
            </p>
            
            <p class="sensor">
              <i class="fa-solid fa-heart-pulse" style="color: red"></i> 
              <span class="sensor-labels">ECG</span>
              <span id="ecg">%ECG%</span>
              <sup class="units">bpm</sup>
            </p>
          
           
            </div>
           <iframe src="https://github.com/bogde/HX711" width="250" height="240" name ="image du bébé"> 
            </iframe>
          </div>
        </div>
      </div>
    </div>
  <div>
  </div>
  <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha1/dist/js/bootstrap.bundle.min.js" integrity="sha384-w76AqPfDkMBDXo30jS1Sgez6pr3x5MlQ1ZAGC+nuZB+EYdgRZgiwxhTBTkF7CXvN" crossorigin="anonymous"></script>
  </body>
  <script>
    setInterval(function ( ) {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("temperature").innerHTML = this.responseText;
        }
      };
      xhttp.open("GET", "/temperature", true);
      xhttp.send();
    }, 10000 ) ;
    
    setInterval(function ( ) {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("humidity").innerHTML = this.responseText;
        }
      };
      xhttp.open("GET", "/humidity", true);
      xhttp.send();
    }, 10000 ) ;
     setInterval(function ( ) {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("capteurPoids").innerHTML = this.responseText;
        }
      };
      xhttp.open("GET", "/capteurPoids", true);
      xhttp.send();
    }, 10000 ) ;
    
    setInterval(function ( ) {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("ecg").innerHTML = this.responseText;
        }
      };
      xhttp.open("GET", "/ecg", true);
      xhttp.send();
    }, 10000 ) ;
///////////////////////////////////////::
  </script>
</html>)rawliteral";
// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return readDHTTemperature();
  }
  else if(var == "HUMIDITY"){
    return readDHTHumidity();
  }
  else if(var == "POIDS"){
    return capteurPoids();
  }
  else if(var == "ECG"){
    return readECG();
  }
  return String();
}


void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(c, INPUT);
   pinMode(35, INPUT); // Setup for leads off detection LO +
  pinMode(34, INPUT); // Setup for leads off detection LO -
  /*********************CAPTEUR POIDS***********************/
  Serial.println(F("Initialize System"));
  scale.begin(doutPin, sckPin);
  scale.set_scale(); //remove scale divider
  scale.tare();           // reset the scale to 0
  //find
  //Serial.println("Put a known wait and send command");
  //while (Serial.available() == 0) {}
  //Serial.print("Compute average on 10...");
  //Serial.println(scale.get_units(10), 1); //repeat with two different weight to find DIVIDER and OFFSET
  //WEIGHT= (val-OFFSET)/DIVIDER
  scale.set_scale(LOADCELL_DIVIDER);
  scale.tare();
  //scale.set_offset(LOADCELL_OFFSET);
  delay(200);
 initWeight = scale.get_units(numberOfReadings * 10), 10;
  myservo.setPeriodHertz(50); 
  myservo.attach(servoPin);

  // Set all boutons to off when the program starts - if set to Normally Open (NO), the bouton is off when you set the bouton to HIGH
//  for(int i=1; i<=NUM_BTP; i++){
//    pinMode(boutonGPIOs[i-1], OUTPUT);
//    if(BTP_NO){
//      digitalWrite(boutonGPIOs[i-1], HIGH);
//      //Serial.print("Activer... ");
//    }
//    else{
//      digitalWrite(boutonGPIOs[i-1], LOW);
//      //Serial.print("Activer... ");
//    }
//  }

  dht.begin();

//  pinMode(L0_plus, INPUT);
//  pinMode(L0_moin, INPUT);
//  pinMode(sensor, INPUT);

  // Connectez-vous au Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Imprimer l'adresse IP locale ESP32
  Serial.println(WiFi.localIP());

  // Route pour la racine / la page Web
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTTemperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTHumidity().c_str());
  });
   server.on("/capteurPoids", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", capteurPoids().c_str());
  });
  server.on("/ecg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readECG().c_str());
  });



  //////////////////////////////////////////////////////////:
  // Send a GET request to <ESP_IP>/update?bouton=<inputMessage>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
//    Serial.println("Entreeeeeeeeeeeeeer");
//    String inputMessage;
//    String inputParam;
//    String inputMessage2;
    String inputParam2;
    // GET input1 value on <ESP_IP>/update?bouton=<inputMessage>
//    if (request->hasParam(PARAM_INPUT_1) & request->hasParam(PARAM_INPUT_2)) {
//      inputMessage = request->getParam(PARAM_INPUT_1)->value();
//      inputParam = PARAM_INPUT_1;
//      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
//      inputParam2 = PARAM_INPUT_2;
//      if(BTP_NO){
//        Serial.print("NO : ");
//        Serial.println(boutonGPIOs[inputMessage.toInt()-1], !inputMessage2.toInt());
//        digitalWrite(boutonGPIOs[inputMessage.toInt()-1], !inputMessage2.toInt());
//      }
//      else{
//        Serial.print("NC : ");
//        digitalWrite(boutonGPIOs[inputMessage.toInt()-1], inputMessage2.toInt());
//        Serial.println(boutonGPIOs[inputMessage.toInt()-1], !inputMessage2.toInt());
//      }
//    }
//    else {
//      Serial.println("No message sent");
//      Serial.println("none");
//      inputMessage = "No message sent";
//      inputParam = "none";
//    }
//    Serial.println(inputMessage + inputMessage2);
    request->send(200, "text/plain", "OK");
  });
  Serial.println("Bonnnnnnnnnnnnnnnn");
  // Démarrer le serveur
  server.begin();
}

void loop(){
   /*lecture valeur du capteur de son*/
capteur();
//for(int i= 0;i<=180;i++){
// myservo.write(i); 
//  delay(1500);}
//  for(int i= 180;i>=0;i--){
//  myservo.write(i);
//  delay(1500);} 
//
///*  float ECG = analogRead(sensor);
//  float valeur = ECG/15;
//  float temp = dht.readTemperature();
//  if ((valeur<100 || valeur<100) || (temp<36.5 || temp<38.5)){
//    Serial.println("Probléme avec le bébé");
//  }
//  Serial.println(valeur);*/
//    Serial.print("poids de l'enfant");
//  Serial.print(weight); 
  Serial.println(WiFi.localIP());
}
