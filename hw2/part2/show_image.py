# -*- coding: utf-8 -*-
"""
Created on Sun Oct 13 10:32:42 2024

@author: kunyao
"""

import matplotlib.pyplot as plt
import cv2

threads = [1, 2, 3, 4, 5, 6]
speedup1 = [1, 1.99, 2.93, 3.81, 4.64, 5.58]
speedup2 = [1, 1.96, 2.93, 3.81, 4.74, 5.55]

plt.plot(threads, speedup1, marker='x', linestyle='-', color='b', label="view 1")
plt.plot(threads, speedup2, marker='o', linestyle='-', color='r', label="view 2")

plt.xlabel("Number of threads")
plt.ylabel("SpeedUp(times)")
plt.title("totalRows = 1")
plt.legend()
plt.show()

time1 = [384.201, 193.356, 131.184, 100.828, 82.770, 68.969]
time2 = [224.846, 115.105, 76.837, 58.994, 47.493, 40.452]

plt.plot(threads, time1, marker='x', linestyle='-', color='b', label="view 1")
plt.plot(threads, time2, marker='o', linestyle='-', color='r', label="view 2")

plt.xlabel("Number of threads")
plt.ylabel("Execution time(ms)")
plt.title("totalRows = 1")
plt.legend()
plt.show()

"""block"""
threads = [1, 2, 3, 4, 5, 6]
speedup1 = [1, 1.95, 1.62, 2.38, 2.46, 3.14]
speedup2 = [1, 1.68, 2.17, 2.59, 2.95, 3.20]

plt.plot(threads, speedup1, marker='x', linestyle='-', color='b', label="view 1")
plt.plot(threads, speedup2, marker='o', linestyle='-', color='r', label="view 2")

plt.xlabel("Number of threads")
plt.ylabel("SpeedUp(times)")
plt.title("totalRows = height / numThreads")
plt.legend()
plt.show()


time1 = [384.201, 196.716, 237.116, 161.329, 156.375, 122.361]
time2 = [224.846, 133.578, 103.735, 86.862, 76.124, 70.206]

plt.plot(threads, time1, marker='x', linestyle='-', color='b', label="view 1")
plt.plot(threads, time2, marker='o', linestyle='-', color='r', label="view 2")

plt.xlabel("Number of threads")
plt.ylabel("Execution time(ms)")
plt.title("totalRows = height / numThreads")
plt.legend()
plt.show()

#%%

for i in range(6):
    src = cv2.imread('C:/Users/kunyao/Desktop/view2.png')
    h, w, _ = src.shape
    step = h // (i+1)
    line = [j for j in range(step-1, h-1, step)]
    print(line)
    if len(line) > 0:
        for l in range(i):
            print(l, end=" ")
            cv2.line(src, (0, line[l]), (w-1, line[l]), (255, 255, 0), 3)
        print()
    plt.subplot(2, 3, i+1)
    plt.imshow(src)
    plt.axis("off")
plt.subplots_adjust(wspace=0.01, hspace=-0.47) 
plt.show()