import matplotlib.pyplot as plt
from subprocess import *
import multiprocessing


"""Sparse coding: n clusters used amongst c clusters for encoding messages"""

a = 0

def calc_stuff(args):
    nbmess, c, l, n, k, m, i = args
    print "c=", c, ", l=", l, ", n=", n, ", k=", k, ", mess: ", nbmess, ", a: ", a
    
    stdout = Popen(["../bin/neurons", "-c", "simul4", str(c), str(l), str(n),
                          str(k), str(nbmess), str(i), str(a)], stdout=PIPE).stdout
    
    output = stdout.readline().split(' ');
    
    print output
    
    y = float(output[0])
    d = float(output[1])
    it = float(output[2])/100
    
    print y,d,it
    return y, d, it
        
def subplot(c, l, n, k, m, i):
    Y = []
    D=[]
    I=[]
        
    pool = multiprocessing.Pool(4)
    Y, D, I = zip(*pool.map(calc_stuff, [(nbmess, c, l, n, k, m, i) for nbmess in X]))
    
    label = ("X="+str(c) + ", l=" + str(l) + ", c=" + str(n) + ", kc=" + str(k))
    if a:
        label += " (noisy)"
    if a == 0 and c == 100:
        plt.plot(X, D, "--", label="density")        

    plt.plot(X, Y, "-", marker=m, label=label)
    plt.plot(X, I, "--", marker=m, label=label + " (it)")
    
    plt.legend(loc="upper left")
    plt.savefig("simul4-"+str(a)+".png");

X = [x*6000 for x in range(1,30)]

plt.xlabel("Number of learnt messages (M)")
plt.ylabel("Error rate, density")

#Neural clique networks (GBNN)
subplot(100, 64, 12, 9, '^', 100)
a = 1
subplot(100, 64, 12, 9, 'x', 100)
a = 10
subplot(100, 64, 12, 9, '*', 100)
a = 40
subplot(100, 64, 12, 9, 's', 100)
a = 80
subplot(100, 64, 12, 9, 'v', 100)

plt.show()
