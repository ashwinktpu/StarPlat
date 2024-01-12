class FixedPoint:
    def isFinished(self):
        finished = True
        for v in self.g.modified.values():
            finished = finished and (not v)

        return finished

    def __init__(self, body, g, finished):
        self.finished = finished
        self.g = g
        self.body = body

        # Define the globals and locals dictionaries for the exec call
        exec_globals = {'g': g}
        exec_locals = {}

        while not self.finished:
            exec(self.body, exec_globals, exec_locals)
            self.finished = self.isFinished()