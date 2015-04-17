import matplotlib.pyplot as plt
from subprocess import *
import multiprocessing
import numpy as np
import scipy
import scipy.misc
import math

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



def blurProba(x, delta=1):
    l = 100
    chi = 64
    c = 12
    d = 1-(1 - c*(c-1)*1.0/(chi*(chi-1)*l*l) )**x
    return 1 - (1-(1-(1-d)**(2*delta+1))**(c-1))**(2*delta*c)

X = [x * 6000 for x in range(1, 21)] 

#subplot(1,its=1)
#subplot(3,its=10)
#subplot(1,its=10, network = "cs")
#plt.plot(X, [0.0, 0.0, 0.0, 0.000125, 0.0010625000000000003, 0.0038750000000000013, 0.018375, 0.061187500000000006, 0.161375, 0.35531250000000014, 0.6110625, 0.8469374999999999, 0.9710625, 0.997875, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0], marker="x", label="simulation, $\delta: 1$")
#plt.plot(X,[f(x,1) for x in X], "--", label="theory, $\delta: 1$")
#subplot(3, marker="^", its=10, network="ws")
#subplot(3, marker="^", its=10, network="cs")
#plt.plot(X,[0.0, 0.0011250000000000001, 0.0296875, 0.29318750000000005, 0.8606249999999998, 0.999625, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0], marker="v", label="simulation, $\delta: 3$")
#plt.plot(X,[f(x,3) for x in X], "--", label="theory, $\delta: 3$")

def erProba(x, ce=3):
    l = 100
    chi = 64
    c = 12
    ci = c - ce
    d = 1-(1 - c*(c-1)*1.0/(chi*(chi-1)*l*l) )**x
    return 1 - (1-d**ci)**((chi-ci)*l-ce)
    
def ersubplot(delta, marker="x", its=1, network="cb"):
    ssplot(delta, marker, "e", "c_e", its, network=network)

X = [x * 10000 for x in range(1, 21)]
#X = [x * 1500 for x in range(1, 21)] 
#ersubplot(3,its=10, network="wb")
#ersubplot(6,its=10, network="wb")
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
    #P1 = (1 - sum([scipy.misc.comb(c+i-1,k)*d**k*(1-d)**(c+i-1-k)for k in range(c-1, c+i)]))**i
    #P2 = (1 - sum([scipy.misc.comb(c+i-1,k)*d**k*(1-d)**(c+i-1-k)for k in range(c, c+i)]))**((c+i)*(l-1))
    #P3 = (1 - sum([scipy.misc.comb(c+i,k)*d**k*(1-d)**(c+i-k)for k in range(c, c+i+1)]))**((chi-c)*l)
    P = (1 - sum([scipy.misc.comb(c+i-1,k)*d**k*(1-d)**(c+i-1-k) for k in range (c-1, c+i)]))**i
    #return 1 - P1*P2*P3
    return 1-P

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

def efficiency(x, errRate):
    l = 100
    chi = 64
    c = 12
    n = math.log(scipy.misc.comb(chi, c),2) + c*math.log(l, 2)
    H = x * (n - math.log(x, 2))
    HW = chi*(chi-1)/2*l*l
    d = 1-(1 - c*(c-1)*1.0/(chi*(chi-1)*l*l) )**x    
    #print H, HW, d, (1-(d)**(c))    
    return (1-errRate) * H / HW #* (1-(d)**(c)) * 1.0 * ((chi)*(chi-1)/2*l*l - x) /((chi)*(chi-1)/2*l*l) 
    
def smallEfficiency(x, errRate):
    l = 256
    chi = 8
    c = 8
    n = math.log(scipy.misc.comb(chi, c),2) + c*math.log(l, 2)
    H = x * (n - math.log(x, 2))
    HW = chi*(chi-1)/2*l*l
    return (1-errRate) * H / HW

