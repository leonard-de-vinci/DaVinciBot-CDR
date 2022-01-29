from math import sqrt, degrees, atan2


class LidarObject:
    """
    Represents an object detected by the LiDAR.

    An object is a a cluster of points processed by a clustering algorithm like DBSCAN.
    """

    def __init__(self, points):
        self.points = points

        distances = []
        angles = []

        first_point = points[0]
        distances.append(sqrt(first_point[0] ** 2 + first_point[1] ** 2))
        angles.append(degrees(atan2(first_point[1], first_point[0])) % 360)

        center_point = (sum([x[0] for x in points]) / len(points), sum([x[1] for x in points]) / len(points))
        distances.append(sqrt(center_point[0] ** 2 + center_point[1] ** 2))
        angles.append(degrees(atan2(center_point[1], center_point[0])) % 360)

        last_point = points[-1]
        distances.append(sqrt(last_point[0] ** 2 + last_point[1] ** 2))
        angles.append(degrees(atan2(last_point[1], last_point[0])) % 360)

        self.special_points = [first_point, center_point, last_point]
        self.distances = distances
        self.angles = angles
