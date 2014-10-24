import matplotlib.pyplot as plt
from subprocess import *

X = [x * 10 for x in range(1,30)]

a = 0
def subplot(n, m):
    Y = []
    for nbmess in X:
        print "mess: ", nbmess
        output = Popen(["../bin/neurons", "-c", "hopfield", str(n), str(nbmess), str(a)]
                        , stdout=PIPE).communicate()[0]
        
        Y.append(float(output))
    
    label = ("size="+ str(n) + ", noise: " + str(a == 1))
    plt.plot(X, Y, "-", marker=m, label=label)

subplot(6000, '^')
a = 1
subplot(6000, 'x')

#plot
plt.xlabel("Number of learnt messages (M)")
plt.ylabel("Error rate (inputs with 1/4 noise)")
plt.legend(loc="upper left")

plt.show()