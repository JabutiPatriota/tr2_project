// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include <NewPing.h>

// Replace with your network credentials
const char* ssid     = "MENDESLUNA_2G";
const char* password = "8132654102";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

//Motor PINs
#define ENA D0
#define IN1 D1
#define IN2 D2
#define IN3 D3
#define IN4 D4
#define ENB D5
#define TRIGGER_PIN 13
#define ECHO_PIN 12
#define MAX_DISTANCE 200
//const int echoPin = 12; //D6
//const int trigPin = 13; //D7

unsigned long myChannelNumber = 2592597;
const char * myWriteAPIKey = "CSEOMRZFXE49T3K1";

//Control variables
bool forward = 0;
bool backward = 0;
bool left = 0;
bool right = 0;
int Speed = 150; //inicialmente constante

//sensor variables
long duration;
int distance;

// Auxiliar variables to store the current output state
String output5State = "off";
String output4State = "off";
String carMovingForward = "off";
String carStopped = "off";
String carMovingBackward = "off";

// Assign output variables to GPIO pins
const int LED = 15;
const int output5 = 5;
const int output4 = 4;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

// Ping sensor
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output5, OUTPUT);
  pinMode(output4, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Set outputs to LOW
  digitalWrite(output5, LOW);
  digitalWrite(output4, LOW);
  digitalWrite(LED, LOW);

  // Set sensor PINs
  //pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  //pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //duas piscadas de led indicam conexão na rede
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void carforward() {
  analogWrite(ENA, Speed-40);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void carbackward() {
  analogWrite(ENA, Speed-40);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void carturnleft() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void carturnright() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void carStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void loop(){
  delay(50);
  unsigned int uS = sonar.ping();
  pinMode(ECHO_PIN, OUTPUT);
  digitalWrite(ECHO_PIN, LOW);
  pinMode(ECHO_PIN, INPUT);
  Serial.print("Ping: ");
  float distance = uS/US_ROUNDTRIP_CM;
  Serial.print(distance);
  Serial.println("cm");
  delay(100);

  while(distance < 7  && 1 < distance){
    carStop();
    carturnright();
    delay(200);
    carStop();
    delay(50);
    unsigned int uS = sonar.ping();
    pinMode(ECHO_PIN, OUTPUT);
    digitalWrite(ECHO_PIN, LOW);
    pinMode(ECHO_PIN, INPUT);
    Serial.print("Ping: ");
    distance = uS/US_ROUNDTRIP_CM;
    Serial.print("DESVIAR");
    Serial.print(distance);
    Serial.println("cm");
    delay(100);
  }

  if(carMovingForward == "on"){
    carforward();
  } else if (carMovingBackward == "on") {
    carbackward();
  }

  WiFiClient client = server.available();   // Listen for incoming clients
  WiFiClient client2 = server.available();

  //Initialize ThingSpeak
  ThingSpeak.begin(client2);

  int x = ThingSpeak.writeField(myChannelNumber, 1, distance, myWriteAPIKey);

    if (x==200){
      Serial.println("Channel update successful. (DISTANCE)");
    } else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
  

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("RSSI:");
  Serial.println(rssi);
  int y = ThingSpeak.writeField(myChannelNumber, 2, rssi, myWriteAPIKey);

  if (y==200){
    Serial.println("Channel update successful. (RSSI)");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  if (client) {    // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /forward") >= 0){
              Serial.println("Moving Forward");
              carforward(); //move o carro para frente
              carStopped = "off";
              carMovingForward = "on";
              carMovingBackward = "off";
              digitalWrite(LED, HIGH); //acende o led amarelo
            } else if (header.indexOf("GET /backward") >= 0){
              Serial.println("Moving Backward");
              carbackward(); //move o carro para trás
              carStopped = "off";
              carMovingForward = "off";
              carMovingBackward = "on";
              digitalWrite(LED, LOW);
            } else if (header.indexOf("GET /right") >= 0){
              Serial.println("Moving Right");
              carturnright(); //move o carro para a direita
              delay(200);
              carStop();
              carStopped = "on";
              carMovingForward = "off";
              carMovingBackward = "off";
              digitalWrite(LED, LOW);
            } else if (header.indexOf("GET /left") >= 0){
              Serial.println("Moving Left");
              carturnleft(); //move o carro para a esquerda
              delay(200);
              carStop();
              carStopped = "on";
              carMovingForward = "off";
              carMovingBackward = "off";
              digitalWrite(LED, LOW);
            } else if (header.indexOf("GET /stop") >= 0){
              Serial.println("Stopping the car");
              carStop(); //para o carro
              carStopped = "on";
              carMovingForward = "off";
              carMovingBackward = "off";
              digitalWrite(LED, LOW);
            } else if (header.indexOf("GET /velocidade") >= 0){
              Serial.println("Stopping the car");
              carStop(); //para o carro
              carStopped = "on";
              carMovingForward = "off";
              carMovingBackward = "off";
              Serial.println(header[header.indexOf("idade-")+6]);
              digitalWrite(LED, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP8266 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 5  
            //client.println("<p>GPIO 5 - State " + output5State + "</p>");
            // If the car is stopped, it cans run forward, backward, or turn sides       
            if (carStopped=="on") {
              client.println("<p><a href=\"/forward\"><button class=\"button\">FRENTE</button></a></p>");
              client.println("<p><a href=\"/backward\"><button class=\"button\">RE</button></a></p>");
              client.println("<p><a href=\"/right\"><button class=\"button\">DIREITA</button></a></p>");
              client.println("<p><a href=\"/left\"><button class=\"button\">ESQUERDA</button></a></p>");
              client.println("<p><input type=\"number\" id=\"teste\" placeholder=\"velocidade\" /></p>");
              client.println("<p><button href=\"/velocidade-${document.getElementById('teste')}\">enviar</button><p>");
            } else {
              client.println("<p><a href=\"/stop\"><button class=\"button button2\">PARAR</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 4  
            //client.println("<p>GPIO 4 - State " + output4State + "</p>");
            // If the output4State is off, it displays the ON button       
            //if (output4State=="off") {
              //client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
            //} else {
            //  client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            //}
            //client.println("</body></html>");
                        
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop

            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }

    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
 }
