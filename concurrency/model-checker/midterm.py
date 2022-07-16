class Midterm:
    s, s1, s2 = 0, 0, 0

    @thread
    def t1(self):
        self.s1 = self.s
        self.s1 += 1
        self.s = self.s1

        self.s1 = self.s
        self.s1 += 1
        self.s = self.s1

        self.s1 = self.s
        self.s1 += 1
        self.s = self.s1

    @thread
    def t2(self):
        self.s2 = self.s
        self.s2 += 1
        self.s = self.s2

        self.s2 = self.s
        self.s2 += 1
        self.s = self.s2

        self.s2 = self.s
        self.s2 += 1
        self.s = self.s2

    @marker
    def mark(self, state):
        if self.s == 2:
            return 'red'
