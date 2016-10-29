

/*
    https://github.com/Links2004/arduinoWebSockets
*/

#include <ESP8266HTTPUpdateServer.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <WebSockets.h>
#include <Hash.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
//__________________________________________________
#define D1 D1
long lastPressTime = 0;
int estadoD1 = 0;
int estadoD2 = 0;
int iD1 = 0;
//_________________________________________________
const char* host = "XXXX";
const char* update_path = "/update";
const char* update_username = "XXX";
const char* update_password = "XXXXXX";
static const char ssid[] = "XXXXXXXX";
static const char password[] = "XXXXXX";
IPAddress ip(XXX, XXX, X, XXX); // desired IP Address
IPAddress mask(XXX, XXX, XXX, XX); //Mascara de red
IPAddress gat(XXX, XXX, X, X);//gateway

String macAddress();

MDNSResponder mdns;

static void writeLED(bool);

bool LEDon;
unsigned long ulReconncount = 0;
uint8_t socketNumber;

ESP8266WiFiMulti WiFiMulti;

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
WebSocketsServer webSocket = WebSocketsServer(81);
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="mobile-web-app-capable" content="yes" /> 
<meta name="viewport" content="user-scalable=no, width=device-width, initial-scale=.85, maximum-scale=.85"/>

<script>

function LEDswitch(){
            var LEDswitchCheck = document.getElementById('myonoffswitch').checked;        
            if(LEDswitchCheck){
                websock.send("teensyLEDon");      
            }
            else if(!LEDswitchCheck) {
                websock.send("teensyLEDoff");
            }}

var websock;
var test = "open";
function start() {
            websock = new WebSocket('ws://' + window.location.hostname + ':81/');
            websock.onopen = function(evt) { 
                console.log('websock open'); 
                //statusUpdate();
            };
            websock.onclose = function(evt) { 
                console.log('websock close'); 
            };
            websock.onerror = function(evt) { 
                console.log(evt); 
            };
            websock.onmessage = function(evt) {
                console.log('httpServer: ', evt.data.slice(2));
                //mihora;
                //mifecha;

                
                /*temperatura y humedad
           if(evt.data.slice(0,2)=='2:')
           { document.getElementById('ActTemp').value = evt.data.slice(2);}
           else if(evt.data.slice(0,2)=='3:')
           { document.getElementById('ActHum').value = evt.data.slice(2);}
           */
var te = document.getElementById('myonoffswitch');

                if(evt.data === 'teensyLEDon')
                {
                    te.checked = 'checked';
                }
                else if(evt.data === 'teensyLEDoff')
                {
                    te.checked = '';
                }


};

}
</script>
</head>
<body onload="start();">
<h1 style: align=center>BUTTON</h1>
<style>table, th, td {border: 0px solid black; border-collapse:separate;
  border-spacing: 60px 0; padding-top: 60px; padding-right: 0px;
  padding-bottom: 20px; padding-left: 0px;}</style>
<table style="margin: 0px auto;">
  <tr>
    
    <td><div class='switch'>
            <input type='checkbox' id='myonoffswitch' onclick='LEDswitch()'>
        <label><i></i></label></div>
</div></td>
  </tr>
  
</table>

<style>
table.table1{background: rgba(88, 90, 106, .9);
  box-shadow: inset 0 3px 3px rgba(255, 255, 255, 0.8),
  0 4px 6px #131216;
border-collapse:separate;
border-spacing: 5px 0; padding: 10px 0px 10px 0px;}</style>
<table class="table1" style="margin: 10px auto;">
  
    <tr>
    <td><h3>HORA</h3></td>
      <td><i><h3><output name='mihora' id='mihora'></output>24H<br></h3></i></td>
  </tr>
  <tr>
    <td><h3>FECHA</h3></td>
    <td><i><h3><output name='mifecha' id='mifecha'></output></h3></i></td>
  </tr>
  </table>
  
//CSS code not import

 <body>
<div class="container"></div>
 </body>
</html>
)rawliteral";

// Current LED status
bool LEDStatus;

