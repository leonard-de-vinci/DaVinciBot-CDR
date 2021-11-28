from abc import ABC
import pygame


class PygCdrObject(ABC):
    def __init__(self, pos, size=None, object=None, cursor_data=None, onclick=None, onhover=None, onenter=None, onleave=None, anchor=(0, 0)):
        self.size = size
        self.x, self.y = pos
        self.anchor = anchor
        self.object = object
        self.cursor_data = cursor_data

        self.onclick = onclick
        self.onhover = onhover
        self.onenter = onenter
        self.onleave = onleave

        self.is_hovered = False

        if pos is not None and size is not None:
            self.rect = pygame.Rect(*(self.compute_pos()), size[0], size[1])
        else:
            self.rect = None

    def compute_pos(self):
        """Computes the position of the object based on its anchor."""
        return (self.x - self.size[0] * self.anchor[0], self.y - self.size[1] * self.anchor[1])

    def show(self, screen):
        """Shows the object on the screen."""
        if self.object is not None:
            screen.blit(self.object, self.compute_pos())

    def clickable(self) -> bool:
        """Returns True if the object is clickable."""
        return self.onclick is not None

    def get_cursor(self):
        """Returns the hover cursor used when the mouse is over this object. It should return a tuple with a cursor id."""
        if self.cursor_data is not None:
            return self.cursor_data
        elif self.clickable():
            return ("tri_left", pygame.cursors.tri_left)
        else:
            return (None, None)

    def detect_mouse(self) -> bool:
        """Returns True if the mouse is over the object."""
        x, y = pygame.mouse.get_pos()
        detected = self.rect is not None and self.rect.collidepoint(x, y)

        if detected:
            if not self.is_hovered and self.onenter is not None:
                self.onenter()
            self.is_hovered = True
            if self.onhover is not None:
                self.onhover()
        else:
            if self.is_hovered and self.onleave is not None:
                self.onleave()
            self.is_hovered = False

        return detected


class PygCdrScene():
    def __init__(self, window):
        self.elements = {}
        self.last_cursor = None
        self.window = window

    def add_element(self, name: str, element: PygCdrObject):
        self.elements[name] = element
        return element

    def remove_element(self, name: str):
        del self.elements[name]

    def __getitem__(self, key):
        return self.elements[key]

    def clicked(self):
        for element in self.elements.values():
            if element.clickable() and element.detect_mouse():
                element.clicked()

    def show(self):
        cursor_got = False
        for element in self.elements.values():
            if element.detect_mouse():
                cursor_got = True
                cursor_id, cursor = element.get_cursor()
                if cursor_id is not None and cursor_id is not self.last_cursor:
                    pygame.mouse.set_cursor(*cursor)
                    self.last_cursor = cursor_id
            element.show(self.window)

        if not cursor_got and self.last_cursor is not "arrow":
            pygame.mouse.set_cursor(*pygame.cursors.arrow)
            self.last_cursor = "arrow"


class PygCdrText(PygCdrObject):
    """Inspired by https://pythonprogramming.altervista.org/buttons-in-pygame/"""

    def __init__(self, text, font_size, bg_color="white", font_color="black", font_name="Arial", **kwargs):
        self.bg_color = bg_color
        self.font_color = font_color
        self.font = pygame.font.SysFont(font_name, font_size)
        self.given_size = "size" in kwargs

        super().__init__(**kwargs)
        self.render_text(text)

    def render_text(self, text):
        self.text_obj = self.font.render(text, 1, pygame.Color(self.font_color))
        text_size = self.text_obj.get_size()
        tx, ty = 0, 0

        if self.given_size:
            tx = (self.size[0] - text_size[0]) // 2
            ty = (self.size[1] - text_size[1]) // 2
        else:
            self.size = text_size

        self.object = pygame.Surface(self.size)
        self.object.fill(self.bg_color)
        self.object.blit(self.text_obj, (tx, ty))
