from ...action import Action


class ActionCustom(Action):
    def __init__(self, start_callback, tick_callback=None, finish_callback=None):
        self.start_callback = start_callback
        self.tick_callback = tick_callback
        self.finish_callback = finish_callback

    def start(self) -> None:
        self.start_callback()

    def tick(self) -> bool:
        if self.tick_callback is None:
            return True
        else:
            return self.tick_callback()

    def finish(self):
        if self.finish_callback is not None:
            self.finish_callback()
