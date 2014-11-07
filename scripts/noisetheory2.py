# -*- coding: utf-8 -*-
"""
Created on Fri Nov  7 09:34:26 2014
"""

import __builtin__
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
        
phi1 = dict()

#Calculate P(B(10*ck, 0.5) < B(10*i, 0.5))
for i in range(0, ck+1):
    sum = float(0)
    for j in range (1, synapses*i+1):
        sum += ((1 - scipy.stats.binom.cdf(j, synapses*i, prelease))
                * scipy.stats.binom.pmf(j-1, synapses*ck, prelease))
        # Here we add the case when two neurons have the same score:
        sum += ((scipy.stats.binom.pmf(j, synapses*i, prelease))
                * scipy.stats.binom.pmf(j, synapses*ck, prelease))*0.5

    phi1[i] = sum
        
print phi1

phice = dict()
pmfce = dict()
pmf1 = dict()

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
        
print phice

print ""
print pmf1, __builtin__.sum(pmf1[y] for y in pmf1)
print ""
print pmfce, __builtin__.sum(pmfce[y] for y in pmfce)

Y = []
for mess in Mess:
    d = 1.*(c**2-c)/(l**2*(X**2-X))
    d = 1. - (1-d) ** mess
    
    pbXEdges = []

    for k in range(0, ck+1):
        pbXEdges.append(d**k * (1.-d) **(ck-k) * scipy.misc.comb(ck, k))
        
    sum = 0.
    
    for k in range (0, ck+1):
        sum += phi1[k] * pbXEdges[k]

    #Probability that one node fails        
    val1 = 1. - (1. - sum) ** (l-1)
    
    sum = 0.
    
    for k in range (0, ck+1):
        sum += phice[k] * pbXEdges[k]
        
    val2 = 1. - (1. - sum) ** (l*(X-c))

    Y.append(1 - (1-val1)*(1-val2))
    
plt.plot(Mess, Y, "-", marker="x")
