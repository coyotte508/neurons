import matplotlib.pyplot as plt
from subprocess import *

X = [x/50. for x in range(1,20)]

def subplot(c, l, m):
    Y = []
    for density in X:
        print "c=", c, ", l=", l, ", density: ", density
        Y.append(float(Popen(["../bin/neurons", "-c", "simul2", str(c), str(l), 
                              str(density)], 
                      stdout=PIPE).communicate()[0]))
        print(Y)
    
    label = ("c="+str(c) + ", l=" + str(l))
    plt.semilogy(X, Y, "--", marker=m, label=label)

subplot(4, 512, 'o')
subplot(6, 512, 'x')
subplot(8, 512, '^')
subplot(4, 256, 'v')
subplot(6, 256, 's')
subplot(8, 256, '*')

#plot
plt.xlabel("Network density (d)")
plt.ylabel("Ratio of accepted unlearnt messages over learnt ones")
plt.legend(loc="upper left")

plt.show()