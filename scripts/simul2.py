import matplotlib.pyplot as plt
from subprocess import *

X = [x/50. for x in range(1,20)]

#1
Y = []
for density in X:
    print "c=4, l=512, density: ", density
    Y.append(float(Popen(["../bin/neurons", "-c", "simul2", "4", "512", str(density)], 
                  stdout=PIPE).communicate()[0]))

plt.semilogy(X, Y, "--", marker='o', label="c=4, l=512")

#2
Y = []
for density in X:
    print "c=6, l=512, density: ", density
    Y.append(float(Popen(["../bin/neurons", "-c", "simul2", "6", "512", str(density)], 
                      stdout=PIPE).communicate()[0]))
                      
plt.semilogy(X, Y, "--", marker='x', label="c=6, l=512")
  
#3                
Y = []
for density in X:
    print "c=8, l=512, density: ", density
    Y.append(float(Popen(["../bin/neurons", "-c", "simul2", "8", "512", str(density)], 
                      stdout=PIPE).communicate()[0]))

plt.semilogy(X, Y, "--", marker='^', label="c=8, l=512")
                      
#1
Y = []
for density in X:
    print "c=4, l=256, density: ", density
    Y.append(float(Popen(["../bin/neurons", "-c", "simul2", "4", "256", str(density)], 
                  stdout=PIPE).communicate()[0]))

plt.semilogy(X, Y, "--", marker='v', label="c=4, l=256")

#2
Y = []
for density in X:
    print "c=6, l=512, density: ", density
    Y.append(float(Popen(["../bin/neurons", "-c", "simul2", "6", "512", str(density)], 
                      stdout=PIPE).communicate()[0]))
                      
plt.semilogy(X, Y, "--", marker='s', label="c=6, l=256")
  
#3                
Y = []
for density in X:
    print "c=8, l=512, density: ", density
    Y.append(float(Popen(["../bin/neurons", "-c", "simul2", "8", "512", str(density)], 
                      stdout=PIPE).communicate()[0]))
                      
plt.semilogy(X, Y, "--", marker='*', label="c=8, l=256")

#plot
plt.xlabel("Network density (d)")
plt.ylabel("Ratio of accepted unlearnt messages over learnt ones")
plt.legend(loc="upper left")

plt.show()