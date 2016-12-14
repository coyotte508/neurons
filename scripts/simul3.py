import matplotlib.pyplot as plt
import numpy as np
from subprocess import *

a = 0
sy = []
sd = []
def subplot(c, l, k, m,i):
    global sy, sd
    
    Y = []
    D=[]
    if 1:
		for nbmess in X:
			print "c=", c, ", l=", l, ", k=", k, ", mess: ", nbmess
			
			sumY = 0
			sumD = 0
			for idzedzed in range(0, 10):
				output = Popen(["../bin/neurons", "-c", "simul3", str(c), str(l), 
									  str(k), str(nbmess), str(i), str(a)], stdout=PIPE).communicate()[0]
				output = output.split(' ');
				sumY += float(output[0])
				sumD += float(output[1])
			Y.append(sumY/10)
			D.append(sumD/10)
    else:
        Y = []
        D = []
    sy = Y
    sd = D
    
    label = ("erased nodes: " + str(k))
    if a == 0:
        plt.plot(X, D, "--", label="density")
    plt.plot(X, Y, "-", marker=m, label=label)
    
    plt.savefig("simul3-"+str(k)+".png");

#subplot(4, 512, 3, 'x', 1)
X = [x*1000 for x in range(1,30)]
#subplot(8, 256, 4, '^', 5)
#a = 1
subplot(8, 256, 4, 'x', 5)

#plot
plt.xlabel("Number of stored messages (M)")
plt.ylabel("Error rate, density")
plt.legend(loc="upper left")


with open("simul3-" + str(256) + ".txt", "w") as f: 
    f.write(str(X) + "\n" + str(sy) + "\n" + str(sd))

plt.gca().grid(True)
plt.yticks(np.arange(0, 1, 0.1))
    
plt.show()

