from dvbcdr.intercom import Intercom


def recv_1(message_data):
    print("received from 'topic_1':", message_data)


def recv_2(message_data):
    print("received from 'topic_2' or 'topic_3':", message_data)


intercom = Intercom()
intercom.subscribe("topic_1", recv_1)
intercom.subscribe(["topic_2", "topic_3"], recv_2)
# subscribe can accept either a list of strings or a string as topic(s)

# wait_for_topic (by default) automatically subscribes to a topic, wait for a
# message to be received on this topic and returns it
message = intercom.wait_for_topic("topic_wait")
print("received from 'topic_wait':", message)

intercom.wait_here()
# wait_here indefinitely blocks the thread and automatically run callbacks
# its equivalent (but not identical) to:
#
# while True:
#     intercom.run_callbacks()
