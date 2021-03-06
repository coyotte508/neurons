import matplotlib.pyplot as plt
from subprocess import *
import multiprocessing

X = [x * 10 for x in range(1,29)]

a = 0
def calc_stuff(args):
    n, nbmess = args
    print "mess: ", nbmess
    output = Popen(["../bin/neurons", "-c", "hopfield", str(n), str(nbmess), str(a)]
                    , stdout=PIPE).communicate()[0]
    
    return float(output)
    
def subplot(n, m):
    pool = multiprocessing.Pool(6)
    Y = pool.map(calc_stuff, [(n, nbmess) for nbmess in X])
    
    label = ("size="+ str(n) + ", noise: " + str(a == 1))
    plt.plot(X, Y, "-", marker=m, label=label)
    plt.legend(loc="upper left")
    plt.savefig("hopfield-"+str(a)+".png");

plt.xlabel("Number of learnt messages (M)")
plt.ylabel("Error rate (inputs with 1/2 noise)")

#plot
a = 1
subplot(2048, '^')
a = 0
subplot(2048, 'x')

plt.show()
