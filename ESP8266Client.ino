#define ENA   14          //         GPIO14(D5)
#define ENB   12          //         GPIO12(D6)
#define IN_1  0           //         GPIO0(D3)
#define IN_2  2           //         GPIO2(D4)
#define IN_3  13          //         GPIO13(D7)
#define IN_4  15          //         GPIO15(D8)

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

String comando;             // Variabile per salvare il comando.
int velocita = 150;         // Potenza dei motori 0-1023
int coefVelocita = 3;       // Coefficiente per la velocità in rotazione

bool jammer = false;

const char* ssid = "ArduinoNetwork24G";             //Dichiarazione variabili per il collegamento con il wifi
const char* password = "ArduinoWifi2022";           //

// Set your Static IP address
IPAddress local_IP(192, 168, 0, 110);
// Set your Gateway IP address
IPAddress gateway(192, 168, 0, 1);

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192, 168, 0, 1);   //optional
IPAddress secondaryDNS(8, 8, 8, 8); //optional

ESP8266WebServer server(80);         // Porta di ascolto per il webserver

void setup() {
 pinMode(ENA, OUTPUT);   //
 pinMode(ENB, OUTPUT);   //
 pinMode(IN_4, OUTPUT);  // Dichiarazione utilizzo dei pin in output
 pinMode(IN_3, OUTPUT);  //
 pinMode(IN_2, OUTPUT);  //
 pinMode(IN_1, OUTPUT);  //
  
 Serial.begin(9600);  // Inizializzazione del monitor seriale

  fermo();

  delay(150); // Attesa 

  while (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {       //
    Serial.println("STA Failed to configure");
  }
  
  while (!WiFi.setHostname("Macchina")) {       //
    Serial.println("STA Failed to configure");
  }
  
  WiFi.begin(ssid, password);                   //
  Serial.print("Connecting to ");               // Connessione al wifi + log
  Serial.print(ssid); Serial.println(" ...");   //
                                                //
  int i = 0;                                    //
  while (WiFi.status() != WL_CONNECTED) {       //
    delay(250);                                 //
    Serial.print(++i); Serial.print(' ');       //
  }                                             //

  Serial.println('\n');                          //
  Serial.println("Connection established!");     // Stampa ip più log del collegamento
  Serial.print("IP address:\t");                 //
  Serial.println(WiFi.localIP());                //

  server.on ( "/", HTTP_handleRoot );            //
  server.onNotFound ( HTTP_handleRoot );         // Avvio del webserver
  server.begin();                                //
  
  fermo();                            //   
}

void indietro(){ 

      digitalWrite(IN_4, HIGH);
      digitalWrite(IN_3, LOW);
      analogWrite(ENA, velocita);

      digitalWrite(IN_2, HIGH);
      digitalWrite(IN_1, LOW);
      analogWrite(ENB, velocita);
  }
  
void avanti(){ 

      digitalWrite(IN_4, LOW);
      digitalWrite(IN_3, HIGH);
      analogWrite(ENA, velocita);

      digitalWrite(IN_2, LOW);
      digitalWrite(IN_1, HIGH);
      analogWrite(ENB, velocita);
  }

void sinistra(){ 

      digitalWrite(IN_4, HIGH);
      digitalWrite(IN_3, LOW);
      analogWrite(ENA, velocita);

      digitalWrite(IN_2, LOW);
      digitalWrite(IN_1, HIGH);
      analogWrite(ENB, velocita);
  }

void destra(){

      digitalWrite(IN_4, LOW);
      digitalWrite(IN_3, HIGH);
      analogWrite(ENA, velocita);

      digitalWrite(IN_2, HIGH);
      digitalWrite(IN_1, LOW);
      analogWrite(ENB, velocita);
  }

void avantiDestra(){
      
      digitalWrite(IN_4, LOW);
      digitalWrite(IN_3, HIGH);
      analogWrite(ENA, velocita/coefVelocita);
 
      digitalWrite(IN_2, LOW);
      digitalWrite(IN_1, HIGH);
      analogWrite(ENB, velocita);
   }

void avantiSinistra(){
      
      digitalWrite(IN_4, LOW);
      digitalWrite(IN_3, HIGH);
      analogWrite(ENA, velocita);

      digitalWrite(IN_2, LOW);
      digitalWrite(IN_1, HIGH);
      analogWrite(ENB, velocita/coefVelocita);
  }

void indietroDestra(){ 

      digitalWrite(IN_4, HIGH);
      digitalWrite(IN_3, LOW);
      analogWrite(ENA, velocita/coefVelocita);

      digitalWrite(IN_2, HIGH);
      digitalWrite(IN_1, LOW);
      analogWrite(ENB, velocita);
  }

void indietroSinistra(){ 

      digitalWrite(IN_4, HIGH);
      digitalWrite(IN_3, LOW);
      analogWrite(ENA, velocita);

      digitalWrite(IN_2, HIGH);
      digitalWrite(IN_1, LOW);
      analogWrite(ENB, velocita/coefVelocita);
  }

void fermo(){  

      digitalWrite(IN_4, LOW);
      digitalWrite(IN_3, LOW);
      analogWrite(ENA, velocita);

      digitalWrite(IN_2, LOW);
      digitalWrite(IN_1, LOW);
      analogWrite(ENB, velocita);
 }

void loop() {
    server.handleClient();     // Gestione delle richieste http
      
      comando = server.arg("State");    // Salvataggio dei parametri ricevuti

      //Serie di If per la gestione dei parametri  
      
      if (comando == "Bloccato") jammer = true;
      else if (comando == "Sbloccato") jammer = false;

      if(jammer){
        fermo();
      }else{
        if (comando == "A") avanti();     
        else if (comando == "I") indietro();
        else if (comando == "S") sinistra();
        else if (comando == "D") destra();
        else if (comando == "AD") avantiDestra();
        else if (comando == "AS") avantiSinistra();
        else if (comando == "ID") indietroDestra();
        else if (comando == "IS") indietroSinistra();
        else if (comando == "F") fermo();
        else if (comando == "Bloccato") jammer = true;
        else if (comando == "Sbloccato") jammer = false;
      }
}

void HTTP_handleRoot(void) {

if( server.hasArg("State") ){
       Serial.println(server.arg("State"));
  }
  server.send ( 200, "text/html", "" );
  delay(1);
}
