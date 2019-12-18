import graphviz as gp

dot = gp.Digraph(comment='AKINATOR')
file = open("akinator.txt", "r")
data = file.read()
def grapher(data, offset, dir, num, flag):
    if (data[offset] == '{'):
        offset += 1
        a = data.find("{", offset)
        b = data.find("}", offset)
        if (a < b):
            dot.node(num, data[offset + 1:a])
            if (flag):
            num += 1
            offset = a
            offset = grapher(data, offset, 0, num, 1)
            offset = grapher(data, offset, 1, num)
    return offset





    
grapher(data, 0, 0, 0)
dot.render('akinator_database.png', view=True)
    



    