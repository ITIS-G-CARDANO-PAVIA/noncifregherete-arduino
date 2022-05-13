// @author: BUSIN LORENZO
// @author: CODENA LORENZO

#include <WiFiNINA.h>
#include <WiFiUDP.h>

char ssid[] = "ArduinoNetwork24G"; // SSID Rete Wifi
char pass[] = "ArduinoWifi2022";   // Password della rete WiFi WPA2/PSK
int keyIndex = 0;                  // Index della rete per il WPA2
int status = WL_IDLE_STATUS;       // Stato della connessione WiFi

String header;                          // Dichiarazione dell'header

WiFiServer server(80);                  // Porta della connessione
WiFiClient client = server.available(); // Disponibilità del server

IPAddress indirizzoIPMacchina;

String valoreOttenuto = "0";    // Variabile per salvare il valore ottenuto dalla richiesta HTTP
bool statoJammer = true;         // Variabile per salvare la posizione del ponte
int pos1 = 0;                   // Variabile per estrarre il valore del rosso
int pos2 = 0;                   // Variabile per il terminatore di riga

String indirizzoIPRobot = "";

unsigned long currentTime = millis(); // Tempo corrente
unsigned long currentTimeDistance = millis(); // Tempo corrente

unsigned long previousTime = 0;       // Tempo della richiesta precedente
unsigned long previousDelayDistance = 0;       // Tempo della misura richiesta precedente
const long timeoutTime = 2000;        // Timeout richiesta
const long delayDistance = 25;

const int distanzaTrigger = 10;
const int LEDVERDE_PIN = 13;
const int LEDROSSO_PIN = 12;

const int TRIG_PIN = 10;
const int ECHO_PIN = 9;

String splitString(String str, char sep, int index)
{
 int found = 0;
 int strIdx[] = { 0, -1 };
 int maxIdx = str.length() - 1;

 for (int i = 0; i <= maxIdx && found <= index; i++)
 {
    if (str.charAt(i) == sep || i == maxIdx)
    {
      found++;
      strIdx[0] = strIdx[1] + 1;
      strIdx[1] = (i == maxIdx) ? i+1 : i;
    }
 }
 return found > index ? str.substring(strIdx[0], strIdx[1]) : "";
}

IPAddress toIp(String s){
  
  String b1 = splitString(s, '.',0);//byte uno
  String b2 = splitString(s, '.',1);//byte uno
  String b3 = splitString(s, '.',2);//byte uno
  String b4 = splitString(s, '.',3);//byte uno
  int ib1=b1.toInt();
  int ib2=b2.toInt();
  int ib3=b3.toInt();
  int ib4=b4.toInt();

  IPAddress ip = IPAddress(ib1,ib2,ib3,ib4);

  return ip;
}

void printWifiStatus() {
  Serial.print("SSID: ");                                                     // Stampo l'SSID
  Serial.println(WiFi.SSID());                                                //

  IPAddress ip = WiFi.localIP();                                              //
  Serial.print("Indirizzo IP: ");                                             // Stampo l'indirizzo IP
  Serial.println(ip);                                                         //

  long rssi = WiFi.RSSI();                                                    //
  Serial.print("Forza del segnale (RSSI): ");                                 // Stampo la forza del segnale in dBm
  Serial.print(rssi);                                                         //
  Serial.println(" dBm");                                                     //

  Serial.print("Per connettersi al server digitare -> http://");              // Stampo i valori per connettersi al WebServer
  Serial.println(ip);                                                         //
}



void enable_WiFi() {
  if (WiFi.status() == WL_NO_MODULE) {                            //
    //Serial.println("Errore con il modulo WiFi");                  // Eseguo un post sul modulo WiFi
    while (true);                                                 //
  }                                                               //

  String fv = WiFi.firmwareVersion();                             //
  if (fv < "1.0.0") {                                             // Eseguo un controllo sulla versione del Firmware
    //Serial.println("Firmware da aggiornare");                     //
  }                                                               //
  
}



void connect_WiFi() {
  while (status != WL_CONNECTED) {                        //
    //Serial.print("Tentativo di connessione alla rete: "); // Tentativo di connessione alla rete WiFi
    //Serial.println(ssid);                                 //
    status = WiFi.begin(ssid, pass);                      //
    
    digitalWrite(LEDVERDE_PIN, HIGH); 
    delay(100);                                          // Attendo 200 millisecondi
    digitalWrite(LEDVERDE_PIN, LOW); 
    delay(100);                                          // Attendo 200 millisecondi
  }
}