XICB = [x * 10000 for x in range(1, 21)]
YI1it6CB = [0.0, 0.0, 0.0, 0.0, 0.0, 0.000125, 0.000125, 0.001375, 0.0033750000000000004, 0.004750000000000002, 0.013625000000000003, 0.026875, 0.037375, 0.057875000000000024, 0.07912499999999999, 0.11437500000000003, 0.1365, 0.18162499999999998, 0.22712499999999997, 0.271125]
YI10it6CB = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.00025, 0.00075, 0.002875, 0.0033750000000000004, 0.006625000000000001, 0.008750000000000003, 0.013875000000000004, 0.022250000000000006, 0.032750000000000015, 0.04062500000000001, 0.05575, 0.076125]
YI1it12CB = [0.0, 0.0, 0.0, 0.000625, 0.0022500000000000003, 0.009250000000000003, 0.025, 0.05012500000000001, 0.08962499999999998, 0.14600000000000005, 0.205375, 0.27787500000000004, 0.3655, 0.444125, 0.5295, 0.592125, 0.6622500000000001, 0.7205, 0.7671249999999998, 0.8047500000000001]
YI10it12CB =[0.0, 0.0, 0.0, 0.0, 0.000125, 0.001125, 0.003, 0.008000000000000002, 0.016375000000000004, 0.037000000000000005, 0.06087499999999999, 0.09325000000000001, 0.12912500000000002, 0.17425, 0.218625, 0.272, 0.3075, 0.36500000000000005, 0.417375, 0.45512499999999995]
XRCB = [x * 6000 for x in range(1, 21)]
YR1it3CB = [0.0, 0.0, 0.0, 0.000375, 0.0008749999999999999, 0.002500000000000001, 0.012312500000000007, 0.030312499999999992, 0.07493750000000002, 0.14724999999999994, 0.281375, 0.46506250000000005, 0.6588125, 0.8331875, 0.9414999999999999, 0.9851249999999998, 0.9978124999999999, 0.9991249999999997, 0.9989375, 0.9993125]
YR10it3CB = [0.0, 0.0, 0.0, 0.0, 0.0, 0.000375, 0.0013749999999999997, 0.0032500000000000003, 0.010000000000000004, 0.037625000000000006, 0.11074999999999997, 0.30050000000000004, 0.62225, 0.85075, 0.9549999999999998, 0.9935, 0.999125, 0.99975, 1.0, 1.0]
YR1it6CB = [0.00043750000000000006, 0.023374999999999996, 0.1755625, 0.5583750000000001, 0.9155000000000001, 0.9974375, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0]
YR10it6CB = [0.0, 0.0, 0.0, 0.0, 0.0046250000000000015, 0.07775000000000001, 0.3425, 0.74225, 0.9127500000000002, 0.9797499999999999, 0.99725, 0.9995, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0]
XECB = [x * 10000 for x in range(1, 21)]
YE1it3CB = [0.0, 0.0, 0.0, 6.25e-05, 0.0003125, 0.001125, 0.004437500000000002, 0.010875000000000006, 0.028375000000000004, 0.06518749999999998, 0.12468749999999999, 0.2180625, 0.3494375, 0.5151874999999999, 0.6984374999999999, 0.8418750000000002, 0.9358125000000002, 0.9811249999999999, 0.9959999999999999, 0.9997499999999999]
YE10it3CB = [0.0, 0.0, 0.0, 0.0, 0.0, 0.00025, 0.000125, 0.002125, 0.00475, 0.014750000000000004, 0.0335, 0.0685, 0.1455, 0.2668750000000001, 0.455375, 0.6567500000000001, 0.828125, 0.9435, 0.9875, 0.9963749999999999]
YE1it6CB = [0.0, 0.0003125, 0.004187500000000002, 0.0230625, 0.07474999999999998, 0.18412499999999996, 0.366875, 0.6040000000000001, 0.8132499999999998, 0.9442500000000003, 0.9861249999999998, 0.9988750000000002, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0]
YE10it6CB = [0.0, 0.0, 0.0, 0.0, 0.00025, 0.000625, 0.0012499999999999998, 0.0033750000000000004, 0.013375000000000003, 0.030625000000000006, 0.09462500000000001, 0.25749999999999995, 0.589625, 0.9015000000000001, 0.99025, 0.998625, 1.0, 1.0, 1.0, 1.0]
XBCB = [x * 10000 for x in range(1, 21)]
YB1it1CB = [0.0, 0.0, 0.0, 0.000375, 0.001, 0.0032499999999999994, 0.012750000000000003, 0.030375000000000003, 0.07137500000000001, 0.13899999999999998, 0.225875, 0.343375, 0.481625, 0.624875, 0.7426250000000001, 0.8446250000000003, 0.9113749999999999, 0.9563750000000001, 0.978375, 0.9902499999999999]
YB10it1CB = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.000125, 0.00025, 0.0, 0.000625, 0.0019999999999999996, 0.002625, 0.0032500000000000003, 0.004250000000000001, 0.009375000000000003]
YB1it3CB = [0.0, 0.0024999999999999996, 0.048, 0.27925, 0.6983750000000002, 0.9526249999999999, 0.997875, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0]
YB10it3CB = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.000125, 0.000125, 0.000625, 0.001125, 0.001875, 0.005875, 0.021875, 0.107125, 0.35525000000000007, 0.674875]

