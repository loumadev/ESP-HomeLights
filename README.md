# ESP8266 Smart Home Controller

Lights and Temperature controller for Home using ESP8266

## Requirements


### WebSockets

This library is used to create and manage communication between server and client.

To install this library go to `Sketch > Include Library > Add .ZIP Libraries` in your Arduino IDE and then select `arduinoWebSockets.zip` file from root folder of project.


### SPI Flash File System

Saving data is done by SPIFFS. To store data directly from your PC you must have istalled flashing tool.

To install this tool open explorer in Arduino folder. This is usually in
```
<drive>:\Users\<username>\Documents\Arduino
```
Into this folder paste **uncompressed** `tools.zip` folder from root folder of project and restart Arduino IDE.
Now you should have in `tools` menu `ESP8266 Sketch Data Upload` option.


## Installation

### Setup rooms

You can create by default up to 10 rooms (Can be changed in `server.ino` > `maxRooms`).

To manage these you have to point into `/server/data/data.txt`.
Format is following:
```
Name_Of_Room;RED_VALUE;GREEN_VALUE;BLUE_VALUE
```

Example:
```
Bedroom;200;255;255
```

_**NOTE:**_ Make sure there is no new line character `\n` at the end of the file. This can lead to unexpected behavior.

_**NOTE:**_ This settings are default for first run and after changing values by Control Panel, they get overwritten!

_**INFO:**_ Try to avoid using long names for room because of text owerflow in Control Panel.


### Flashing to SPIFFS

Go to `Tools > Flash Size` and select `1M (64K SPIFFS)`. Then select `tools > ESP8266 Sketch Data Upload`.


### Network Configuraton

Change your SSID and Password in `Server.ino` at the top of the source code.


### Uploading Sketch

Now is time to upload sketch.


## Controlling

### Control Panel

To access Control Panel, make sure you are connect to same WiFi network as your ESP8266 Server (or you can use to port forwarding for accessing from differnt network; Server listens on *port 80*). ESP after connecting to WiFi network always prints out it's Local IP Address into Serial Port. In any web browser (Google Chrome preffered) just go to that IP Address and you are connected!


### Adjusting Commands

You can add custom calls into `eventHandler` function.
Default code prints out events and values:
```
void eventHandler(String data[10]) {
    String command = data[0];
    
    if(command == "light") {
        String name = data[1];    //Name of the room
        String rgb[4] = {data[2], data[3], data[4], data[5]};
        updateRoom(name, rgb);
        Serial.print("\nUpdate: " + name + " > rgb(" + rgb[0] + "," + rgb[1] + "," + rgb[2] + "," + rgb[3] + ")");
    } else if(command == "temp") {
        String mode = data[2];   //Temperature mode
        Serial.print("\nUpdate: " + data[1] + " > Temperature mode = " + mode);
    }
}
```

## Events
First element (index 0) in data array is always event name!

### Light change

Event occurs when color, brightness or switching is done in Control Panel.

#### Name
```
light
```

#### Data

1. **Name of room**
2. **Red channel**
3. **Green channel**
4. **Blue channel**
5. **Alpha channel**
   - _`0` - Off_
   - _`1` - On_

### Temperature mode change

Event occurs when temperature mode is changed.

#### Name
```
temp
```

#### Data

1. **Name of room**
2. **mode**
   - _`-1` - None_
   - _`0` - Heating_
   - _`1` - Cooling_



## Authors

* **Jaroslav Louma** - [@jaroslav.louma](https://www.instagram.com/jaroslav.louma/)
