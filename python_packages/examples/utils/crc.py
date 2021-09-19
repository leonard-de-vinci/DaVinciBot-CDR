import dvbcdr.utils.crc as crc

# crc24(topic: str) returns a 24-bit integer associated with the given topic
examples = ["Hello, World!", "dvbcdr"]
for example in examples:
    print(example, "->", crc.crc24(example))

user_topic = input("\nEnter a topic: ")
print(user_topic, "->", crc.crc24(user_topic))
