# -*- coding: utf-8 -*-
"""
Created on Thu Nov  6 10:59:38 2014

Calculate, after one iteration, with binomial noise, the probability of finding 
the correct clique.
"""

import scipy
import scipy.stats
import matplotlib.pyplot as plt

print "calculating binomials..."

X = 8
l = 256
c = 8
ck = 4
ce = c-ck

synapses = 10
prelease = 0.5

Mess = [x * 200 for x in range(100)]
        
results = dict()

pmfce = dict()
for i in range(0, synapses*ck):
    pmfce [i] = (scipy.stats.binom.cdf(synapses*ck - i, synapses*ck, prelease) ** ce
                - scipy.stats.binom.cdf(synapses*ck - i - 1, synapses*ck, prelease) ** ce)
pmfce[synapses*ck] = pmfce[0]

#for k in range(0, synapses*ck+1):
#    if scipy.stats.binom.cdf(k, synapses*ck, prelease) ** ce > 0.5:
#        averageMinimum = synapses*ck - k+1
#        break

averageMinimum = 0.
for k in range(0, synapses*ck+1):
    averageMinimum += k * pmfce[k]
    
print averageMinimum

paverageMinimum = 1-(averageMinimum-int(averageMinimum))

#Calculate P(B(10*ck, 0.5) < B(10*i, 0.5))
for i in range(0, ck+1):
    sum1 = float(0)
    for j in range (int(averageMinimum)+1, synapses*i+1):
        sum1 += scipy.stats.binom.pmf(j, synapses*i, prelease)
    # Here we add the case when two neurons have the same score:
    sum1 += 0.5 * scipy.stats.binom.pmf(int(averageMinimum), synapses*i, prelease) 
    
    sum2 = float(0)
    for j in range (int(averageMinimum)+2, synapses*i+1):
        sum2 += scipy.stats.binom.pmf(j, synapses*i, prelease)
    # Here we add the case when two neurons have the same score:
    sum2 += 0.5 * scipy.stats.binom.pmf(int(averageMinimum)+1, synapses*i, prelease) 
                 
    results[i] = sum2#sum1*paverageMinimum + sum2 * (1-paverageMinimum)
        
print results

Y = []
for mess in Mess:
    d = 1.*(c**2-c)/(l**2*(X**2-X))
    d = 1. - (1-d) ** mess
    
    pbXEdges = []

    for k in range(0, ck+1):
        pbXEdges.append(d**k * (1.-d) **(ck-k) * scipy.misc.comb(ck, k))
        
    sum = 0.
    
    for k in range (0, ck+1):
        sum += results[k] * pbXEdges[k]

    #Probability that one node fails        
    val = 1. - (1. - sum) ** ((l-1) + l*(X-c))
    #Probability that one of the node fails
    #val = 1. - (1. - val)**(c-ck)
    Y.append(val)
    
plt.plot(Mess, Y, "-")
