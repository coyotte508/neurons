# -*- coding: utf-8 -*-
"""
Created on Fri Nov  7 09:34:26 2014
"""

import scipy
import scipy.stats
import matplotlib.pyplot as plt
import matplotlib
import numpy as np

figure = matplotlib.figure.Figure()
fcb = matplotlib.backends.backend_agg.FigureCanvasBase(figure)
supported_file_types = fcb.get_supported_filetypes()

print "calculating binomials..."

sparse = False

if not sparse:
    X = 8
    l = 256
    c = 8
    ck = 4
else:
    X = 100
    l = 64
    c = 12
    ck = 9
ce = c-ck

synapses = 10
prelease = 0.5

Mess = [x * 200 for x in range(76)]

phice = dict()
pmfce = dict()
pmf1 = dict()
pmf = dict()

for i in range(0, ck+1):
    pmf[i] = dict()
    for j in range(0, synapses*ck+1):
        pmf[i][j] = scipy.stats.binom.pmf(j, synapses*i, prelease)

for i in range(0, synapses*ck+1):
    pmf1 [i] = scipy.stats.binom.pmf(i, synapses*ck, prelease)
                
for i in range(0, synapses*ck):
    pmfce [i] = (scipy.stats.binom.cdf(synapses*ck - i, synapses*ck, prelease) ** ce
                - scipy.stats.binom.cdf(synapses*ck - i - 1, synapses*ck, prelease) ** ce)
pmfce[synapses*ck] = pmfce[0]

#Calculate P(B(10*ck, 0.5) < B(10*i, 0.5))
for i in range(0, ck+1):
    sum = float(0)
    for j in range (1, synapses*i+1):
        sum += ((1 - scipy.stats.binom.cdf(j-1, synapses*i, prelease))
                * pmfce[j-1])
        # Here we add the case when two neurons have the same score:
        sum += ((scipy.stats.binom.pmf(j, synapses*i, prelease))
                * pmfce[j]) * 0.5

    phice[i] = sum

Y = []
Y2 = []
for mess in Mess:
    d = 1.*(c**2-c)/(l**2*(X**2-X))
    d = 1. - (1-d) ** mess
    
    pbXEdges = []

    for k in range(0, ck+1):
        pbXEdges.append(d**k * (1.-d) **(ck-k) * scipy.misc.comb(ck, k))
        
    psucc = 0
    #peq: probability that an incorrect node has a score x.
    peq = [0 for x in range(synapses*ck+1)]
    for i in range(0, ck+1):
        for x in range(0, synapses*ck+1):
            peq[x] += pmf[i][x] * pbXEdges[i]
            
    for x0 in range (0, synapses*ck+1):
        sum0 = float(0)
        for i in range(0, ck+1):
            for x in range(0, x0+1):
                sum0 += pmf[i][x] * pbXEdges[i] #* (0.5 if x == x0 else 1)
        psucc += sum0**(l-1) * pmf[ck][x0]

    #adjusted probability in full network    
    if not sparse:
        deamb = 0
        for x0 in range (0, synapses*ck+1):
            sum1 = 0
            for k in range(0, l-1):
                sum0 = float(0)
                t1 = 1./(k+1)
                t1 *=scipy.misc.comb(l-1, k)*peq[x0]**k
                for x in range(0, x0):
                    sum0 += peq[x]
                sum1 += t1 * sum0**(l-1-k)
            deamb += pmf[ck][x0] * sum1
            
        print(deamb)
        val1 = 1. - (deamb) ** ce#1. - (psucc) ** ce
    
    #adjusted probability in sparse network
    if sparse:
        deamb = 0
        psupp = 1.0
        for x0 in range (0, synapses*ck+1):
            psupp -= pmf[ck][x0]
            for j in range (1, ce):
                sum1 = 0
                for k in range(0, min((X-c)*l, 5)):
                    sum0 = float(0)
                    t1 = 1./scipy.misc.comb(j+k,j)
                    #print (X-c)*l, k, peq[x0]
                    t1 *=scipy.misc.comb((X-c)*l, k)*peq[x0]**k
                    for x in range(0, x0):
                        sum0 += peq[x]
                    sum1 += t1 * sum0**((X-c)*l-k)
                deamb += scipy.misc.comb(ce, j) * (pmf[ck][x0]**j) * (psupp**(ce-j))
        print(deamb)
        val1 = 1. - deamb
        
    #Probability that one node fails        
    Y.append(val1)
    
    #psucc1 = (1 - pbXEdges[ck])**(l-1)
    #Y2.append(1. - psucc1 ** ce)
#    sum2 = 0.
    
#    for k in range (0, ck+1):
#        sum2 += phice[k] * pbXEdges[k]
        
#    val2 = 1. - (1. - sum2) ** (l*(X-c))

#    Y.append(1 - (1-val1)*(1-val2))

print Y
    
plt.plot(Mess, Y, "-", label="c: " + str(c) + ", ck: " + str(ck) + ", l: " + str(l) + " (theory)")
#plt.plot(Mess, Y2, "-", marker="^", label="c: " + str(c) + ", ck: " + str(ck) + ", l: " + str(l) + " (theory perfect)")

