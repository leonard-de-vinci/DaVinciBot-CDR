import os
import pysicktim as lidar

from pygame.time import Clock
from dvbcdr.intercom import Intercom


ROBOT_ID = int(os.environ.get("CDR_ROBOT_ID", "0"))

CLOCK_FPS = max(min(int(os.environ.get("CDR_LIDAR_FPS", "24")), 55), 0)
clock = Clock()

print(f"Connected to the LiDAR for robot {ROBOT_ID}.")
print(f"Running at {CLOCK_FPS} Hz.")

intercom = Intercom()

while True:
    lidar.scan()
    distances = [int(x * 100) for x in lidar.scan.distances]
    intercom.publish("lidar_distances", [ROBOT_ID, lidar.scan.dist_start_ang, lidar.scan.dist_angle_res, distances])

    clock.tick(CLOCK_FPS)
