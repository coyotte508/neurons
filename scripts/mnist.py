import matplotlib.pyplot as plt
from subprocess import *
import multiprocessing


"""Sparse coding: n clusters used amongst c clusters for encoding messages"""

X = [x * 50 for x in range(1, 61)]

def calc_stuff(t):
    if t[1] == X[0]:
        print "doing " + t[0] 
    
    stdout = Popen(["../bin/neurons", "-c", "mnist", t[0], str(t[1]), str(t[2])], stdout=PIPE).communicate()[0]
    
    print stdout.strip()
    output = float(stdout.strip());
        
    return output
        
def subplot(t, m, l):
    print "subplotting: " + l
    pool = multiprocessing.Pool(8)
    res = zip(*[pool.map(calc_stuff, [(t, nbmess,  200) for nbmess in X]) for counter in range(40)])

    #monte carlo
    Ys = [sum(x)/float(len(x)) for x in res]
    
    plt.plot(X, Ys, "-", marker=m, label=l)
    
    plt.legend(loc="upper left")
    plt.savefig("mnist-" + t + ".png");
    f = open("mnist-" + t + ".txt", "w");
    f.write(str(X) + "\n" + str(Ys))

subplot("d", "x", "25% erasure")
subplot("e", "v", "25% errors")
subplot("i", "^", "25% insertions")
#subplot("b", "s", "full blur")

plt.xlabel("Number of learnt messages (M)")
plt.ylabel("Error rate")


plt.show()
