import matplotlib.pyplot as plt
from subprocess import *
import multiprocessing
import numpy as np
import scipy
import scipy.misc

plt.rc('text', usetex = True)

"""Sparse coding: n clusters used amongst c clusters for encoding messages"""

a = 0
succ = 2
density = True

X = [x * 6000 for x in range(1, 21)] 

def calc_stuff(args):
    network, counter, tp, nbmess, delta, its = args
    
    if nbmess == X[0]:
        print "iteration ", (counter+1)

    stdout = Popen(["../bin/sparseblur", network, tp, str(nbmess),str(delta), str(its)], stdout=PIPE).communicate()[0]
    
    return float(stdout)
        
def ssplot(delta, marker="x", letter="e", latlet="\epsilon", its=1, network = "cb"):
    pool = multiprocessing.Pool(4)
    #pool.map(calc_stuff, [(nbmess) for nbmess in X])
    Ys = [pool.map(calc_stuff, [(network, counter, letter,nbmess,delta,its) for nbmess in X]) for counter in range(20)]
                
    print Ys
    #monte carlo
    Y = [sum(x)/float(len(x)) for x in zip(*Ys)]
    print Y
    
    label = "simulation, $" + latlet+": " + str(delta) + "$"

    plt.plot(X, Y, marker=marker, label=label)
    
    plt.legend(loc="upper left")
    #plt.savefig("sparseblur.png");
    f = open("sparseblur-" +network + "-" + letter + "-" + str(delta) + "-" + str(its) + "its.txt", "w");
    f.write(str(X) + "\n" + str(Y))
    
def subplot(delta, marker="x", its=1, network="cb"):
    ssplot(delta, marker, "b", "\delta", its, network=network)    

plt.xlabel("Number of learnt messages (M)")
plt.ylabel("Error rate")



def f(x, delta=1):
    l = 100
    chi = 64
    c = 12
    d = 1-(1 - c*(c-1)*1.0/(chi*(chi-1)*l*l) )**x
    alpha = 1- (1 - d)**(2*delta+1)
    return 1 - (1-alpha**c)**((chi-c)*l)*(1-alpha**(c-1))**(2*delta*c)
    
subplot(1,its=10, network = "ws")
subplot(1,its=10, network = "cs")
#plt.plot(X, [0.0, 0.0, 0.0, 0.000125, 0.0010625000000000003, 0.0038750000000000013, 0.018375, 0.061187500000000006, 0.161375, 0.35531250000000014, 0.6110625, 0.8469374999999999, 0.9710625, 0.997875, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0], marker="x", label="simulation, $\delta: 1$")
#plt.plot(X,[f(x,1) for x in X], "--", label="theory, $\delta: 1$")
subplot(3, marker="^", its=10, network="ws")
subplot(3, marker="^", its=10, network="cs")
#plt.plot(X,[0.0, 0.0011250000000000001, 0.0296875, 0.29318750000000005, 0.8606249999999998, 0.999625, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0], marker="v", label="simulation, $\delta: 3$")
#plt.plot(X,[f(x,3) for x in X], "--", label="theory, $\delta: 3$")

def erProba(x, ce=3):
    l = 100
    chi = 64
    c = 12
    ci = c - ce
    d = 1-(1 - c*(c-1)*1.0/(chi*(chi-1)*l*l) )**x
    return 1 - (1-d**ci)**((chi-ci)*l-ce)
    
def ersubplot(delta, marker="x", its=1):
    ssplot(delta, marker, "e", "c_e", its)

X = [x * 10000 for x in range(1, 21)]
#ersubplot(3,its=10)
#plt.plot(X, [0.0, 0.0, 0.0, 6.25e-05, 0.0003125, 0.001125, 0.004437500000000002, 0.010875000000000006, 0.028375000000000004, 0.06518749999999998, 0.12468749999999999, 0.2180625, 0.3494375, 0.5151874999999999, 0.6984374999999999, 0.8418750000000002, 0.9358125000000002, 0.9811249999999999, 0.9959999999999999, 0.9997499999999999], marker="x", label="simulation, $c_e: 3$")
#plt.plot(X,[erProba(x,3) for x in X], "--", label="theory, $c_e: 3$")
#ersubplot(6, marker="v", its=10)
#plt.plot(X, [0.0, 0.0003125, 0.004187500000000002, 0.0230625, 0.07474999999999998, 0.18412499999999996, 0.366875, 0.6040000000000001, 0.8132499999999998, 0.9442500000000003, 0.9861249999999998, 0.9988750000000002, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0], marker="v", label="simulation, $c_e: 6$")
#plt.plot(X,[erProba(x,6) for x in X], "--", label="theory, $c_e: 6$")


def intrProba(x, i=6):
    l = 100
    chi = 64
    c = 12
    d = 1-(1 - c*(c-1)*1.0/(chi*(chi-1)*l*l) )**x
    P1 = (1 - sum([scipy.misc.comb(c+i-1,k)*d**k*(1-d)**(c+i-1-k)for k in range(c-1, c+i)]))**i
    P2 = (1 - sum([scipy.misc.comb(c+i-1,k)*d**k*(1-d)**(c+i-1-k)for k in range(c, c+i)]))**((c+i)*(l-1))
    P3 = (1 - sum([scipy.misc.comb(c+i,k)*d**k*(1-d)**(c+i-k)for k in range(c, c+i+1)]))**((chi-c)*l)
    return 1 - P1*P2*P3

def intrsubplot(delta, marker="x", its=1):
    ssplot(delta, marker, "i", "\iota", its)
    
#intrsubplot(6,its=10)
#plt.plot(X,[intrProba(x,6) for x in X], "--", label="theory, $\iota: 6$")
#intrsubplot(12,marker="v", its=10)
#plt.plot(X,[intrProba(x,12) for x in X], "--", label="theory, $\iota: 12$")

def errsubplot(delta, marker="x", its=1):
    ssplot(delta, marker, "r", "\epsilon", its)
    
def errProba(x, e=3):
    l = 100
    chi = 64
    c = 12
    d = 1-(1 - c*(c-1)*1.0/(chi*(chi-1)*l*l) )**x
    P1 = (1 - sum([scipy.misc.comb(c-1,k)*d**k*(1-d)**(c-1-k)for k in range(c-e-1, c)]))**e
    P2 = (1 - sum([scipy.misc.comb(c-1,k)*d**k*(1-d)**(c-1-k)for k in range(c-e, c)]))**(c*(l-1)-e)
    P3 = (1 - sum([scipy.misc.comb(c,k)*d**k*(1-d)**(c-k)for k in range(c-e, c+1)]))**((chi-c)*l)
    return 1 - P1*P2*P3

#errsubplot(3)
#plt.plot(X,[errProba(x,0) for x in X], "--", label="theory, $\epsilon: 3$")
#errsubplot(6)
#plt.plot(X,[errProba(x,6) for x in X], "--", label="theory, $\epsilon: 6$")

plt.gca().grid(True)
plt.yticks(np.arange(0, 1.1, 0.1))
plt.legend(loc="upper left")

plt.show()
