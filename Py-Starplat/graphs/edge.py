class Edge:
    def __init__(self):
        self.src = None
        self.dest = None
        self.weight = None
        self.id = None
        self.dir = None

    
    def __repr__(self):
        return '<src: {}, dest: {}, weight: {}>'.format(self.src, self.dest, self.weight)

