import matplotlib.pyplot as plt
from subprocess import *

"""Sparse coding: n clusters used amongst c clusters for encoding messages"""

X = [x*1000 for x in range(1,100)]

a = 0
def subplot(c, l, n, k, m,i):
    Y = []
    D=[]
    for nbmess in X:
        print "c=", c, ", l=", l, ", n=", n, ", k=", k, ", mess: ", nbmess
        output = Popen(["../bin/neurons", "-c", "simul4", str(c), str(l), str(n),
                              str(k), str(nbmess), str(i), str(a)], stdout=PIPE).communicate()[0]
        output = output.split(' ');
        
        Y.append(float(output[0]))
        D.append(float(output[1]))
    
    label = ("c="+str(c) + ", l=" + str(l) + ", n=" + str(n) + ", kc=" + str(k))
    if a == 0:
        plt.plot(X, D, "--", label="density")
    plt.plot(X, Y, "-", marker=m, label=label)

#subplot(4, 512, 3, 'x', 1)
X = [x*6000 for x in range(1,30)]
subplot(100, 64, 12, 9, '^', 5)
a = 1
subplot(100, 64, 12, 9, 'x', 5)

#plot
plt.xlabel("Number of learnt messages (M)")
plt.ylabel("Error rate, density")
plt.legend(loc="upper left")

plt.show()
