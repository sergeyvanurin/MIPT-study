import tokenize as tok


s = open("pseudo_code.txt")
output = open("output.txt", "w")
def decistmt(s):
    result = []
    g = tok.generate_tokens(s.readline)
    for toknum, tokval, _, _, _ in g:
        if toknum == tok.NUMBER and '.' in tokval:
            result.extend([
                (tok.NAME, 'Decimal'),
                (tok.OP, '('),
                (tok.STRING, repr(tokval)),
                (tok.OP, ')')
            ])
        else:
            result.append((toknum, tokval))
    return result

data = decistmt(s)

for tup in data:
    if (tup[1] != '\n' and tup[1] != ''):
        print(tup[1], file = output, end = '@')
print("END", file = output, end = '@')