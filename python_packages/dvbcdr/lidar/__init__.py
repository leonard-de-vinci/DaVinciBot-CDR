"""dvbcdr.lidar
===============

LiDAR object class and methods to manipulate distances and points obtained from the SICK TIM 561 LiDAR.
"""

from .LidarObject import LidarObject
from .lidar_tools import transform_lidar_distances, cluster_lidar_points, clusters_to_objects, filter_objects, distances_to_near_objects
