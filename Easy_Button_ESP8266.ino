#include <ESP8266WiFi.h>

const char* ssid = "vrc-mobile";
const char* password = "your-password";

// Create an instance of the server listening on port 80
WiFiServer server(80);

// Define the pins
#define ONE 1
#define TWO 2
#define INTERRUPT 3

void setup() 
{
  Serial.begin(9600);
  delay(10);

  // Define the sound trigger pins as outputs
  pinMode(ONE, OUTPUT);
  pinMode(TWO, OUTPUT);
  pinMode(INTERRUPT, OUTPUT);

  // Set the initial output state
  digitalWrite(ONE, LOW);
  digitalWrite(TWO, LOW);
  digitalWrite(INTERRUPT, HIGH);

  // Connect to WiFi
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
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  // response string
  String response = "";
  // correct request
  bool correct = true;
  if (!client) {
    return;
  }

    // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Match the request
  int val;
  if (req.indexOf("/1") != -1)
    val = 1;
  else if (req.indexOf("/2") != -1)
    val = 2;
  else if (req.indexOf("/3") != -1)
    val = 3;
  else if (req.indexOf("/4") != -1)
    val = 4;
  else {
    response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n";
    correct = false;
    Serial.println("invalid request");
  }

  // If the request was ok, set the pin state and a positive response, else
  // send a 404 response.
  if (correct)
  {
    // Set the output pins to the requested file before firing the interrupt
    digitalWrite(TWO,((val & 2) >> 1));
    digitalWrite(ONE,(val & 1));
    // Fire the interrupt
    digitalWrite(val, 0);
    
    client.flush();
  
    // Prepare the response
    response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html></html>\n";
  
    // Send the response to the client
    client.print(response);
    delay(1);
    Serial.println("Client disonnected");
  
    // The client will actually be disconnected 
    // when the function returns and 'client' object is detroyed
  } else {
    client.flush();

    // Sent the response to the client
    client.print(response);
    delay(1);
    Serial.println("Client disconnected");
  }
}
