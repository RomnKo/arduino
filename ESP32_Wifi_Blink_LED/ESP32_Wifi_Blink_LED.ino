#include <WiFi.h>

const char* ssid = "ILoveCatsTheMost";
const char* password = "ilovecatsverymuch";

int ledPin = 2; // GPIO2

WiFiServer server(80);

void setup() {
  // Setup serial-output
  Serial.begin(115200);
  delay(10);

  WiFi.mode(WIFI_STA);

  // Pin 2 has an integrated LED - configure it, and turn it off
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Connect to WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Set the hostname
  WiFi.hostname("web-blink");
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
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");

  while (client.connected() && !client.available())
  {
    delay(10);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  // client.flush();

  // Match the request
  int value = LOW;
  if (request.indexOf("/LED=OFF") != -1) {
    digitalWrite(ledPin, LOW);
    value = LOW;
  }
  if (request.indexOf("/LED=ON") != -1) {
    digitalWrite(ledPin, HIGH);
    value = HIGH;
  }

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); // do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><style>");
  // CSS to style the on/off buttons 
  // Feel free to change the background-color and font-size attributes to fit your preferences
  client.println("body { font-family: Helvetica; text-align: center;}");
  client.println("span { padding: 5px; border-radius: 5px; }");
  client.println(".state-on {background-color: blue; color: white}");
  client.println(".state-off {background-color: #eee}");
  client.println(".button {}");
  client.println(".button2 {}");
  client.println("</style></head><body>");

  client.print("Led pin is now: ");

  if (value == HIGH) {
    client.print("<span class=\"state-on\">ON</span>");
  } else {
    client.print("<span class=\"state-off\">OFF</span>");
  }
  client.println("<br><br>");
  client.println("<a href=\"/LED=ON\" class=\"button1\">Turn LED ON</a><br>");
  client.println("<a href=\"/LED=OFF\" class=\"button2\">Turn LED OFF</a><br>");
  client.println("</body></html>");

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}
