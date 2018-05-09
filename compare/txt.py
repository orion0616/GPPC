with open('base.log','r') as fin1:
    base = fin1.readlines()
with open('my.log','r') as fin2:
    my = fin2.readlines()
with open('total.log', 'w') as f:
    for b,m in zip(base,my):
        f.write(b.strip() + '  ' + m.strip() + '\n')
