import matplotlib.pyplot as plt
import numpy as np

bias_map = []
x = []
y = []

with open("bias_map.txt", 'r') as f:
    bias_map = f.readlines()

for map in bias_map:
    bias, count = map.split(" ")
    x.append(int(bias))
    count = count.strip("\n")
    y.append(int(count))

X = np.array(x)
Y = np.array(y)

plt.bar(X, Y)
plt.xlabel("Bias")
plt.ylabel("Count")

plt.show()
    