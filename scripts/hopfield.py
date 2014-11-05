import matplotlib.pyplot as plt
from subprocess import *
import multiprocessing

X = [x * 25 for x in range(1,30)]

a = 0
def calc_stuff(args):
    n, nbmess = args
    print "mess: ", nbmess
    output = Popen(["../bin/neurons", "-c", "hopfield", str(n), str(nbmess), str(a)]
                    , stdout=PIPE).communicate()[0]
    
    return float(output)
    
def subplot(n, m):
    pool = multiprocessing.Pool(8)
    Y = pool.map(calc_stuff, [(n, nbmess) for nbmess in X])
    
    label = ("size="+ str(n) + ", noise: " + str(a == 1))
    plt.plot(X, Y, "-", marker=m, label=label)
    plt.legend(loc="upper left")
    plt.savefig("hopfield-"+str(a)+".png");

plt.xlabel("Number of learnt messages (M)")
plt.ylabel("Error rate (inputs with 1/4 noise)")

#plot
subplot(6400, '^')
a = 1
subplot(6400, 'x')

plt.show()
