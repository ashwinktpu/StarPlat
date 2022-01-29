from sys import flags


f = open("Grammar", "r")
fW = open("Grammar.txt", "w")

flag = True

while True:

    c = f.read(1)

    if not c:
        break

    if c == '{':
        flag = False
    elif c == '}':
        flag = True
        c = f.read(1)
        while c != ';':
            c = f.read(1)

    elif flag:
        fW.write(c)

f.close()
fW.close()
