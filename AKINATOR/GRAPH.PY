import graphviz as gp

dot = gp.Digraph(comment='AKINATOR')

file = open("akinator.txt", "r")
data = file.read()
data = data.split()
last_node = -1
def grapher(data, i, last_node, label):
    if (i < len(data)):
        while (data[i].isnumeric() == 1):
            i += 1
        if (data[i] != "#"):
            this_node = i
            dot.node(data[i])
            if (last_node != -1):
                dot.edge(data[last_node], data[i], label)
            i = grapher(data, i + 1, this_node, "yes")
            i = grapher(data, i, this_node, "no")
            return i
        return i+1
    return i
    
grapher(data, 0, -1, "yes")
dot.render('akinator_database.png', view=True)
    



    