// Commands sent through Web Socket
const char LEDON[] = "teensyLEDon";
const char LEDOFF[] = "teensyLEDoff";
//_______________________________________________________________
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
    //ESCRIBE EN SERIAL DESDE DONDE LO HICE [1]
  Serial.printf("webSocketEvent(%d, %d, ...)\r\n", num, type);
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\r\n", num);
      break;
    case WStype_CONNECTED:
      {socketNumber = num;
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);
         // Send the current LED status
        if (LEDStatus) {
          webSocket.sendTXT(num, LEDON, strlen(LEDON));
        }
        else {
          webSocket.sendTXT(num, LEDOFF, strlen(LEDOFF));
        }
      }

      break;

            //ESCRIBE EN SERIAL LA TECLA QUE PRESIONE DEL HTML
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\r\n", num, payload);

      if (strcmp(LEDON, (const char *)payload) == 0) {
        writeLED(true);
      }
      else if (strcmp(LEDOFF, (const char *)payload) == 0) {
        writeLED(false);
      }
//------------------------------------------
      if(payload[0] == 'I') {
                // we get GPIO on data
                uint32_t D1= (uint32_t) strtol((const char *) &payload[1], NULL, 16);

                webSocket.broadcastTXT(LEDON);
                digitalWrite(D1, 1);writeLED(true);
                estadoD1=1;lastPressTime=millis();estadoD2=estadoD1;
               // Serial.println(digitalRead(D1));
      }             
            
//------------------------------------------
      // send data to all connected clients
      webSocket.broadcastTXT(payload, length);
//---------------------------------------------      
  webSocket.broadcastTXT("I10"); // GPIO10 ON
//  webSocket.broadcastTXT("O11"); // GPIO11 OFF
//---------------------------------------------
      break;
    case WStype_BIN:
      Serial.printf("[%u] get binary length: %u\r\n", num, length);
      hexdump(payload, length);

      // echo data back to browser
      webSocket.sendBIN(num, payload, length);
      break;
    default:
      Serial.printf("Invalid WStype [%d]\r\n", type);
      break;
  } 
}

void handleRoot()
{
  httpServer.send(200, "text/html", INDEX_HTML);
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += httpServer.uri();
  message += "\nMethod: ";
  message += (httpServer.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += httpServer.args();
  message += "\n";
  for (uint8_t i=0; i<httpServer.args(); i++){
    message += " " + httpServer.argName(i) + ": " + httpServer.arg(i) + "\n";
  }
  httpServer.send(404, "text/plain", message);
}


static void writeLED(bool LEDon)
{
  LEDStatus = LEDon;
  if (LEDon) {
    webSocket.broadcastTXT(LEDON);
    digitalWrite(D1, 1);
    estadoD1=1;lastPressTime=millis();estadoD2=estadoD1;
    Serial.println(digitalRead(D1));
  }
  else {
    webSocket.broadcastTXT(LEDOFF);
    digitalWrite(D1, 0);
    estadoD1=0;
    if(estadoD2-estadoD1>0){Serial.println(digitalRead(D1));estadoD2=estadoD1;
                           }
       }
}

void setup()
{
  pinMode(D1, OUTPUT);
  writeLED(false);


  Serial.begin(115200);
  Serial.println();

 WiFi.mode(WIFI_AP_STA);
 WiFi.config(ip,gat,mask);
  IPAddress myIP = WiFi.softAPIP();

  for(uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] BOOT WAIT %d...\r\n", t);
    Serial.flush();
    delay(500);
  }

  WiFiMulti.addAP(ssid, password);

 while(WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(150);
  }


  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
      Serial.print("IP Address AP: ");
  Serial.println(myIP);

    String MAC_ADDR = WiFi.macAddress();
 Serial.print("MAC Address: ");
 Serial.println(MAC_ADDR);

  if (mdns.begin(host, WiFi.localIP())) {
    Serial.println("MDNS responder started");
    mdns.addService("http", "tcp", 80);
    mdns.addService("ws", "tcp", 81);
  }
  else {
    Serial.println("MDNS.begin failed");
  }
    httpUpdater.setup(&httpServer, update_path, update_username, update_password);
  httpServer.begin();

    Serial.printf("HTTPUpdateServer ready! Open http://%s.local%s in your browser and login with username '%s' and password '%s'\n", host, update_path, update_username, update_password);

  httpServer.on("/", handleRoot);

  httpServer.on("/inline", [](){
 //   server.send(200, "text/html", "conectado oZ");
  });
  httpServer.onNotFound(handleNotFound);

  httpServer.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}
void loop()
{
  webSocket.loop();  
  httpServer.handleClient();

     if(millis() - lastPressTime >=3000)
   {iD1=0;digitalWrite(D1,0);estadoD1=0;writeLED(false);}
}
