from dvbcdr.intercom import Intercom, Message

intercom = Intercom()

print("Sending to 'topic_1'...")
intercom.publish(Message("topic_1", "not printed first"))
# this message will not be printed first because run_callbacks will not be
# called before wait_for_topic, which is waiting for 'topic_wait'

print("Sending to 'topic_wait'...")
intercom.publish(Message("topic_wait", "unlock the thread"))

print("Sending to 'topic_1', 'topic_2' and 'topic_3'...")
intercom.publish(Message("topic_1", [1, 2, 3]))
intercom.publish(Message("topic_2", {"a": "b", "c": "d"}))
intercom.publish(Message("topic_3", 4242))
# a message can support every type of data supported by json.dumps
