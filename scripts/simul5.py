import matplotlib.pyplot as plt
from subprocess import *
import multiprocessing
import numpy as np

"""Sparse coding: n clusters used amongst c clusters for encoding messages"""

ref = False 
X = [x * 1000 for x in range(1, 31)]

def calc_stuff(t):
    stdout = Popen(["../bin/neurons", "-c", "simul5", str(t)], stdout=PIPE).communicate()[0]
    
    output = stdout.splitlines();
    
    for i in range(len(output)):
        output[i] = [float(y) for y in output[i].split(' ')];
    
    return output
        
def subplot(t, m, refdata=None, data=None):
    global ref

    if not data or not refdata:
        pool = multiprocessing.Pool(8)
        res = zip(*pool.map(calc_stuff, [t for x in range(8)]))

    #monte carlo
        Ys = [[x[0] for x in y] for y in res]
        Yrefs = [[x[1] for x in y] for y in res]
        Ys = [sum(x)/float(len(x)) for x in Ys]
        Yrefs = [sum(x)/float(len(x)) for x in Yrefs]
    else:
        Ys = data
        Yrefs = refdata

    label = "Recurrent Hebbian network"

    if not ref:
        ref = True
        # print X
        # print Yrefs
        plt.plot(X, Yrefs, "--", label="Neural clique network")

    plt.plot(X, Ys, "-", marker=m, label=label)
    
    plt.legend(loc="upper left")
    plt.savefig("simul5-"+str(t)+".eps");
    if not data:
        f = open("simul5-" + str(t) + ".txt", "w");
        f.write(str(X) + "\n" + str(Ys) + "\n" + str(Yrefs))

plt.xlabel("Number of learnt messages (M)")
plt.ylabel("Error rate")
plt.xticks(np.arange(0, 35000, 5000))
plt.yticks(np.arange(0, 1.1, 0.1))
plt.ylim([0,1])
plt.gca().grid(True)
fromData = True

if fromData:
    subplot(0, 'x', data=[0.0, 0.0, 0.0, 0.0, 0.0, 0.000125, 0.000125, 0.000375, 0.0005, 0.00175, 0.0040625, 0.00875, 0.0143125, 0.022687499999999996, 0.03900000000000001, 0.056687499999999995, 0.093625, 0.1293125, 0.19481250000000003, 0.26143750000000004, 0.34368750000000003, 0.43756249999999997, 0.529625, 0.614, 0.7028749999999999, 0.768625, 0.8324999999999999, 0.8756875, 0.9134375000000001, 0.933875], refdata=[0.0, 0.0, 0.0, 0.0, 0.0, 0.000125, 6.25e-05, 0.0003125, 0.0008750000000000001, 0.00175, 0.003375, 0.0063750000000000005, 0.0091875, 0.0150625, 0.024874999999999998, 0.0379375, 0.0594375, 0.08206250000000001, 0.12475, 0.1791875, 0.242125, 0.32243750000000004, 0.41075, 0.5017499999999999, 0.6008125000000001, 0.6873750000000001, 0.7643749999999999, 0.8276874999999999, 0.8798124999999999, 0.9145625])
else:
    subplot(0, 'x')
    subplot(2, '^')

plt.show()
