import math
import scipy.stats

# A single Y neuron receives (or not) input from neuronsInput neurons
# epsilon01 is the probability that no input results in Y lit up
# epsilon10 is the probability that input results in Y off

# This is equivalent to a binary asymmetric channel, calculus of the capacity
#  is at the end

nsyn = 20
prel = 0.3
neuronsInput = 6
threshold = 0.7 #Percentage compared to the average number of synapses triggered

lambd = 0.000
minDiff = 3

insignificant = 0.0001
k = 0

lambdaProbas = {}

while 1:
    lambdaProbas[k] = lambd**k*1.0/(math.factorial(k)) * math.exp(-lambd)

    if (lambdaProbas[k] < insignificant):
        break
        
    k = k+1
    
kmax = k

minSynapses = math.ceil(nsyn * threshold * prel * neuronsInput)

epsilon01 = 0.0
epsilon10 = 0.0

for i in range(0, kmax+1):
    if minDiff > 0:
        epsilon01 += lambdaProbas[i] * (1 - scipy.stats.binom.cdf(minSynapses - 1, nsyn*(i+neuronsInput-minDiff), prel))
        epsilon10 += lambdaProbas[i] * scipy.stats.binom.cdf(minSynapses - 1, nsyn*(i+neuronsInput), prel)    
    else:
        epsilon01 += lambdaProbas[i] * (1 - scipy.stats.binom.cdf(minSynapses - 1, nsyn*i, prel))
        epsilon10 += lambdaProbas[i] * scipy.stats.binom.cdf(minSynapses - 1, nsyn*(i+neuronsInput), prel)

print epsilon01, epsilon10

def entropy(x):
    return -x*math.log(x, 2) -(1-x)*math.log(1-x, 2)
    
def f(x):
    return entropy(x)/(1-x)
    
p1 = epsilon01
p2 = epsilon10

#using formula for capacity from http://mathoverflow.net/questions/96493/computing-channel-capacities-for-non-symmetric-channels

if p1 > p2:
    p1, p2 = p2, p1

#p1, p2 = p2, p1

if epsilon01 > 0:
    z = math.pow(2, (entropy(p1) - entropy(p2)) / (1-p1-p2) )
    
    C = math.log(z+1, 2) - (1-p2)/(1-p1-p2)*entropy(p1) + p1/(1-p1-p2)*entropy(p2)
else:
    C = entropy(1/ (1 + math.exp(f(p2)))) - f(p2)/(1+math.exp(f(p2)))
    
print C