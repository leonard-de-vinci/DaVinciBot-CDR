from dvbcdr.intercom import Intercom


def recv_1(message_data):
    print("received from 'topic_1' or 'topic_number':", message_data)


def recv_2(message_data):
    print("received from 'topic_2' or 'topic_3' or 'test_topic':", message_data)


intercom = Intercom()
intercom.subscribe(["topic_1", "topic_number"], recv_1)
intercom.subscribe(["topic_2", "topic_3", "test_topic"], recv_2)
# subscribe can accept either a list of strings or a string as topic(s)

# wait_for_topic (by default) automatically subscribes to a topic, wait for a
# message to be received on this topic and returns it
# message = intercom.wait_for_topic("topic_wait")
# print("received from 'topic_wait':", message)

print("waiting here")
intercom.wait_here()
# wait_here indefinitely blocks the thread and automatically run callbacks
# its equivalent (but not identical) to:
#
# while True:
#     intercom.run_callbacks()