XBCS = [x * 1500 for x in range(1, 21)]
YB1CS = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.000125, 0.0, 0.000375, 0.001125, 0.0008750000000000001, 0.0020000000000000005, 0.0022499999999999994, 0.002875, 0.0045, 0.0065000000000000014, 0.010625000000000004, 0.011500000000000005, 0.017000000000000005]
YB3CS = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0005, 0.00025, 0.000625, 0.0023749999999999995, 0.0022500000000000003, 0.002999999999999999, 0.005, 0.011125000000000005, 0.015750000000000007, 0.023750000000000004, 0.03375, 0.06075000000000001, 0.129625]
YB1WS = [0.0, 0.0, 0.0, 0.0, 0.007500000000000004, 0.027500000000000007, 0.0625, 0.09000000000000002, 0.1375, 0.24000000000000013, 0.30499999999999994, 0.3475000000000001, 0.3899999999999999, 0.45499999999999996, 0.5050000000000001, 0.5549999999999999, 0.5824999999999998, 0.625, 0.67, 0.7449999999999997]
YB3WS = [0.0, 0.06000000000000003, 0.32749999999999996, 0.55, 0.7025000000000001, 0.8550000000000002, 0.9324999999999998, 0.9299999999999999, 0.9549999999999997, 0.9600000000000005, 0.975, 0.9949999999999999, 0.9924999999999999, 0.9899999999999999, 0.9975000000000002, 0.9949999999999999, 0.9975000000000002, 0.9975000000000002, 1.0, 0.9949999999999999]
XECS = [x * 1500 for x in range(1, 21)]
YE4CS = [0.0, 0.0, 0.0, 0.0014999999999999998, 0.0, 0.0, 0.0, 0.005250000000000002, 0.015375000000000005, 0.026125000000000002, 0.034625, 0.067875, 0.108125, 0.156625, 0.2507500000000001, 0.35462499999999997, 0.5367499999999998, 0.689625, 0.8295, 0.9148750000000001]
YE4WS = [0.0, 0.0, 0.0, 0.0, 0.0027500000000000003, 0.000625, 0.010750000000000001, 0.015375000000000003, 0.04162500000000001, 0.09287500000000001, 0.16099999999999998, 0.27125000000000005, 0.43725, 0.61425, 0.7746249999999999, 0.8743749999999999, 0.9424999999999999, 0.9810000000000002, 0.993, 0.9958750000000001]
XEWB = [x * 10000 for x in range(1, 21)]
YE3WB  = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0005, 0.0017500000000000003, 0.0031250000000000006, 0.0065000000000000014, 0.013250000000000007, 0.02350000000000001, 0.05362500000000002, 0.134, 0.21912499999999996, 0.3509999999999999, 0.521625, 0.678375, 0.8275, 0.9103750000000002, 0.968375]
YE6WB = [0.0, 0.0, 0.0, 0.0, 0.0, 0.00075, 0.0023750000000000004, 0.006624999999999999, 0.015000000000000005, 0.04837500000000001, 0.15724999999999997, 0.388875, 0.6827499999999999, 0.8790000000000001, 0.9697500000000001, 0.9945000000000002, 0.99875, 1.0, 1.0, 1.0]
XBWB = [x * 10000 for x in range(1, 21)]
YB1WB = [0.0, 0.0, 0.0, 0.0, 0.0025000000000000005, 0.007500000000000004, 0.012500000000000002, 0.030000000000000016, 0.06499999999999997, 0.08499999999999998, 0.08499999999999998, 0.165, 0.21000000000000002, 0.29999999999999993, 0.33750000000000013, 0.3899999999999999, 0.46499999999999997, 0.55, 0.545, 0.6349999999999999]
YB3WB = [0.0, 0.0025000000000000005, 0.1125, 0.30750000000000005, 0.5125, 0.7200000000000002, 0.8299999999999998, 0.885, 0.9375, 0.9574999999999998, 0.975, 0.9774999999999995, 0.9899999999999999, 0.9924999999999999, 1.0, 0.9975000000000002, 0.9924999999999999, 1.0, 1.0, 1.0]

#insertions
#plt.plot(XICB, YI1it6CB, label="1 iteration, $\iota=6$", marker="x")
#plt.plot(XICB, [intrProba(x,6) for x in XICB], "--", label="1 iteration, $\iota=6$, theory")
#plt.plot(XICB, YI10it6CB, label="10 iterations, $\iota=6$", marker="v")
#plt.plot(XICB, YI1it12CB, label="1 iteration, $\iota=12$", marker="s")
#plt.plot(XICB, [intrProba(x,12) for x in XICB], "--", label="1 iteration, $\iota=12$, theory")
#plt.plot(XICB, YI10it12CB, label="10 iterations, $\iota=12$", marker="*")

