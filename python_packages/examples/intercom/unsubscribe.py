import time
from dvbcdr.intercom import intercom_instance as intercom


def recv(data):
    print("received:", data)


ref_a = intercom.subscribe("topic", recv)
ref_b = intercom.subscribe("topic", recv)

intercom.wait_in_new_thread()

print("Sending to both callbacks...")
intercom.publish("topic", "received twice")

time.sleep(1)

print("Unsubscribing...")
intercom.unsubscribe(ref_a)
intercom.publish("topic", "received only once")

time.sleep(1)
