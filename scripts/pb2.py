import matplotlib.pyplot as plt
from subprocess import *
import multiprocessing
import math
import scipy.misc
import numpy as np

"""Sparse coding: n clusters used amongst c clusters for encoding messages"""
plt.rc('text', usetex = True)

ref = False 
X = [x * 1000 for x in range(1, 31)]
eraseProba = 0.5
successProba = 1- eraseProba
successProba2 = successProba * successProba
n = 10
l = 256
c = 8


   
def calc_stuff(args):
    nbmess, c, l, n = args
    stdout = Popen(["../bin/neurons", "-c", "pb2", str(c), str(l), str(nbmess),
                          str(n), str(eraseProba)], stdout=PIPE).communicate()[0]

    y = float(stdout)
    
    print y
    #return math.log(y)
    return y
        
def subplot(c, l, m, n):
    global density
    pool = multiprocessing.Pool(8)
    
    res = [pool.map(calc_stuff, [(nbmess, c, l, n) for nbmess in X])
                    for counter in range(50)]
           
    print res
    
    #monte carlo
    Y = [sum(x)/float(len(x)) for x in zip(*res)]
    

    plt.plot(X, Y, "-", marker=m, label="Erasure rate: " + str(eraseProba) + ", n: " + str(n) + "$")
    plt.legend(loc="upper left")
    
    plt.savefig("pb2-" + str(eraseProba) + "-simple-" + str(n) + ".png");
    f = open("pb2-" + str(eraseProba) + "-simple-" + str(n) + ".txt", "w");
    f.write(str(X) + "\n"  + str(Y))
    
#if 0:
plt.xlabel("Number of learnt messages (M)")
plt.ylabel("Success rate")

#    eraseProba = 0.1
#subplot(8, 256, 's', 1)
 #   eraseProba = 0.2
#subplot(8, 256, 'x', 2)
#    eraseProba = 0.3
#subplot(8, 256, 'v', 3)
#    eraseProba = 0.4


#subplot(8, 256, '^', n)

D = [1 - (1 - 1./(l*l))**x for x in X]

def proba(x, dmax=15):
    d = 1 - (1 - 1./(l*l))**x
    #print d
    #ctot = d*c*(c-1)*l*l/2
    #nmoy = sum([scipy.misc.comb(c, k) * (1-eraseProba) ** k * eraseProba ** (c-k) * k * (k-1)/2 for k in range(0, c+1)])
    develop = [(d**k/(float(k+1)) - d**(k+1)/float(k+2)) * (1 - (1 -successProba2)**((k+1)*n)) for k in range(0, dmax)]
    nmoy = sum(develop)
    k = dmax #* (1 - (1 -successProba2)**((k+1)*n))
    nmoy += d**k/(float(k+1)) * (1 - (1 -successProba2)**((k+1)*n))
   
    #print develop
    #print nmoy
    #print (sum(develop[0:2]) + d**2/(float(2+1)))
    #print nmoy * 1. / (c*(c-1)/2) 
    #return (1 - nmoy/ctot)**n
    return nmoy;
    #return sum(develop[0:2]) + d**2/(float(2+1))
    #return develop[0] + d/float(2)  + d**2/3
#plt.plot(X, D, "--", label="density")

def proba2(x, dmax = 30) :
    puredevelop = [scipy.misc.comb(x-1, k) * (1./l**2)**k * (1-1./l**2)**(x-1-k) for k in range(0, dmax)]
    develop = [scipy.misc.comb(x-1, k) * (1./l**2)**k * (1-1./l**2)**(x-1-k) * (1 - (1 -successProba2)**((k+1)*n))  for k in range(0, dmax)]
    print puredevelop, develop
    nmoy = sum(develop) + 1 - sum([scipy.misc.comb(x, k) * (1./(l*l))**k * (1-1./(l*l))**(x-1-k)  for k in range(0, dmax)])
    #nmoy = develop[0] + d
    #nmoy = (1-d) * (1 - (1 -successProba2**n)) + d
    return nmoy;
    

def proba3(x, dmax = 30) :
    successProba = 1- eraseProba
    successProba2 = successProba * successProba

    puredevelop = [scipy.misc.comb(x-1, k) * (1./l**2)**k * (1-1./l**2)**(x-1-k) / (k+1) for k in range(0, dmax)]
    develop = [scipy.misc.comb(x-1, k) * (1./l**2)**k * (1-1./l**2)**(x-1-k) / (k+1) * (1 - (1 -successProba2)**((k+1)*n))  for k in range(0, dmax)]
    #print puredevelop, develop
    nmoy = sum(develop) / sum(puredevelop)
    #nmoy = develop[0] + d
    #nmoy = (1-d) * (1 - (1 -successProba2**n)) + d
    return nmoy;
    
    
#plt.plot(X, [proba(x) for x in X], "--", label="theoric (v1)")

#plt.plot(X, [proba2(x) for x in X], "--", label="theoric (picked from the list)")
#plt.plot(X, [proba3(x) for x in X], "--", label="theoric (picked from the set)")
#plt.xlabel("Network sweeps (r)")
#plt.ylabel("Success rate")

#for erProba in np.array(range(1, 6)) * 0.1:
#    plt.plot(range(1, 21), [[1 - (1-(1- erProba)*(1- erProba))**x] for x in range(1, 21)], "--", label="Erasure rate: " + str(erProba))
    
expectedSuccessRate = 0.95
n = 15

def findEraseProba(x):
    global eraseProba
    
    lo = 0.
    hi = 1.
    
    while 1:
        mid = (lo+hi)/2.
        eraseProba = mid
        midval = proba3(x)
        if midval < expectedSuccessRate:
            hi = mid
        elif midval > expectedSuccessRate: 
            lo = mid
        
        #print lo, hi, mid, abs(midval-expectedSuccessRate)
        if abs(midval-expectedSuccessRate) < 0.00001:
            return mid

plt.ylabel("Node erasure rate")
#print findEraseProba(10000)
plt.plot(X, [findEraseProba(x) for x in X], "--", label="Success rate: " + str(expectedSuccessRate) + ", r: " + str(n))
expectedSuccessRate = 0.85
plt.plot(X, [findEraseProba(x) for x in X], "--", label="Success rate: " + str(expectedSuccessRate) + ", r: " + str(n))
n = 14
plt.plot(X, [findEraseProba(x) for x in X], "--", label="Success rate: " + str(expectedSuccessRate) + ", r: " + str(n))
expectedSuccessRate = 0.95
plt.plot(X, [findEraseProba(x) for x in X], "--", label="Success rate: " + str(expectedSuccessRate) + ", r: " + str(n))

#if 0:
plt.legend(loc="center right")
plt.show()

#proba(100)