plt.xlabel("Number of learnt messages (M)")
plt.ylabel("Error rate")
if ck == 7:
    plt.plot([650, 1300, 1950, 2600, 3250, 3900, 4550, 5200, 5850, 6500, 7150, 7800, 8450, 9100, 9750, 10400, 11050, 11700, 12350, 13000, 13650, 14300, 14950, 15600, 16250, 16900, 17550, 18200, 18850, 19500],
        [0.0, 0.0, 5e-05, 0.00015, 0.0, 0.00045, 0.0010000000000000002, 0.0014000000000000004, 0.002000000000000001, 0.0030000000000000014, 0.004650000000000003, 0.006900000000000005, 0.009400000000000007, 0.009900000000000008, 0.014799999999999978, 0.01954999999999995, 0.02139999999999996, 0.030899999999999928, 0.03534999999999994, 0.04289999999999994, 0.046899999999999976, 0.05935000000000001, 0.0715, 0.079, 0.09345000000000005, 0.09935000000000004, 0.1141499999999999, 0.12680000000000008, 0.14104999999999998, 0.15714999999999996],
         "--", marker='x', label="c: " + str(c) + ", ck: " + str(ck) + ", l: " + str(l) + " (simul)")
#plt.plot([1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000, 12000, 13000, 14000, 15000, 16000, 17000, 18000, 19000, 20000, 21000, 22000, 23000, 24000, 25000, 26000, 27000, 28000, 29000, 30000], 
#         [0.0001999998, 0.0008666658000000001, 0.002999997400000001, 0.009266660399999972, 0.018533324999999993, 0.037066660200000025, 0.061933315999999926, 0.09873330740000008, 0.14093330459999984, 0.19740000220000006, 0.2720000053999999, 0.3523333134000015, 0.43606664800000183, 0.5207333419999994, 0.5993333319999985, 0.6768666759999992, 0.7479333479999997, 0.8034666640000008, 0.8529999920000007, 0.8921999859999985, 0.9261333439999978, 0.9474666939999964, 0.9622000499999964, 0.977133409999996, 0.9839334119999967, 0.9882000699999961, 0.993133375999998, 0.9951333739999985, 0.9972666899999991, 0.9974000219999991],
#         "--", marker='x', label="c: " + str(c) + ", ck: " + str(ck) + ", l: " + str(l) + " (simul)")
if ck == 4:
    plt.plot([500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000, 10500, 11000, 11500, 12000, 12500, 13000, 13500, 14000, 14500, 15000],
         [0.0025499999999999997, 0.01075, 0.02275, 0.039349999999999996, 0.06215, 0.08645, 0.1224, 0.15525, 0.19715000000000002, 0.23565, 0.2835, 0.32195, 0.36879999999999996, 0.4192, 0.4719499999999999, 0.5155, 0.5532500000000001, 0.60095, 0.6449499999999999, 0.67375, 0.7082999999999999, 0.74015, 0.77025, 0.79665, 0.82115, 0.84055, 0.8584500000000002, 0.87325, 0.8910500000000001, 0.9079],
                  "--", marker='x', label="c: " + str(c) + ", ck: " + str(ck) + ", l: " + str(l) + " (simul)")
    plt.plot(Mess, [0.0, 4.3967683827084159e-08, 6.9920519429889083e-07, 3.5182096634889248e-06, 1.1051694420616265e-05, 2.6817639144471883e-05, 5.5270769810933373e-05, 0.00010177234101915733, 0.00017256006166055382, 0.00027471798105382916, 0.00041614614258023153, 0.00060552978490768794, 0.00085230786473200482, 0.0011666406670634011, 0.0015593762575455417, 0.0020420155323404021, 0.0026266756202839581, 0.0033260513938001557, 0.0041533748532950332, 0.0051223721602263739, 0.0062472181026538287, 0.0075424878007546159, 0.0090231054726668258, 0.010704290111097192, 0.012601497946153617, 0.014730361595369779, 0.017106625850561552, 0.019746080071695316, 0.022664487215761642, 0.025877509561496814, 0.029400631249859477, 0.03324907779762154, 0.03743773281074958, 0.041981052167716637, 0.046892976001252418, 0.052186838875030506, 0.057875278593176982, 0.063970144157136644, 0.070482403422000184, 0.077422051075364706, 0.084798017602089826, 0.092618079949275511, 0.10088877464928969, 0.10961531419176995, 0.11880150746683749, 0.12844968512153387, 0.13856063068412061, 0.14913351831001209, 0.16016585799974858, 0.17165344911260372, 0.18359034297099996, 0.19596881530469668, 0.20877934922695873, 0.2220106293639863, 0.23564954767411672, 0.2496812214059736, 0.26408902352728902, 0.27885462585171072, 0.29395805495498839, 0.3093777608473709, 0.32509069822111902, 0.34107241995586068, 0.35729718241213515, 0.37373806190260239, 0.39036708158278077, 0.40715534786633256, 0.42407319533701127, 0.44109033900891859, 0.45817603267303619, 0.47529923197490098, 0.49242876078676201, 0.50953347937510585, 0.52658245282083427, 0.54354511812996587, 0.5603914484679311, 0.57709211297426255],
             "-", label="c: " + str(c) + ", ck: " + str(ck) + ", l: " + str(l) + " (theory, no noise)")
plt.gca().grid(True)
plt.yticks(np.arange(0, 1.1, 0.1))
plt.xlim((0, 15000))
plt.legend(loc="upper left")