class Peterson:
    flag = '  '
    turn = ' '

    @thread
    def t1(self):
        while True:
            self.flag = '🏴' + self.flag[1]
            self.turn = '🏳'
            while self.flag[1] != ' ' and self.turn == '🏳':
                pass
            cs = True
            del cs
            self.flag = ' ' + self.flag[1]

    @thread
    def t2(self):
        while True:
            self.flag = self.flag[0] + '🏳'
            self.turn = '🏴'
            while self.flag[0] != ' ' and self.turn == '🏴':
                pass
            cs = True
            del cs
            self.flag = self.flag[0] + ' '

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
