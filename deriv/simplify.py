import sympy as sym


data = open("output.txt")
output = open("simplified.txt", "w")
expr = data.readlines()
for i in range (2):
    print(sym.latex(sym.simplify(expr[i])), file = output)