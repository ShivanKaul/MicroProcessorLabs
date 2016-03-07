class KalmanFilter(object):
    q = 0.0
    r = 0.0
    x = 0.0
    p = 0.0
    k = 0.0

    def __init__(self, q=0.1, r=0.1, p=0.1, k=0.0, initial_value=0.0):
        self.q = q
        self.r = r
        self.p = p
        self.x = initial_value

    def update(self, measurement):
        self.p = self.p + self.q
        self.k = self.p / (self.p + self.r)
        self.x = self.x + self.k * (measurement - self.x)
        self.p = (1 - self.k) * self.p

        return self.x
