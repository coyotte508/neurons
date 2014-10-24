import matplotlib.pyplot as plt
from subprocess import *

"""Sparse coding: n clusters used amongst c clusters for encoding messages"""

a = 0
def subplot(c, l, n, k, m, i):
    Y = []
    D=[]
    stdout = Popen(["../bin/neurons", "-c", "simul4", str(c), str(l), str(n),
                              str(k), str(X[0]), str(i), str(a), str(len(X))], stdout=PIPE).stdout
    for nbmess in X:
        print "c=", c, ", l=", l, ", n=", n, ", k=", k, ", mess: ", nbmess
        
        output = stdout.readline().split(' ');
        
        print output
        
        Y.append(float(output[0]))
        D.append(float(output[1]))
    
    label = ("c="+str(c) + ", l=" + str(l) + ", n=" + str(n) + ", kc=" + str(k))
    if a:
        label += " (noisy)"
    if a == 0 and c == 100:
        plt.plot(X, D, "--", label="density")
    plt.plot(X, Y, "-", marker=m, label=label)

X = [x*6000 for x in range(1,30)]
#Neural clique networks (GBNN)
subplot(100, 64, 12, 9, '^', 5)
a = 1
subplot(100, 64, 12, 9, 'x', 5)

a = 0

#Willshaw networks
subplot(6400, 1, 12, 9, '*', 5)
a = 1
subplot(6400, 1, 12, 9, 's', 5)

#plot
plt.xlabel("Number of learnt messages (M)")
plt.ylabel("Error rate, density")
plt.legend(loc="upper left")

plt.show()
