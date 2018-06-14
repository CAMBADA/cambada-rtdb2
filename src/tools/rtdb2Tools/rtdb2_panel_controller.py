class RtDBPanelController():
    def __init__(self, max_vpos = None, max_hpos = None):
        self.vpos = 0
        self.hpos = 0
        self.max_vpos = max_vpos
        self.max_hpos = max_hpos

    def reset(self):
        self.vpos = 0
        self.hpos = 0

    def increment_vertical(self, page = 1):
        if self.max_vpos is not None:
            self.vpos = self.vpos + page if self.vpos + page < self.max_vpos else self.max_vpos - 1
    def decrement_vertical(self, page = 1):
        self.vpos = 0 if self.vpos - page < 0 else self.vpos - page
    def increment_horizontal(self, page = 1):
        if self.max_hpos is not None:
            self.hpos = self.hpos + page if self.hpos + page < self.max_hpos else self.max_hpos - 1
    def decrement_horizontal(self, page = 1):
        self.hpos = 0 if self.hpos - page < 0 else self.hpos - page
