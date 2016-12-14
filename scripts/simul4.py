import matplotlib.pyplot as plt
from subprocess import *
import multiprocessing
import numpy as np

"""Sparse coding: n clusters used amongst c clusters for encoding messages"""

prel = 0
succ = 2
density = True 

def calc_stuff(args):
    nbmess, c, l, n, k, m, i, tries, r = args
    print ("c=", c, ", l=", l, ", n=", n, ", k=", k, ", mess: ", nbmess, ", prel: ", 
                prel, ",r: ", r)
    
    stdout = Popen(["../bin/neurons", "-c", "simul4", str(c), str(l), str(n),
                          str(k), str(nbmess), str(i), str(prel), str(tries),
                          str(r), str(succ)], stdout=PIPE).communicate()[0]
    
    output = stdout.split(' ');
    
    print output
    
    y = float(output[0])
    d = float(output[1])
    it = float(output[2])/100
    
    print y,d,it
    return y, d, it
        
def subplot(c, l, n, k, m, i, r):
    global density
    pool = multiprocessing.Pool(3)
    res = [zip(*pool.map(calc_stuff, [(nbmess, c, l, n, k, m, i, 200, r) for nbmess in X]))
                    for counter in range(100)]
                
    #monte carlo
    Ys = [x[0] for x in res]
    Ds = [x[1] for x in res]
    Is = [x[2] for x in res]
    Y = [sum(x)/float(len(x)) for x in zip(*Ys)]
    D = [sum(x)/float(len(x)) for x in zip(*Ds)]
    I = [sum(x)/float(len(x)) for x in zip(*Is)]
    
    label = ("X="+str(c) + ", l=" + str(l) + ", c=" + str(n) + ", kc=" + str(k))
    if prel:
        label += " (" + str(prel) + "%, " + str(succ) + " iter)"
    if r:
        label += " (r: " + str(r) + ")"
    if not density:
        density = True
        plt.plot(X, D, "--", label="density")

    plt.plot(X, Y, "-", marker=m, label=label)
    if 0:
        plt.plot(X, I, "--", marker=m, label=label + " (it)")
    
    plt.legend(loc="upper left")
    plt.savefig("simul4-"+str(succ)+"it-"+str(prel)+"prel.png");
    f = open("simul4-" + str(succ) + "it-"+str(prel)+"prel.txt", "w");
    f.write(str(X) + "\n" + str(Y) + "\n" + str(D))
    
X = [x*1000 for x in range(1,31)]

plt.xlabel("Number of stored messages (M)")
plt.ylabel("Error rate, density")

#Neural clique networks (GBNN)
#subplot(100, 64, 12, 9, '^', 100, 0)
#subplot(8, 26, 8, 6, '^', 100, 0)
#subplot(100, 64, 12, 9, '^', 100, 0.23)
#a = 50
succ = 2
prel = 50
subplot(8, 256, 8, 4, 'x', 100, 0)
succ = 3
subplot(8, 256, 8, 4, 's', 100, 0)
succ = 4
subplot(8, 256, 8, 4, 'v', 100, 0)
#subplot(100, 64, 12, 9, 'x', 100, 0)
#subplot(100, 64, 12, 9, 'x', 100, 0.23)
#a =0
#subplot(16, 256, 16, 8, 'x', 100, 0)
#a = 40
#subplot(16, 256, 16, 8, 's', 100, 0)
#a = 80
#subplot(16, 256, 16, 8, 'v', 100, 0)
#a = 10
#subplot(100, 64, 12, 5, '*', 100,0)
#a = 40
#subplot(100, 64, 12, 9, 's', 100,0)
#subplot(8, 256, 8, 4, 's', 100, 0)
#a = 40
#subplot(8, 256, 8, 4, '*', 100, 0)
#a = 80

#a = 80
#subplot(100, 64, 12, 9, 'v', 100,0)
#subplot(8, 256, 8, 4, 'v', 100, 0)

plt.gca().grid(True)
plt.yticks(np.arange(0, 1, 0.1))

plt.show()
