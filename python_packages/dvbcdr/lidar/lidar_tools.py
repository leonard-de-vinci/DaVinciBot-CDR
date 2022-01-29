from math import cos, sin, radians
from typing import Any, Dict, List, Tuple
from sklearn.cluster import DBSCAN
from .LidarObject import LidarObject

Point = Tuple[int, int]


def transform_lidar_distances(distances: List[int], start_angle: float, angle_div: float) -> List[Point]:
    """
    Transforms distances to points around the origin of all the distances.

    Points are distributed on circles with angles starting at `start_angle` and separated with an angle of `angle_div`.

    Args:
        distances: A list of distances of objects from a point in space.
        start_angle: The angle of the first distance in degrees.
        angle_div: The angle between each distance line in degrees.

    Returns:
        A list of tuples with 2 coordinates. The origin is the point from which the distances were measured.
    """
    angle = start_angle
    relative_points = []
    for val in distances:
        if val < 0.01:
            continue

        relative_points.append((int(cos(radians(angle)) * val * 100), int(sin(radians(angle)) * val * 100)))
        angle = (angle - angle_div) % 360

    return relative_points


def cluster_lidar_points(points: List[Point], eps: int = 5) -> List[List[Point]]:
    """
    Performs the DBSCAN algorithm on a list of points to find cluster of points.

    Args:
        points: The list of points (tuple of 2 int) to cluster.
        eps: The eps parameter given to the DBSCAN instance.

    Returns:
        A list of clusters, each cluster being itself a list of points.
    """
    labels = DBSCAN(eps).fit_predict(points)
    clusters = {}

    for i in range(len(points)):
        point = points[i]
        label = labels[i]

        if label in clusters:
            clusters[label].append(point)
        else:
            clusters[label] = [point]
    return clusters.values()


def clusters_to_objects(clusters: List[List[Point]]) -> List[LidarObject]:
    return [LidarObject(x) for x in clusters]


def filter_objects(objects: List[LidarObject], distance_thresold: int = 30) -> List[LidarObject]:
    """
    Filters a list of `LidarObject`, keeping only close objects.

    Args:
        objects: A list of `LidarObject`.
        distance_thresold: The maximum distance to consider an object close.

    Returns:
        A list of close objects based on `distance_thresold`.
    """
    return [obj for obj in objects if any(d < distance_thresold for d in obj.distances)]


def distances_to_near_objects(distances: List[int], start_angle: float, angle_div: float, eps: int = 5, distance_threshold: int = 30) -> List[LidarObject]:
    """
    One-liner to convert LiDAR distances (a list of int) into a list of close `LidarObject` instances.

    Automatically calls the methods required to perform all the steps of the conversion.

    Args:
        distances: The distances obtained from the LiDAR.
        start_angle: The angle of the first distance line from the LiDAR.
        angle_div: The angle between each distance line of the LiDAR.
        eps: Algorithm parameter passed to the DBSCAN cluster method.
        distance_thresold: Maximum distance for an object to be considered close.

    Returns:
        A list of close objects measured with the LiDAR.
    """
    objects = clusters_to_objects(cluster_lidar_points(transform_lidar_distances(distances, start_angle, angle_div), eps))
    return filter_objects(objects, distance_threshold)
