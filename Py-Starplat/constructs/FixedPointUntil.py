class FixedPointUntil:
    def __init__(self, condition):
        self.condition = condition

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        pass

    def run(self, block):
        while not self.condition():
            block()