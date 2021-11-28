from dvbcdr.utils.pygame_ext import PygCdrScene, PygCdrObject, PygCdrText
from gui_utils import mouse_to_table, table_to_mouse

import pygame
import os


os.chdir(os.path.dirname(os.path.abspath(__file__)))
pygame.init()

irl_size = (300, 200)
table_size = (1200, 800)
margins = (120, 80)

robot_start_position = table_to_mouse((20, 65), table_size, irl_size, margins)

window = pygame.display.set_mode((1200 + 2 * margins[0], 800 + 2 * margins[1]), pygame.HWSURFACE | pygame.DOUBLEBUF)
pygame.display.set_caption("Table Visualizer")
pygame.display.set_icon(pygame.image.load("images/logo_ageofbots.png"))

table_image = pygame.transform.scale(pygame.image.load("images/table.png"), table_to_mouse((300, 200), table_size, irl_size))
robot_image = pygame.transform.scale(pygame.image.load("images/dvb.png"), table_to_mouse((30, 30), table_size, irl_size))

running = True
clock = pygame.time.Clock()
scene = PygCdrScene(window)

cursor_pos_text = scene.add_element("cursor_pos_text", PygCdrText("", font_size=15, pos=(3, 3)))
scene.add_element("table", PygCdrObject(object=table_image, pos=(margins[0], margins[1]), size=table_size, cursor_data=("diamond", pygame.cursors.diamond),
                                        onhover=lambda: cursor_pos_text.render_text("Cursor position: {}".format(mouse_to_table(pygame.mouse.get_pos(), table_size, irl_size, margins))), onleave=lambda: cursor_pos_text.render_text("")))

scene.add_element("robot_preview", PygCdrObject(object=robot_image, pos=robot_start_position, size=robot_image.get_size(), anchor=(0.5, 0.5)))
print(robot_image.get_size())

while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
            break
    # if we didn't break, we're still running, continue the logic, otherwise, we're not so break the while loop to quit
    else:
        window.fill((255, 255, 255))
        scene.show()
        pygame.display.update()

        clock.tick(24)
        continue
    break

pygame.quit()
