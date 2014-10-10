import matplotlib.pyplot as plt
from subprocess import *

X = [x/20. for x in range(20)]

#1
Y = []
for density in X[1:]:
    print "c=4, l=512, density: ", density
    Y.append(float(Popen(["../bin/neurons", "-c", "simul1", "4", "512", str(density)], 
                  stdout=PIPE).communicate()[0]))

plt.semilogy(X + [1], [0] + Y + [1], "--", marker='o', label="c=4, l=512")

#2
Y = []
for density in X[8:]:
    print "c=6, l=512, density: ", density
    Y.append(float(Popen(["../bin/neurons", "-c", "simul1", "6", "512", str(density)], 
                      stdout=PIPE).communicate()[0]))
                      
plt.semilogy([0] + X[8:] + [1], [0] + Y + [1], "--", marker='x', label="c=6, l=512")
  
#3                
Y = []
for density in X[12:]:
    print "c=8, l=512, density: ", density
    Y.append(float(Popen(["../bin/neurons", "-c", "simul1", "8", "512", str(density)], 
                      stdout=PIPE).communicate()[0]))
                      
plt.semilogy([0] + X[12:] + [1], [0] + Y + [1], "--", marker='^', label="c=8, l=512")

#plot
plt.xlabel("Network density (d)")
plt.ylabel("Probability of accepting a random message")
plt.legend(loc="upper left")

plt.show()