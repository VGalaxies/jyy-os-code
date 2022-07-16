class LockOrdering:
    locks = [ '', '', '' ]

    def tryacquire(self, lk):
        self.locks[lk], seen = '🔒', self.locks[lk]
        return seen == ''

    def release(self, lk):
        self.locks[lk] = ''

    @thread
    def t1(self):
        while True:
            while not self.tryacquire(0): pass
            while not self.tryacquire(1): pass
            while not self.tryacquire(2): pass
            self.release(0), self.release(1), self.release(2)

    @thread
    def t2(self):
        while True:
            while not self.tryacquire(2): pass
            while not self.tryacquire(1): pass
            self.release(1), self.release(2)

    @marker
    def mark_negative(self, state):
        pass
