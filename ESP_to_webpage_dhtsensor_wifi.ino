#include <DHT11.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "ESP32_Control";
const char* password = "12345678";

WebServer server(80);

// Initialize the DHT11 sensor on Pin 4
DHT11 dht11(4); 

String webpage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 DHT11 Monitor</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body {
            text-align: center;
            font-family: Arial, sans-serif;
            margin-top: 50px;
        }
        .sensor-val {
            font-size: 28px;
            font-weight: bold;
            color: #0076a3;
            margin-top: 20px;
        }
    </style>
</head>
<body>

    <h2>ESP32 Climate Monitor</h2>

    <div class="sensor-val" id="sensorValue">Loading...</div>

    <script>
        function updateSensor() {
            fetch('/sensor')
                .then(response => response.text())
                .then(data => {
                    document.getElementById('sensorValue').innerText = data;
                });
        }
        // Update sensor readings every 2 seconds
        setInterval(updateSensor, 2000);
        updateSensor();
    </script>

</body>
</html>
)rawliteral";

void handleRoot()//handleroot is for sending html code to webpage
{
    server.send(200, "text/html", webpage);
}

void handleSensor()
{
    // Read temperature and humidity as integers (DHT11 returns whole numbers)
    int temperature = dht11.readTemperature();
    int humidity = dht11.readHumidity();

    // Check if the sensor returned an error code
    if (temperature == DHT11::ERROR_TIMEOUT || temperature == DHT11::ERROR_CHECKSUM ||
        humidity == DHT11::ERROR_TIMEOUT || humidity == DHT11::ERROR_CHECKSUM) {
        server.send(200, "text/plain", "Failed to read sensor");
        return;
    }

    // Combine readings into a single simple string
    String reading = "Temp: " + String(temperature) + " C | Humidity: " + String(humidity) + "%";
    server.send(200, "text/plain", reading);
}

void setup()
{
    Serial.begin(115200);

    // Start Wi-Fi Access Point
    WiFi.softAP(ssid, password);
    Serial.print("IP adress:");
    Serial.println(WiFi.softAPIP());
    server.on("/", handleRoot);
    server.on("/sensor", handleSensor);

    server.begin();
}

void loop()
{
    server.handleClient();//handleclient is for starting the html code which is sent
}