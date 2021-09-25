import paho.mqtt.client as mqtt
import time

MQTT_ADDRESS = '172.20.10.12'
MQTT_PORT = 1883
MQTT_USER = 'johan'
MQTT_PASSWORD = 'test123'
MQTT_TOPIC = 'home/+/+'

print("test1")
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe(MQTT_TOPIC)

print("test2")


def on_message(client, userdata, message):
    print("Message recieved: " + message.topic + ' ' + str(message.payload))

print("test3")

def main():
    mqtt_client = mqtt.Client()
    mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message

    mqtt_client.connect(MQTT_ADDRESS, 1883)
    mqtt_client.loop_forever()


if __name__ == '__main__':
    print('test4')
    main()