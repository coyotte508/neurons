import matplotlib.pyplot as plt
import numpy as np

print "calculating binomials..."


X = 8
l = 26
c = 8
ck = 6

Mess = [x * 10 for x in range(31)]


plt.plot([15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165, 180, 195, 210, 225, 240, 255, 270, 285, 300],
         [0.0, 0.0, 2.9999999999999997e-05, 3e-05, 9.5e-05, 0.00021500000000000008, 0.0004700000000000003, 0.0006100000000000003, 0.0012049999999999982, 0.0018549999999999916, 0.002879999999999976, 0.004614999999999954, 0.006449999999999921, 0.008554999999999922, 0.012050000000000045, 0.01715000000000014, 0.022420000000000183, 0.028960000000000104, 0.036655000000000174, 0.04569000000000022],
        "--", marker='x', label="c: " + str(c) + ", ck: " + str(ck) + ", l: " + str(l) + " (alea)")
         
plt.xlabel("Number of learnt messages (M)")
plt.ylabel("Error rate")

#plt.plot([100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200],
#         [0.0017, 0.00445, 0.00565, 0.0121, 0.031, 0.0639, 0.10655, 0.19375, 0.2618, 0.3404, 0.44675],
#        "-", marker='*', label="c: " + str(c) + ", ck: " + str(ck) + ", l: " + str(l) + " (2 layers)")
        
plt.plot([100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200],
         [0.0023, 0.0021, 0.00315, 0.0062, 0.0093, 0.0178, 0.0327, 0.053, 0.07225, 0.1132, 0.16055],
        "-", marker='*', label="c: " + str(c) + ", ck: " + str(ck) + ", l: " + str(l) + " (2 layers, improved)")
        
plt.plot([100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200],
         [0.0023, 0.0017, 0.00275, 0.0052, 0.0071, 0.0112, 0.01655, 0.02175, 0.0291, 0.045, 0.0611],
        "-", marker='^', label="c: " + str(c) + ", ck: " + str(ck) + ", l: " + str(l) + " (code layer, strict)")

plt.plot([10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200],
         [0, 0.0013, 0.0045, 0.01395, 0.02225, 0.0375, 0.0594, 0.09075, 0.1008, 0.15775, 0.1883, 0.217, 0.2502, 0.2999, 0.3205, 0.3632, 0.38875, 0.42475, 0.44895, 0.4916],
        "-", marker='v', label="c: " + str(c) + ", ck: " + str(ck) + ", l: " + str(l) + " (mono-layer)")
        
plt.gca().grid(True)
plt.yticks(np.arange(0, 0.5, 0.05))
plt.legend(loc="upper left")