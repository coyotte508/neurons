import matplotlib.pyplot as plt
from subprocess import *

def subplot(c, l, n, m, i):
    Y = []
    D=[]
    stdout = Popen(["../bin/neurons", "-c", "stabletest", str(c), str(l), str(n),
                              str(X[0]), str(i), str(len(X))], stdout=PIPE).stdout
    for nbmess in X:
        print "c=", c, ", l=", l, ", n=", n, ", mess: ", nbmess
        
        output = stdout.readline().split(' ');
        
        print output
        
        Y.append(float(output[0]))
        D.append(float(output[1]))
    
    label = ("c="+str(c) + ", l=" + str(l) + ", n=" + str(n))
    plt.plot(X, D, "--", label="density")
    plt.plot(X, Y, "-", marker=m, label=label)

X = [x*6000 for x in range(1,30)]
#Neural clique networks (GBNN)
subplot(100, 64, 12, '^', 5)

#plot
plt.xlabel("Number of learnt messages (M)")
plt.ylabel("Error rate, density")
plt.legend(loc="upper left")

plt.show()
