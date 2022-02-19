from .. import Action


class ActionCustom(Action):
    """
    Allows the execution of custom code during the action flow of a mission.
    """

    def __init__(self, start_callback, tick_callback=None, finish_callback=None):
        """
        Creates an action to execute custom code during the action flow of a mission.

        Each callback is optional, however, if the tick_callback is not provided, the action will finish after the first tick (ie. after start_callack()).

        Args:
            start_callback: The callback to be called when the action starts (called within `action.start()`).
            tick_callback: The callback to be called every tick (called within `action.tick()`).
                It should return True when the action should be finished, False otherwise.
            finish_callback: The callback to be called when the action finishes (called within `action.finish()`).
        """
        self.start_callback = start_callback
        self.tick_callback = tick_callback
        self.finish_callback = finish_callback

    def start(self) -> None:
        self.start_callback()

    def tick(self, ticks) -> bool:
        if self.tick_callback is None:
            return True
        else:
            return self.tick_callback(ticks)

    def finish(self):
        if self.finish_callback is not None:
            self.finish_callback()
