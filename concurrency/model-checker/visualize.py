import sys, re, graphviz, jinja2, json, markdown, argparse
from pathlib import Path
from collections import namedtuple

TEMPLATE = Path('template.html').read_text()
EMPTY = '␡'
COLORS = {
    None: '#f1f5f9',
    'red': '#fecaca',
    'yellow': '#fef08a',
    'green': '#bbf7d0',
    'blue': '#bfdbfe',
    'purple': '#e9d5ff'
}

Vertex = namedtuple('Vertex', 'name state marks')
Edge = namedtuple('Edge', 'name u v t')
State = namedtuple('State', 'pcs lvars gvars')

vertices, edges, threads = {}, [], []
code_lines, pcmap, gvar, lvar = [], {}, [], []


def parse_input():

    def CLASS(cl):
        global hacked_src
        hacked_src = cl

    def STATE(u, state, marks):
        vertices[u] = Vertex(name=u, state=state, marks=marks)

    def TRANS(u, v, t):
        edges.append(Edge(name=f'{u}-{v}', u=vertices[u], v=vertices[v], t=t))
        if t not in threads:
            threads.append(t)

    for line in sys.stdin.readlines():
        eval(line)


def parse_src():
    md_lines = ['    :::python']
    for line in hacked_src.splitlines():
        if 'yield' not in line:
            md_lines.append(f'    {line.replace("    ", "  ")}')
    html = markdown.markdown('\n'.join(md_lines), extensions=['codehilite'])
    code_lines.extend(
        re.search(r'<code.*?>(.*)</code>', html,
                  re.DOTALL).group(1).rstrip().splitlines())

    new_pc = 0
    for pc, line in enumerate(hacked_src.splitlines()):
        if 'yield' not in line:
            pcmap[pc + 1] = (new_pc := new_pc + 1)


def parse_vars():
    gv, lv = set(), set()
    for v in vertices.values():
        for t, val in v.state.items():
            if t not in threads: gv.add(t)
            else: lv |= set((t, var) for var in val[1])
    gvar.extend(sorted(list(gv)))
    lvar.extend(sorted(list(lv)))


def parse_state(s):
    gvars = [s[g] for g in gvar]
    lvars = ['⊥' for _ in enumerate(lvar)]
    pcs = [-1 for _ in enumerate(threads)]

    for t in [t for t in threads if t in s]:
        tpc, tstate = s[t]
        pcs[threads.index(t)] = pcmap[tpc + 1]
        for vname, val in tstate.items():
            lvars[lvar.index((t, vname))] = val

    return State(pcs, lvars, gvars)


def reduce(tree_only=False):
    tree, depth, leaf = [], {'s0': 0}, {'s0': True}
    updated, rnd = 0, 0
    while updated + len(threads) > rnd:
        rnd += 1

        def expand():
            found = False
            for e in edges:
                _, u, v, t = e
                if u.name in depth and v.name not in depth and t == threads[
                        -rnd % len(threads)]:
                    depth[v.name] = depth[u.name] + 1
                    leaf[v.name] = True
                    leaf[u.name] = False
                    tree.append(e)
                    found = True
            return found

        while expand():
            updated = rnd

    others = []
    if not tree_only:
        tnames = set(e.name for e in tree)
        for e in edges:
            name, u, v, t = e
            if name not in tnames:
                w = filter(lambda e1: e1.v.name == u.name, edges).__next__()
                if (leaf[e.u.name]
                        and w.t == e.t) or (depth[u.name] >= depth[v.name]):
                    others.append(e)
    return tree, others


parse_input()
parse_src()
parse_vars()

parser = argparse.ArgumentParser(
    description='Visualize modeler checker outputs')
parser.add_argument('--tree', '-t', help='Draw tree', action='store_true')
parser.add_argument('--reduce',
                    '-r',
                    help='Draw reduced graph',
                    action='store_true')
args = parser.parse_args()

if args.reduce or args.tree:
    edges, others = reduce(args.tree)
else:
    edges, others = edges, []

COLORS |= dict(zip(threads, ['#be123c', '#1d4ed8', '#eab308', '#4d7c0f']))

g = graphviz.Digraph('G',
                     filename='/tmp/a.gv',
                     graph_attr={
                         'fontsize': '12',
                         'layout': 'dot',
                         'nodesep': '0.75'
                     })
metadata = {'s0': vertices['s0'].name}

with g.subgraph(name='legend') as c:
    c.node_attr = {
        'style': 'filled',
        'fontname': 'Courier New',
        'shape': 'Mrecord'
    }

    # draw vertices and code blocks
    for v in vertices.values():
        ps = parse_state(v.state)
        color = ([None] + [col for col in v.marks if col])[-1]

        gvals, lvals = '| '.join([f'{g}={v.state[g]}' for g in gvar]), ''
        for i, t in enumerate(threads):
            if i != 0: lvals += '|'
            lvals += '{' + '| '.join([
                f'{t}.{l}={v.state.get(t, (0, {}))[1].get(l, EMPTY)}'
                for t1, l in lvar if t1 == t
            ]) + '}'
        label = '{' + '{' + gvals + '}' + '|' + '{' + lvals + '}' + '}'
        label = label.replace('True', 'T').replace('False', 'F')
        c.node(v.name,
               id=v.name,
               label=label,
               fillcolor=COLORS.get(color, color))

        lines = ['<div class="codehilite">']
        for i, line in enumerate(code_lines):
            cl = 'new' if i + 1 in ps.pcs else ''
            lines.append(
                f'<pre class="{cl}"><code>{line if line else " "}</pre></code>'
            )
        lines.append('<div class="vars">')
        for i, nm in enumerate(gvar):
            lines.append(f'<pre><code>{nm} = {ps.gvars[i]}</code></pre>')
        for i, (t, nm) in enumerate(lvar):
            lines.append(f'<pre><code>{t}.{nm} = {ps.lvars[i]}</code></pre>')
        lines.append('</div></div>')
        metadata[v.name] = '\n'.join(lines)

    # draw edges
    for idx, (name, u, v, t) in enumerate(edges + others):
        ps_old, ps_new = parse_state(u.state), parse_state(v.state)
        c.edge(u.name,
               v.name,
               id=name,
               fontcolor=COLORS[t],
               color=COLORS[t],
               label=t,
               fontname='Helvetica')

        diff = lambda x, y: f'<span class="new">{y}</span> <span class="old">({x})</span>' if x != y else x
        lines = ['<div class="codehilite">']
        for i, line in enumerate(code_lines):
            if i + 1 in ps_new.pcs: cl = 'new'
            elif i + 1 in ps_old.pcs: cl = 'old'
            else: cl = ''
            lines.append(
                f'<pre class="{cl}"><code>{line if line else " "}</pre></code>'
            )
        lines.append('<div class="vars">')
        for i, nm in enumerate(gvar):
            lines.append(
                f'<pre><code>{nm} = {diff(ps_old.gvars[i], ps_new.gvars[i])}</code></pre>'
            )
        for i, (tname, nm) in enumerate(lvar):
            lines.append(
                f'<pre><code>{tname}.{nm} = {diff(ps_old.lvars[i], ps_new.lvars[i])}</code></pre>'
            )
        lines.append('</div></div>')
        metadata[name] = '\n'.join(lines)

svg_file = Path(g.render(format='svg'))
html = jinja2.Template(TEMPLATE).render(svg=svg_file.read_text(),
                                        data=json.dumps(metadata))
print(html)