#errors
#plt.plot(XRCB, YR1it3CB, label="1 iteration, $\epsilon=3$", marker="x")
#plt.plot(XRCB, [errProba(x,3) for x in XRCB], "--", label="1 iteration, $\epsilon=3$, theory")
#plt.plot(XECB, YR10it3CB, label="10 iterations, $\epsilon=3$", marker="v")
#plt.plot(XRCB, YR1it6CB, label="1 iteration, $\epsilon=6$", marker="s")
#plt.plot(XRCB, [errProba(x,6) for x in XRCB], "--", label="1 iteration, $\epsilon=6$, theory")
#plt.plot(XECB, YR10it6CB, label="10 iterations, $\epsilon=6$", marker="*")

#erasures
#plt.plot(XECB, YE1it3CB, label="1 iteration, $c_e=3$", marker="x")
#plt.plot(XECB, [erProba(x,3) for x in XECB], "--", label="1 iteration, $c_e=3$, theory")
#plt.plot(XECB, YE10it3CB, label="10 iterations, $c_e=3$", marker="v")
#plt.plot(XECB, YE1it6CB, label="1 iteration, $c_e=6$", marker="s")
#plt.plot(XECB, [erProba(x,6) for x in XECB], "--", label="1 iteration, $c_e=6$, theory")
#plt.plot(XECB, YE10it6CB, label="10 iterations, $c_e=6$", marker="*")

#blur
#plt.plot(XBCB, YB1it1CB, label="1 iteration, $\delta=1$", marker="x")
#plt.plot(XBCB, [blurProba(x,1) for x in XBCB], "--", label="1 iteration, $\delta=1$, theory")
#plt.plot(XBCB, YB10it1CB, label="10 iterations, $\delta=1$", marker="v")
#plt.plot(XBCB, YB1it3CB, label="1 iteration, $\delta=3$", marker="s")
#plt.plot(XBCB, [blurProba(x,3) for x in XBCB], "--", label="1 iteration, $\delta=3$, theory")
#plt.plot(XBCB, YB10it3CB, label="10 iterations, $\delta=3$", marker="*")

#blur on small networks
#plt.plot(XBCS, YB1CS, label="NBWN, $\delta=1$", marker="x");
#plt.plot([0]+XECS, [0]+[smallEfficiency(x,y) for (x,y) in zip(XBCS, YB1CS)], "--", label="NBWN, efficiency, $\delta=1$")
#plt.plot(XBCS, YB3CS, label="NBWN, $\delta=3$", marker="v");
#plt.plot([0]+XECS, [0]+[smallEfficiency(x,y) for (x,y) in zip(XBCS, YB3CS)], "--", label="NBWN, efficiency, $\delta=3$")
#plt.plot(XBCS, YB1WS, label="WN, $\delta=1$", marker="s");
#plt.plot(XBCS, YB3WS, label="WN, $\delta=3$", marker="*");

#erasure on small networks
#plt.plot(XECS, YE4CS, label="NBWN, $c_e=4$", marker="x")
#plt.plot([0]+XECS, [0]+[smallEfficiency(x,y) for (x,y) in zip(XECS, YE4CS)], "--", label="NBWN, efficiency, $c_e=4$")
#plt.plot(XECS, YE4WS, label="WN, $c_e=4$", marker="v")

#erasure on big networks
#plt.plot(XEWB, YE10it3CB, label="NBWN, $c_e=3$", marker="x")
#plt.plot([0]+XEWB, [0]+[efficiency(x,y) for (x,y) in zip(XEWB, YE10it3CB)], "--", label="NBWN, efficiency, $c_e=3$")
#plt.plot(XEWB, YE10it6CB, label="NBWN, $c_e=6$", marker="v")
#plt.plot([0]+XEWB, [0]+[efficiency(x,y) for (x,y) in zip(XEWB, YE10it6CB)], "--", label="NBWN, efficiency, $c_e=6$")
#plt.plot(XEWB, YE3WB, label="WN, $c_e=3$", marker="s")
#plt.plot(XEWB, YE6WB, label="WN, $c_e=6$", marker="*")

#blur on big networks
plt.plot(XBWB, YB10it1CB, label="NBWN, $\delta=1$", marker="x")
##plt.plot([0]+XEWB, [0]+[efficiency(x,y) for (x,y) in zip(XBWB, YB10it1CB)], "--", label="NBWN, efficiency, $\delta=1$")
plt.plot(XBCB, YB10it3CB, label="NBWN, $\delta=3$", marker="v")
plt.plot(XBWB, YB1WB, label="WN, $\delta=1$", marker="s")
plt.plot(XBWB, YB3WB, label="WN, $\delta=3$", marker="*")

plt.gca().grid(True)
plt.yticks(np.arange(0, 1.1, 0.1))
plt.legend(loc="upper left")

plt.show()
