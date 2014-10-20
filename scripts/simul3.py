import matplotlib.pyplot as plt
from subprocess import *

X = [x*1000 for x in range(1,100)]

def subplot(c, l, k, m,i):
    Y = []
    D=[]
    for nbmess in X:
        print "c=", c, ", l=", l, ", k=", k, ", mess: ", nbmess
        output = Popen(["../bin/neurons", "-c", "simul3", str(c), str(l), 
                              str(k), str(nbmess), str(i)], stdout=PIPE).communicate()[0]
        output = output.split(' ');
        
        Y.append(float(output[0]))
        D.append(float(output[1]))
    
    label = ("c="+str(c) + ", l=" + str(l) + ", kc=" + str(k))
    plt.plot(X, Y, "-", marker=m, label=label)
    plt.plot(X, D, "--", label="density")

#subplot(4, 512, 3, 'x', 1)
X = [x*1000 for x in range(1,30)]
subplot(8, 256, 4, '^', 5)

#plot
plt.xlabel("Number of learnt messages (M)")
plt.ylabel("Error rate, density")
plt.legend(loc="upper left")

plt.show()