import matplotlib.pyplot as plt
from subprocess import *
import multiprocessing


"""Sparse coding: n clusters used amongst c clusters for encoding messages"""

ref = False 
X = [x * 1000 for x in range(1, 31)]

def calc_stuff(t):
    stdout = Popen(["../bin/neurons", "-c", "simul5", str(t)], stdout=PIPE).communicate()[0]
    
    output = stdout.splitlines();
    
    for i in range(len(output)):
        output[i] = [float(y) for y in output[i].split(' ')];
    
    return output
        
def subplot(t, m):
    global ref
    pool = multiprocessing.Pool(8)
    res = zip(*pool.map(calc_stuff, [t for x in range(8)]))

    #monte carlo
    Ys = [[x[0] for x in y] for y in res]
    Yrefs = [[x[1] for x in y] for y in res]
    Ys = [sum(x)/float(len(x)) for x in Ys]
    Yrefs = [sum(x)/float(len(x)) for x in Yrefs]
    
    label = ("mode="+str(t))

    if not ref:
        ref = True
        plt.plot(X, Yrefs, "--", label="reference")

    plt.plot(X, Ys, "-", marker=m, label=label)
    
    plt.legend(loc="upper left")
    plt.savefig("simul5-"+str(t)+".png");
    f = open("simul5-" + str(t) + ".txt", "w");
    f.write(str(X) + "\n" + str(Ys) + "\n" + str(Yrefs))

plt.xlabel("Number of learnt messages (M)")
plt.ylabel("Error rate")

subplot(0, 'x')
subplot(2, '^')

plt.show()
