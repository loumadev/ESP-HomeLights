/**
 * @name    Home Lights and Temperature control for ESP8266, controled using webapp
 * @author  Jaroslav Louma
 * @version 1.0.0
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <FS.h>

const String ssid = "ssid";                           //Wireless network name
const String password = "password";                   //Wireless network password

const String Path = "/data.txt";                      //Relative path to data file
const int maxRooms = 10;                              //Max number of rooms
const int Elms = 5;                                   //Number of data in single record
String Rooms[maxRooms][Elms];                         //Rooms object

WebSocketsServer webSocket(81);                       //Setup WebSocket Server
ESP8266WebServer server(80);                          //Setup HTTP Server



/* Main */

void setup() {
    Serial.begin(115200);                             //Setup Serial Port
    WiFi.begin(ssid, password);                       //Connect to Access Point
    Serial.println("\n");

    Serial.print("Connecting");
    while(WiFi.status() != WL_CONNECTED) {            //Wait until establish connection
        delay(500);
        Serial.println(".");
    }

    //Print information
    Serial.print("Connected to: ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    //Setup HTTP Request Listener (Anonymous Callback)
    server.onNotFound([]() {
        Serial.print("\n" + server.client().remoteIP().toString() + " => " + server.uri());
        sendFile("/client.html", "text/html");        //Send UI file
    });

    SPIFFS.begin();                                   //Start SPI Flash File System
    
    server.begin();                                   //Start HTTP Server
    
    webSocket.begin();                                //Start WebSocket Server
    webSocket.onEvent(webSocketEvent);                //Setup WebSocket Event Listener


    if(SPIFFS.exists(Path)) {                         //Check if data file exists
        loadRooms(Rooms);                             //Load data file
    } else {
        writeFile(Path, "Room0;255;0;0;1");           //Create default data file
    }
}

void loop() {
    webSocket.loop();                                 //Update WebSocket
    server.handleClient();                            //Handle HTTP Requests
}


/* Functions */

void eventHandler(String data[10]) {
    String command = data[0];
    
    if(command == "light") {
        String name = data[1];  //Name of the room
        String rgb[4] = {data[2], data[3], data[4], data[5]};
        updateRoom(name, rgb);
        Serial.print("\nUpdate: " + name + " > rgb(" + rgb[0] + "," + rgb[1] + "," + rgb[2] + "," + rgb[3] + ")");
    } else if(command == "temp") {
        String mode = data[2]; //Temperature mode
        Serial.print("\nUpdate: " + data[1] + " > Temperature mode = " + mode);
    }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t len) {
    if(type == WStype_CONNECTED) {            //On new connection
        String rooms = parseRooms(Rooms);             //Parse room data as string
        webSocket.sendTXT(0, rooms);                  //Send parsed data to client
    } else if(type == WStype_TEXT) {          //On message
        String buff;
        for(int i = 0; i < len; i++) buff += (char)payload[i];    //Create String Buffer

        String data[10];
        stringSplit(buff, ';', data, 10);             //Parse data
        
        eventHandler(data);
    }
}

void loadRooms(String output[maxRooms][Elms]) {
    String data = readFile(Path);
          
    String rooms[maxRooms];
    stringSplit(data, '\n', rooms, maxRooms);

    for(int i = 0; i < maxRooms; i++) {
        String room[Elms];
        stringSplit(rooms[i], ';', room, maxRooms);
              
        String name = room[0];
        if(!name) continue;

        for(int j = 0; j < Elms; j++) output[i][j] = room[j];
    }
}

void updateRoom(String name, String data[Elms-1]) {
    String curr[maxRooms][Elms];
    loadRooms(curr);

    for(int i = 0; i < maxRooms; i++) {
        for(int j = 0; j < Elms-1; j++) {
            if(curr[i][0] == name) {
                curr[i][j+1] = data[j];
                Rooms[i][j+1] = data[j];
            }
        }
    }

    writeFile(Path, parseRooms(curr));
}

String parseRooms(String rooms[maxRooms][Elms]) {
    String buff;
    for(int i = 0; i < maxRooms; i++) {
        if(!rooms[i][0] || rooms[i][0] == NULL || rooms[i][0] == "") continue;
        for(int j = 0; j < Elms; j++) buff += rooms[i][j] + (j == Elms-1 ? "" : ";");
        buff += "\n";
    }
    return buff.substring(0, buff.length()-1);
}


/* Helper Functions */

bool sendFile(String path, String type) {
    if(!SPIFFS.exists(path)) return false;
    File f = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(f, type);
    f.close();
}

bool deleteFile(String path) {
    if(!SPIFFS.exists(path)) return false;
    SPIFFS.remove(path);
    return true; 
}

void appendFile(String path, String text) {
    File f = SPIFFS.open(path, "a");
    f.print((text && text != NULL) ? text : "");
    f.close();
}

void writeFile(String path, String text) {
    File f = SPIFFS.open(path, "w");
    String a = (text && text!=NULL) ? text : "";
    f.print(a);
    f.close();
}

String readFile(String path) {
    if(SPIFFS.exists(path)) {
        File f = SPIFFS.open(path, "r");
        String buff;
        while(f.available()) buff += char(f.read());
        f.close();
        return buff;
    }
}

String getContentType(String filename, String dismatch = "text/plain") {
    if(filename.endsWith(".htm")) return "text/html";
    else if(filename.endsWith(".html")) return "text/html";
    else if(filename.endsWith(".css")) return "text/css";
    else if(filename.endsWith(".js")) return "application/javascript";
    else if(filename.endsWith(".json")) return "application/json";
    else if(filename.endsWith(".png")) return "image/png";
    else if(filename.endsWith(".gif")) return "image/gif";
    else if(filename.endsWith(".jpg")) return "image/jpeg";
    else if(filename.endsWith(".ico")) return "image/x-icon";
    else if(filename.endsWith(".svg")) return "image/svg+xml";
    else if(filename.endsWith(".xml")) return "text/xml";
    else if(filename.endsWith(".pdf")) return "application/x-pdf";
    else if(filename.endsWith(".zip")) return "application/x-zip";
    else if(filename.endsWith(".gz")) return "application/x-gzip";
    return dismatch;
}

void stringSplit(String data, char separator, String output[], int size) {
  int len = data.length();
  int id = 0;
  int lastIndex = 0;

  for(int i = 0; i < len; i++) {
      if(data.charAt(i) == separator) {
          output[id++] = data.substring(lastIndex, i);
          lastIndex = i+1;
      }
      if(id >= size) break;
  }

  output[id] = data.substring(lastIndex, len);
}
