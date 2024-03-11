

class Ventrilator:
    def __init__(self, auto = False, valueTarget = 30):
        self.auto = auto
        self.valueTarget = valueTarget

    def setMode(self, mode, valueTarget = 30):
        self.auto = mode
        self.valueTarget = valueTarget


    def getMode(self):
        return self.auto


    def getValueTarget(self):
        return self.valueTarget