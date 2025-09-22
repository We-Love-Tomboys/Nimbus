import requests
import time
from datetime import datetime
import csv

# Your ESP32 API URL
url = "http://192.168.29.63/data"

while True:
    try:
        response = requests.get(url)
        data = response.json()

        temperature = data["temperature"]
        humidity = data["humidity"]
        timestamp = data["timestamp"]

        print(f"{timestamp} | Temperature: {temperature}°C | Humidity: {humidity}%")
        with open("weather_data.csv", "a", newline="") as file:
            writer = csv.writer(file)
            writer.writerow([timestamp, temperature, humidity])

    except Exception as e:
        print("Error fetching data:", e)

    # Wait for 5 seconds before fetching again
    time.sleep(5)

# Note: Ensure that the ESP32 is connected to the same network as this script.
