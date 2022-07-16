import inspect, ast, astor, copy, sys
from pathlib import Path

threads, marker_fn = [], []


def thread(fn):
    '''Decorate a member function as a thread'''
    global threads
    threads.append(fn.__name__)
    return fn


def marker(fn):
    '''Decorate a member function as a state marker'''
    global marker_fn
    marker_fn.append(fn)


def localvar(s, t, varname):
    '''Return local variable value of thread t in state s'''
    return s.get(t, (0, {}))[1].get(varname, None)


def checkpoint():
    '''Instrumented `yield checkpoint()` goes here'''
    f = inspect.stack()[1].frame  # stack[1] is the caller of checkpoint()
    return (f.f_lineno, {k: v for k, v in f.f_locals.items() if k != 'self'})


def hack(Class):
    '''Hack Class to instrument @mc.thread functions'''

    class Instrument(ast.NodeTransformer):

        def generic_visit(self, node, in_fn=False):
            if isinstance(node, ast.FunctionDef):
                if node.name in threads:
                    # a @mc.thread function -> instrument it
                    in_fn, node.decorator_list = True, []
                elif node.decorator_list:
                    # a decorated function like @mc.mark -> remove it
                    return None

            body = []
            for line in getattr(node, 'body', []):
                # prepend each line with `yield checkpoint()`
                if in_fn:
                    body.append(
                        ast.Expr(
                            ast.Yield(
                                ast.Call(func=ast.Name(checkpoint.__name__,
                                                       ctx=ast.Load()),
                                         args=[],
                                         keywords=[]))))
                body.append(self.generic_visit(line, in_fn))
            node.body = body
            return node

    if not hasattr(Class, 'hacked'):
        hacked_ast = Instrument().visit(ast.parse(Class.source))
        hacked_src, vars = astor.to_source(hacked_ast), {}
        # set a breakpoint() here to see **magic happens**!
        exec(hacked_src, globals(), vars)
        Class.hacked, Class.hacked_src = vars[Class.__name__], hacked_src
    return Class


def execute(Class, trace):
    '''Execute trace (like [0,0,0,2,2,1,1,1]) on Class'''

    def attrs(obj):
        for attr in dir(obj):
            val = getattr(obj, attr)
            if not attr.startswith('__') and type(val) in [
                    bool, int, str, list, tuple, dict
            ]:
                yield attr, val

    obj = hack(Class).hacked()
    for attr, val in attrs(obj):
        setattr(obj, attr, copy.deepcopy(val))

    T = []
    for t in threads:
        fn = getattr(obj, t)
        T.append(fn())  # a generator for a thread
    S = {t: T[i].__next__() for i, t in enumerate(threads)}

    while trace:
        chosen, tname, trace = trace[0], threads[trace[0]], trace[1:]
        try:
            if T[chosen]:
                S[tname] = T[chosen].__next__()
        except StopIteration:
            S.pop(tname)
            T[chosen] = None

    for attr, val in attrs(obj):
        S[attr] = val
    return obj, S


class State:

    def __init__(self, Class, trace):
        self.trace = trace
        self.obj, self.state = execute(Class, trace)
        self.name = f's{abs(State.freeze(self.state).__hash__())}'

    @staticmethod
    def freeze(obj):
        '''Create an object's hashable frozen (immutable) counterpart'''
        if obj is None or type(obj) in [str, int, bool]:
            return obj
        elif type(obj) in [list, tuple]:
            return tuple(State.freeze(x) for x in obj)
        elif type(obj) in [dict]:
            return tuple(
                sorted(zip(obj.keys(),
                           (State.freeze(v) for v in obj.values()))))
        raise ValueError('Cannot freeze')


def serialize(Class, s0, vertices, edges):
    '''Serialize all model checking results'''
    print(f'CLASS({repr(Class.hacked_src)})')

    sid = {s0.name: 0}

    def name(s):
        if s.name not in sid:
            sid[s.name] = len(sid)
        return repr(f's{sid[s.name]}')

    for u in vertices.values():
        mk = [f(u.obj, u.state) for f in marker_fn if f(u.obj, u.state)]
        print(f'STATE({name(u)}, {repr(u.state)}, {repr(mk)})')

    for u, v, chosen in edges:
        print(f'TRANS({name(u)}, {name(v)}, {repr(threads[chosen])})')


def check_bfs(Class):
    '''Enumerate all possible thread interleavings of @mc.thread functions'''
    s0 = State(Class, trace=[])

    # breadth-first search to find all possible thread interleavings
    queue, vertices, edges = [s0], {s0.name: s0}, []
    while queue:
        u, queue = queue[0], queue[1:]
        for chosen, _ in enumerate(threads):
            v = State(Class, u.trace + [chosen])
            if v.name not in vertices:
                queue.append(v)
                vertices[v.name] = v
            edges.append((u, v, chosen))

    serialize(Class, s0, vertices, edges)


src, vars = Path(sys.argv[1]).read_text(), {}
exec(src, globals(), vars)
Class = [C for C in vars.values() if type(C) == type].pop()
setattr(Class, 'source', src)
check_bfs(Class)
