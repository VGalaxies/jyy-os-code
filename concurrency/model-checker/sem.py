class Semaphore:
    token, waits = 1, ''

    def P(self, tid):
        if self.token > 0:
            self.token -= 1
            return True
        else:
            self.waits = self.waits + tid
            return False

    def V(self):
        if self.waits:
            self.waits = self.waits[1:]
        else:
            self.token += 1

    @thread
    def t1(self):
        self.P('1')
        while '1' in self.waits: pass
        cs = True
        del cs
        self.V()

    @thread
    def t2(self):
        self.P('2')
        while '2' in self.waits: pass
        cs = True
        del cs
        self.V()

    @marker
    def mark_t1(self, state):
        if localvar(state, 't1', 'cs'): return 'blue'

    @marker
    def mark_t2(self, state):
        if localvar(state, 't2', 'cs'): return 'green'

    @marker
    def mark_both(self, state):
        if localvar(state, 't1', 'cs') and localvar(state, 't2', 'cs'):
            return 'red'
