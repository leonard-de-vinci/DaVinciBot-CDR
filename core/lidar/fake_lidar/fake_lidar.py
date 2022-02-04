from cmath import sqrt
import os

from pygame.time import Clock
from dvbcdr.intercom import Intercom


ROBOT_ID = int(os.environ.get("CDR_ROBOT_ID", "0"))

CLOCK_FPS = max(min(int(os.environ.get("CDR_LIDAR_FPS", "30")), 55), 0)
clock = Clock()

print(f"Sending fake values for robot {ROBOT_ID}.")
print(f"Running at {CLOCK_FPS} Hz.")

intercom = Intercom()

with open("fake_points.txt", "r") as f:
    distances = [int(sqrt(int(x[0]) ** 2 + int(x[1]) ** 2).real) for x in [x.split(" ") for x in f.readlines() if len(x) > 0]]

while True:
    intercom.publish("lidar_distances", [ROBOT_ID, -45, 270 / len(distances), distances])
    clock.tick(CLOCK_FPS)
