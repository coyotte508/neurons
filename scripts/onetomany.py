#!/usr/bin/python

# This is a try to get a one-to-many injection between two set of clusters of neurons.
# The injection is naturally not achievable if given a full set (unless we use
#  the identity function) but it is if we only consider a subset. It is a tiny bit similar
# to hashing.
#
# The program works by creating enough c-to-c matching amongst 2 groups of chi clusters
# So that the total number of connections reach in average a density d.
#
# We then take random messages from the first group of chi clusters and see if they correspond
# to a message with only one neuron per cluster in the destination network. We do that for different
# densities, and mesure the average number of neurons activated in the destination clusters. We could
# use medians instead.
#
# It is possible that the matching can be improved using inhibition.

import random
import math
import matplotlib.pyplot as plt

random.seed()

l = 50
chi = 8
c = 6

#density d
def getLength(d): 
    partMessages = math.log(1-d)/math.log(1-(1.*c**2)/(chi*chi*l*l))
    print "messages:", partMessages

    conns = [{x : set() for x in range(l)} for _ in range(chi)]

    doneMessages = set()

    for _ in range(int(partMessages)):
        message = zip([random.randrange(0, l) for _ in range(c)], random.sample(xrange(chi), c))
        messageDest = zip([random.randrange(0, l) for _ in range(c)], random.sample(xrange(chi), c))
        messageDest = set([a + b*l for a,b in messageDest])

        messageSet = tuple(sorted([a + b*l for a,b in message]))
        if messageSet in doneMessages:
            continue
        else:
            doneMessages.add(messageSet)

        for f, x in message:
            conns[x][f] |= messageDest

    results = []
    samples = 100
    for _ in range(samples):
        message = [random.randrange(0, l) for _ in range(chi)]
        
        connlist = [conns[x][message[x]] for x in range(chi)]
        fullmap = {key: 0 for key in set.union(*connlist)}

        for conn in connlist:
            for x in conn:
                fullmap[x] += 1

        result = []
        for i in range(chi):
            maxScore = 0
            tempResult = []
            for x in range(l*i, l*(i+1)):
                if x not in fullmap:
                    continue
                if fullmap[x] > maxScore:
                    maxScore = fullmap[x]
                    tempResult = []
                if fullmap[x] >= maxScore:
                    tempResult.append(x)
            result.extend(tempResult)

        #ideally, only one element of each
        #print set.intersection(*connlist)
        #print result
        results.append(result)

    return sum(len(x) for x in results) / float(len(results))

X = [float(x)*2/100 for x in range(1, 41)]
Y = [getLength(x) for x in X]

plt.plot(X, Y)
plt.show()