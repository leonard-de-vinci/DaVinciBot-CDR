from dvbcdr.utils.pygame_ext import PygCdrScene, PygCdrObject, PygCdrText, PygCdrSlider
from dvbcdr.intercom import Intercom
from gui_utils import mouse_to_table, table_to_mouse

import pygame
import os


os.chdir(os.path.dirname(os.path.abspath(__file__)))
pygame.init()

intercom = Intercom()
intercom.wait_in_new_thread()

# CONSTANTS
irl_size = (3000, 2000)
table_size = (1200, 800)
margins = (120, 80)

robot_start_position = table_to_mouse((200, 650), table_size, irl_size, margins)


# PYGAME SETUP
window = pygame.display.set_mode((1200 + 2 * margins[0], 800 + 2 * margins[1]), pygame.HWSURFACE | pygame.DOUBLEBUF)
pygame.display.set_caption("Table Visualizer")
pygame.display.set_icon(pygame.image.load("images/logo_ageofbots.png"))

table_image = pygame.transform.scale(pygame.image.load("images/table.png"), table_to_mouse((3000, 2000), table_size, irl_size))
robot_image = pygame.transform.scale(pygame.image.load("images/dvb.png"), table_to_mouse((300, 300), table_size, irl_size))

running = True
clock = pygame.time.Clock()
scene = PygCdrScene(window)


# LOGIC UTILITIES
statuses = {}
statuses_key = {"cursor_position": "Cursor position"}


def set_status_part(key, value):
    if value is None:
        del statuses[key]
    else:
        statuses[key] = value


# LOGIC SETUP
# if True, the slider will not show the current rotation
# this value should be reset to False when a new position is set or when the requested rotation is reached
is_rotation_requested = False


def table_clicked():
    # [*(tuple)] transforms a tuple into a list/array with the * (= deconstruct) operator
    intercom.publish("move_robot", [*mouse_to_table(pygame.mouse.get_pos(), table_size, irl_size, margins), -1])


def rotation_set(value):
    pass


# SCENE SETUP
status_text = scene.add_element("status_text", PygCdrText("", font_size=15, pos=(3, 3)))
scene.add_element("table", PygCdrObject(object=table_image, pos=(margins[0], margins[1]), size=table_size, cursor_data=("diamond", pygame.cursors.diamond),
                                        onhover=lambda: set_status_part("cursor_position", mouse_to_table(pygame.mouse.get_pos(), table_size, irl_size, margins)),
                                        onleave=lambda: set_status_part("cursor_position", None),
                                        onclick=table_clicked))
scene.add_element("rotation_text", PygCdrText("Rotation (0°):", font_size=15, pos=(3, 25)))
scene.add_element("rotation_slider", PygCdrSlider(0, 359, 1, 0, "white", "grey", "grey40", pos=(130, 25),
                                                  oninput=rotation_set))

scene.add_element("robot_preview", PygCdrObject(object=robot_image, pos=robot_start_position, size=robot_image.get_size(), anchor=(0.5, 0.5)))


# GAME LOOP
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
            break
        elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
            scene.clicked()
        elif event.type == pygame.MOUSEBUTTONUP and event.button == 1:
            scene.released()
        elif event.type == pygame.MOUSEMOTION:
            scene.hover()
    # if we didn't break, we're still running, continue the logic, otherwise, we're not so break the while loop to quit
    else:
        # update logic
        status = ""
        for key, value in statuses.items():
            status += "{}: {}  ".format(statuses_key[key], value)
        status_text.render_text(status)

        # draws the new scene
        window.fill((255, 255, 255))
        scene.show()
        pygame.display.update()

        clock.tick(24)
        continue
    break

pygame.quit()
