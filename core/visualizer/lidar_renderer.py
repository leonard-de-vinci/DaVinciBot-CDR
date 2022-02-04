from math import radians, cos, sin
from typing import Dict, List, Tuple
from dvbcdr.lidar import LidarObject, distances_to_near_objects
from gui_utils import table_to_mouse

import pygame


draw_colors = [(0, 0, 0), (0, 0, 0)]  # [(55, 126, 184), (77, 175, 74)]


def draw(surface: pygame.Surface, robot_positions: List[Tuple[int, int, int]], messages_dict: Dict[int, Tuple[int, int, List[int]]], table_size: Tuple[int, int], irl_size: Tuple[int, int]):
    surface.fill((0, 0, 0, 0))

    for robot_id, message in messages_dict.items():
        objects = distances_to_near_objects(message[2], message[0], message[1], 50, 3000)
        robot_position = robot_positions[robot_id]
        angle = radians(robot_position[2])
        for object in objects:
            # position = object.special_points[1]
            # position = (position[0] * 10, position[1] * 10)
            # position = table_to_mouse(position, table_size, irl_size)
            # position = (position[0] + 1120, position[1] + 380)
            # pygame.draw.circle(surface, draw_colors[robot_id], position, 3)
            for position in object.points:
                x, y = position[0], position[1]
                rx, ry = int(x * cos(angle) - y * sin(angle)), int(y * cos(angle) + x * sin(angle))
                screen_pos = table_to_mouse((rx + robot_position[0], ry + robot_position[1]), table_size, irl_size)
                pygame.draw.circle(surface, draw_colors[robot_id], screen_pos, 2)


def generate_draw_callback(robot_positions, messages_dict, table_size, irl_size):
    return lambda surface: draw(surface, robot_positions, messages_dict, table_size, irl_size)
