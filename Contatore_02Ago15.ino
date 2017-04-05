/*
*  conteggia gli impulsi provenienti dall'ngresso
*  in questo caso conteggia impulsi del contatore elettrico
*  trasferisce periodicamente il conteggio a thingSpeak
*
*/
     
#include <ESP8266WiFi.h>

/* inserisci nome della rete, ssid, e pasword */
const char* ssid     = "";
const char* password = "";

/* write key del tuo account thingSpeak */
#define	WRITE_KEY "xxxxxx"


const char* host = "www.thingspeak.com";


int value   = 0;

#define PULSE_PIN 12
int led     = 2;


unsigned long lastSendTime;   

// questa è la routine associata all'interrupt
void blink()
{
static unsigned long lastTime = 0;    

    if ((millis() - lastTime) > 50){
        value++;
        lastTime = millis();
        Serial.println(value); 
    }

}
     
void setup() {
    pinMode(led, OUTPUT);
    pinMode(        0, INPUT_PULLUP);
    pinMode(PULSE_PIN, INPUT_PULLUP);
    attachInterrupt(PULSE_PIN, blink, FALLING);
//    attachInterrupt(PULSE_PIN, blink, CHANGE);
/*
 * Pin interrupts are supported through attachInterrupt, detachInterrupt functions. 
 * Interrupts may be attached to any GPIO pin, except GPIO16. 
 * Standard Arduino interrupt types are supported: CHANGE, RISING, FALLING.
 * 
 * https://github.com/esp8266/Arduino/blob/esp8266/hardware/esp8266com/esp8266/doc/reference.md
 * 
 * 
pinMode(12, INPUT_PULLUP);
attachInterrupt(12, blink, CHANGE);
ESP-12 / pin 12 grounded with switch.
https://github.com/esp8266/Arduino/issues/322
*/    
    Serial.begin(115200);
    delay(10);
    
    // start by connecting to a WiFi network

    
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

	lastSendTime = 0;
}
     
 
     
void loop() {

    digitalWrite(led, digitalRead(PULSE_PIN));

    if ((millis() - lastSendTime > 300000 ) || (lastSendTime == 0)){
        
		lastSendTime = millis();
		Serial.print("connecting to ");
		Serial.println(host);
		
		// Use WiFiClient class to create TCP connections
		WiFiClient client;

		const int httpPort = 80;
		if (!client.connect("184.106.153.149", httpPort)) {
			// se non si connette non resetta i dati
			// li scrive tutti alla prima connessione
			Serial.println("connection failed");
			return;
		}

		// ch tuoNumero
		// This will send the request to the server
		client.print(String("GET /update?key=")+
							WRITE_KEY +
							"&field1=") +
							String(value) + 
							" HTTP/1.1\r\n" +
							"Host: api.thingspeak.com\r\n" + 
							"User-Agent: Mozilla/4.0 (compatible; esp8266 Lua; Windows NT 5.1)\r\n\r\n");

		delay(10);
		
		// Read all the lines of the reply from server and print them to Serial
		while(client.available()){
			String line = client.readStringUntil('\r');
			Serial.print(line);
		}
		
		// if (tx ok)
		value = 0;
		
		Serial.println();
		Serial.println("closing connection");
	}

	
	delay (10);
}
