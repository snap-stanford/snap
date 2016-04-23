import matplotlib.pyplot as plt
import numpy as np

C = np.loadtxt('childCounts.txt')
for i in range(len(C)):
  total = sum(C[i])
  C[i] = C[i] / total * 100

f.close()

steps = range(len(C))
plt.plot(steps, C[:,0])
plt.plot(steps, C[:,1])
plt.plot(steps, C[:,2])
plt.plot(steps, C[:,3])
plt.show()
