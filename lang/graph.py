import graphviz as gp

dot = gp.Digraph(comment='AKINATOR')
file = open("tree.txt", "r")
data = file.read()
data = data.split()
last_node = -1
def grapher(data, i, last_node, label):
    if (i < len(data)):
        if (data[i] != "#"):
            this_node = i
            dot.node(str(i), data[i])
            if (last_node != -1):
                #dot.edge(data[last_node], data[i], label)
                dot.edge(str(last_node), str(i), label=label)
            i = grapher(data, i + 1, this_node, "L")
            i = grapher(data, i, this_node, "R")
            return i
        return i+1
    return i
grapher(data, 0, -1, "L")
dot.render('akinator_database.png', view=True)