void setup() {

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);
  
  WiFi.setHostname("Jammer");
  
  Serial.begin(9600);                     // Inizializzazione monitor seriale
  
  enable_WiFi();                          // Funzione per avviare il WiFi
  connect_WiFi();                         // Funzione per connettersi alla rete WiFi

  digitalWrite(LEDVERDE_PIN, HIGH); 

  server.begin();                         // Inizializzazione WebServer             
  printWifiStatus();                      // Scrittura log della connessione
}

void loop() {

  if(currentTimeDistance - delayDistance >= previousDelayDistance && statoJammer == true){
    digitalWrite(LEDROSSO_PIN, HIGH);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    unsigned long tempo = pulseIn(ECHO_PIN, HIGH);
    float distanza = 0.03438 * tempo / 2;

    //Serial.println("Distanza: " + String(distanza) + "cm.");

    if (distanza < distanzaTrigger){
      digitalWrite(LEDROSSO_PIN, LOW);
      
      statoJammer = false;

      inviaDatiClient();
    }

    previousDelayDistance = millis();
  }else{
    digitalWrite(LEDROSSO_PIN, LOW);
  }

  client = server.available(); // Controllo se il client è presente

  if (client) {                // 
    printWEB();                // Se il client è presente stampo la pagina WEB
  }                            //
  
  currentTimeDistance = millis();
}

void inviaDatiClient(){
  if(client.connect(indirizzoIPMacchina, 80)){
     String richiesta = String("/?State=") + String("Bloccato");

    client.println("GET " + richiesta + " HTTP/1.1");
    client.println("Host: " + String(indirizzoIPMacchina));
    client.println("Connection: close");
    client.println();

    Serial.println("Messaggio inviato correttamente");

  }else {
    Serial.println("connessione Fallita");
  }
}

void printWEB() {

  if (client) {                                                                          // Controllo se è presente un client
    currentTime = millis();                                                              // Ottengo il tempo attuale
    previousTime = currentTime;                                                          // Salvo il tempo di inizio comunicazione
    Serial.println("Nuovo client");                                                      // Scrivo un log sulla porta seriale
    String currentLine = "";                                                             // Stringa per salvare i dati della connessione
    while (client.connected() && currentTime - previousTime <= timeoutTime) {            // Ciclo fino a quando il cliente è connesso o fino a quando non scade il TimeOut
      currentTime = millis();                                                            // Ottengo il tempo attuale ogni volta che ciclo
      if (client.available()) {                                                          // Controllo se ci sono dei byte inviati dal Client da leggere
        char c = client.read();                                                          // Leggo i byte della connessione
        //Serial.write(c);                                                                 // Stampo i byte sul monitor seriale
        header += c;                                                                     // Salvo i byte nella variabile header
        if (c == '\n') {                                                                 // Controllo se ci sono dei nuovi byte

          if (currentLine.length() == 0) {                                               // Controllo se la linea è vuota e quindi rispondo al Client

            client.println("HTTP/1.1 200 OK");                                           // 
            client.println("Content-type:text/html");                                    // Header della richiesta HTTP
            client.println("Connection: close");                                         //
            client.println();                                                            //
                                                                                         // Chiudo la richiesta HTTP con una linea vuota

            if(header.indexOf("GET /?j") >= 0) {                                                        // Controllo se il client ha inserito un colore
              
              pos1 = header.indexOf('j');                                                               // Definisco la posizione del colore rosso
              pos2 = header.indexOf('&');                                                               // Definisco la posizione del colore verde
              
              valoreOttenuto = header.substring(pos1+2, pos2).c_str();

              if(valoreOttenuto == "enable"){
                 statoJammer = true;
              }
              if(valoreOttenuto == "disable"){
                 statoJammer = false;
              }
            }

            if(header.indexOf("GET /?ip") >= 0) {                                                        // Controllo se il client ha inserito un colore
              
              pos1 = header.indexOf('p');                                                               // Definisco la posizione del colore rosso
              pos2 = header.indexOf('&');                                                               // Definisco la posizione del colore verde
              
              indirizzoIPRobot = header.substring(pos1+1, pos2).c_str();
                        
              //Serial.println("\n"+String(indirizzoIPRobot));
              indirizzoIPMacchina = toIp(indirizzoIPRobot);
              //indirizzoIPRobot = valoreOttenuto.c_str();
              }
            
             break;                           // Break per uscire dal loop
            
          } else {                            // 
            currentLine = "";                 // Se c'è una nuova richiesta elimino la precedente
          }                                   //
          
        } else if (c != '\r') {               //
          currentLine += c;                   // Se ottengo una nuova richiesta, la aggiungo alla variabile
        }                                     //
      }
    }
    
    header = "";                              // Ripulisco l'header della vecchia richiesta
    
    client.stop();                            //
    Serial.println("Client disconnesso");     // Chiudo la connessione
    Serial.println();                         //
  }
}


