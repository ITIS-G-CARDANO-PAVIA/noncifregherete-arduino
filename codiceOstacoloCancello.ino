// @author: BUSIN LORENZO
// @author: CODENA LORENZO

#include <WiFiNINA.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>
#include <SPI.h>

char ssid[] = "ArduinoNetwork24G"; // SSID Rete Wifi
char pass[] = "ArduinoWifi2022";   // Password della rete WiFi WPA2/PSK
int keyIndex = 0;                  // Index della rete per il WPA2
int status = WL_IDLE_STATUS;       // Stato della connessione WiFi

String header;                          // Dichiarazione dell'header

WiFiServer server(80);                  // Porta della connessione
Servo motore;
WiFiClient client = server.available(); // Disponibilità del server

String valoreOttenuto = "0";    // Variabile per salvare il valore ottenuto dalla richiesta HTTP
int posizionePonte = 0;         // Variabile per salvare la posizione del ponte
int pos1 = 0;                   // Variabile per estrarre il valore del rosso
int pos2 = 0;                   // Variabile per il terminatore di riga
int velocita = 20;              // Variabile per la velocità dell'arduino
 
int pinMotore = 9;                 // Pin del motore
int pinIn1 = 10;
int pinIn2 = 11;


unsigned long currentTime = millis(); // Tempo corrente
unsigned long previousTime = 0;       // Tempo della richiesta precedente
const long timeoutTime = 2000;        // Timeout richiesta

void setup() {                      //
  WiFi.setHostname("Cancello");
  pinMode(pinMotore, OUTPUT);
  pinMode(pinIn1, OUTPUT);  
  pinMode(pinIn2, OUTPUT);
  
  Serial.begin(9600);                     // Inizializzazione monitor seriale
  while (!Serial);                        // Ciclo in caso di errore nella seriale
  
  enable_WiFi();                          // Funzione per avviare il WiFi
  connect_WiFi();                         // Funzione per connettersi alla rete WiFi

  server.begin();                         // Inizializzazione WebServer             
  printWifiStatus();                      // Scrittura log della connessione
}

void loop() {
  client = server.available(); // Controllo se il client è presente

  if (client) {                // 
    printWEB();                // Se il client è presente stampo la pagina WEB
  }                            //
  
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
    Serial.println("Errore con il modulo WiFi");                  // Eseguo un post sul modulo WiFi
    while (true);                                                 //
  }                                                               //

  String fv = WiFi.firmwareVersion();                             //
  if (fv < "1.0.0") {                                             // Eseguo un controllo sulla versione del Firmware
    Serial.println("Firmware da aggiornare");                     //
  }                                                               //
  
}



void connect_WiFi() {
  while (status != WL_CONNECTED) {                        //
    Serial.print("Tentativo di connessione alla rete: "); // Tentativo di connessione alla rete WiFi
    Serial.println(ssid);                                 //
    status = WiFi.begin(ssid, pass);                      //

    delay(200);                                          // Attendo 200 millisecondi
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
        Serial.write(c);                                                                 // Stampo i byte sul monitor seriale
        header += c;                                                                     // Salvo i byte nella variabile header
        if (c == '\n') {                                                                 // Controllo se ci sono dei nuovi byte

          if (currentLine.length() == 0) {                                               // Controllo se la linea è vuota e quindi rispondo al Client

            client.println("HTTP/1.1 200 OK");                                           // 
            client.println("Content-type:text/html");                                    // Header della richiesta HTTP
            client.println("Connection: close");                                         //
            client.println();                                                            //
                   
            client.println("<!DOCTYPE html><html>"); 
            client.println("<head>");
            client.println("<meta charset='UTF-8'><meta name='viewport' content='width=device-width, inner-scale=1'><title>Cancello</title>");  
            
            client.println("<style>");
            client.println(".container{text-align: center;}");
            client.println("label{font: arial;font-size: 22px;text-align: center;}");
            client.println("input{width: 100%;padding: 3px;font-size: 18;}");
            client.println("button{width: 100%;height: 30px;background-color: aqua;border-radius: 1.5rem;margin-top: 1.5rem;}");
            client.println(".invio:hover{background-color: aquamarine;height: 40px}");
            client.println("</style>");
            
            client.println("<script defer>");
            client.println("const paginaURL = window.location.href.split('?')[0]");
            client.println("function inviaIndirizzo(){");
            client.println("var posizione = document.getElementById('posizione').value");
            client.println("if (posizione < 0){");
            client.println("posizione=0");
            client.println("}");
            client.println("if (posizione > 180){");
            client.println("posizione=180");
            client.println("}");
            client.println("var datiDaInviare = (paginaURL + '?P' + posizione + '&')");
            client.println("window.location.href = datiDaInviare");
            client.println("}");
            client.println("</script>");
            
            client.println("</head>");
            client.println("<body>");
            
            client.println("<div class='container'>");
            client.println("<label>Inserire un valore per modificare la posizione</label>");
            client.println("<input type='number' step='1' min='0' max='180' id='posizione'>");
            client.println("<button id='invio' class='invio' onclick='inviaIndirizzo()'>Invio</button>");
            client.println("</div>");
            
            
            
            client.println("</body>");

            client.println("</html>");
            
                                                                                                                
            
            client.println();                                                                           // Chiudo la richiesta HTTP con una linea vuota
            
            //DC Motor ClockWise Rotation
            if(header.indexOf("GET /?CW") >= 0) { 
              digitalWrite(pinIn1, HIGH);
              digitalWrite(pinIn2, LOW);
              analogWrite(pinMotore, 255);
              delay(28000);
              digitalWrite(pinIn1, LOW);
              digitalWrite(pinIn2, LOW);

            //DC Motor CounterClockWise Rotation
            }else if(header.indexOf("GET /?CCW") >= 0){
              digitalWrite(pinIn1, LOW);
              digitalWrite(pinIn2, HIGH);
              analogWrite(pinMotore, 255);
              delay(28000);
              digitalWrite(pinIn1, LOW);
              digitalWrite(pinIn2, LOW);
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
