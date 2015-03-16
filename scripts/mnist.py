import matplotlib.pyplot as plt
from subprocess import *
import multiprocessing


"""Sparse coding: n clusters used amongst c clusters for encoding messages"""

X = [x * 50 for x in range(1, 31)]

def calc_stuff(t):
    stdout = Popen(["../bin/neurons", "-c", "mnist", str(t[0]), str(t[1])], stdout=PIPE).communicate()[0]
    
    print stdout.strip()
    output = float(stdout.strip());
        
    return output
        
def subplot():
    pool = multiprocessing.Pool(8)
    res = zip([pool.map(calc_stuff, [(nbmess,  200) for nbmess in X]) for counter in range(40)])

    #monte carlo
    Ys = [sum(x)/float(len(x)) for x in res]
    
    plt.plot(X, Ys, "-", marker=m, label=label)
    
    plt.legend(loc="upper left")
    plt.savefig("mnist.png");
    f = open("mnist.txt", "w");
    f.write(str(X) + "\n" + str(Ys))

plt.xlabel("Number of learnt messages (M)")
plt.ylabel("Error rate")

subplot()

plt.show()
