#include <WiFi.h>

// Local wifi which we will connect to esp
const char* ssid = "t";    
const char* password = "12345678"; 

// Static IP 
IPAddress local_IP(192, 168, 181, 50); 
IPAddress gateway(192, 168, 181, 1);   
IPAddress subnet(255, 255, 255, 0);   

WiFiServer server(80);

void setup() {
  Serial.begin(115200);


  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Failed to configure static IP.");
    return;
  }

  // Hotspot connect
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi connected.");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());  

  
  server.begin();
  Serial.println("Server started.");
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client connected.");
    String request = "";

    //Client request read karega
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;
        if (c == '\n') break; 
      }
    }

    Serial.println("Request received: " + request);

    int inputIndex = request.indexOf("button=");
    if (inputIndex != -1) {
      String inputValue = request.substring(inputIndex + 7, request.indexOf(" ", inputIndex));
      Serial.print("Button Pressed: ");
      Serial.println(inputValue); 
    } else {
      Serial.println("No valid input detected.");
    }

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("<head><title>ESP32 Digital Input</title></head>");
    client.println("<body>");
    client.println("<h2>Select a Button</h2>");
    for (int i = 1; i <= 17; i++) {
      client.print("<button onclick=\"location.href='/?button=");
      client.print(i);
      client.println("'\">");
      client.print("Button ");
      client.print(i);
      client.println("</button><br>");
    }
    client.println("</body>");
    client.println("</html>");
    client.stop();
    Serial.println("Client disconnected.");
  }
}