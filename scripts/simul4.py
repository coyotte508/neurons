import matplotlib.pyplot as plt
from subprocess import *
import multiprocessing


"""Sparse coding: n clusters used amongst c clusters for encoding messages"""

a = 0

def calc_stuff(args):
    nbmess, c, l, n, k, m, i, tries, r = args
    print ("c=", c, ", l=", l, ", n=", n, ", k=", k, ", mess: ", nbmess, ", a: ", 
                a, ",r: ", r)
    
    stdout = Popen(["../bin/neurons", "-c", "simul4", str(c), str(l), str(n),
                          str(k), str(nbmess), str(i), str(a), str(tries),
                          str(r)], stdout=PIPE).communicate()[0]
    
    output = stdout.split(' ');
    
    print output
    
    y = float(output[0])
    d = float(output[1])
    it = float(output[2])/100
    
    print y,d,it
    return y, d, it
        
def subplot(c, l, n, k, m, i, r):
    pool = multiprocessing.Pool(8)
    res = [zip(*pool.map(calc_stuff, [(nbmess, c, l, n, k, m, i, 30, r) for nbmess in X]))
                    for counter in range(500)]
                
    #monte carlo
    Ys = [x[0] for x in res]
    Ds = [x[1] for x in res]
    Is = [x[2] for x in res]
    Y = [sum(x)/float(len(x)) for x in zip(*Ys)]
    D = [sum(x)/float(len(x)) for x in zip(*Ds)]
    I = [sum(x)/float(len(x)) for x in zip(*Is)]
    
    label = ("X="+str(c) + ", l=" + str(l) + ", c=" + str(n) + ", kc=" + str(k))
    if a:
        label += " (" + str(a) + "%)"
    if r:
        label += " (r: " + str(r) + ")"
    if 0 and a == 0 and c == 100:
        plt.plot(X, D, "--", label="density")        

    plt.plot(X, Y, "-", marker=m, label=label)
    if 0:
        plt.plot(X, I, "--", marker=m, label=label + " (it)")
    
    plt.legend(loc="upper left")
    plt.savefig("simul4-x-rel-"+str(a)+".png");
    f = open("simul4-x-rel-" + str(a) + ".txt", "w");
    f.write(str(X) + "\n" + str(Y))
    
X = [x*1000 for x in range(1,31)]

plt.xlabel("Number of learnt messages (M)")
plt.ylabel("Error rate, density")

#Neural clique networks (GBNN)
#subplot(100, 64, 12, 9, '^', 100, 0)
#subplot(100, 64, 12, 9, '^', 100, 0.23)
a = 50
#subplot(100, 64, 12, 9, 'x', 100, 0)
#subplot(100, 64, 12, 9, 'x', 100, 0.23)
a = 0
subplot(8, 256, 8, 4, 'x', 1, 0)
a = 10
#subplot(100, 64, 12, 5, '*', 100)
a = 40
#subplot(100, 64, 12, 5, 's', 100)
a = 80
#subplot(100, 64, 12, 5, 'v', 100)

plt.show()
