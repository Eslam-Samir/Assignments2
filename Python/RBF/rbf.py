import numpy as np
import random
import matplotlib.pyplot as plt


# Usage: data = dbmoon(N, d, r, w)
# doublemoon.m - genereate the double moon data set in Haykin's book titled
# "neural networks and learning machine" third edition 2009 Pearson
# Figure 1.8 pp.61
# The data set contains two regions A and B representing 2 classes
# each region is a half ring with radius r = 10, width = 6, one is upper
# half and the other is lower half
# d: distance between the two regions
# will generate region A centered at (0, 0) and region B is a mirror image
# of region A (w.r.t.x axis) with a (r, d) shift of origin
# N:  # of samples each class, default = 1000
# d: seperation of two class, negative value means overlapping (default=1)
# r: radius (default=10), w: width of ring (default=6)
def dbmoon(N=1000, d=1, r=10, w=6):
    # clear all; close all;
    # generate region A:
    # first generate a uniformly random distributed data points from (-r-w / 2, 0)
    # to (r+w / 2, r+w / 2)
    N1 = 10 * N  # generate more points and select those meet criteria
    w2 = w / 2
    done = 0
    tmp1 = np.array([[]])
    while not done:
        tmp = np.array(2 * (r + w2) * (np.random.uniform(0, 1, (N1, 1)) - 0.5))
        tmp = np.column_stack((tmp, (r + w2) * np.random.uniform(0, 1, (N1, 1))))
        # 3rd column of tmp is the magnitude of each data point
        thirdCol = np.sqrt(np.multiply(tmp[:, 0], tmp[:, 0]) + np.multiply(tmp[:, 1], tmp[:, 1]))
        tmp = np.column_stack((tmp, thirdCol.T))
        idx = np.nonzero(np.logical_and((tmp[:, 2] > (r - w2)), (tmp[:, 2] < (r + w2))))
        if tmp1.shape == (1, 0):
            tmp1 = tmp[:, 0:2][idx]
        else:
            tmp1 = np.row_stack((tmp1, tmp[:, 0:2][idx]))
        if len(idx[0]) >= N:
            done = 1
            # if not enough data point, generate more and test

    # region A data and class label 0
    # region B data is region A data flip y coordinate - d, and x coordinate +r
    data = np.column_stack((tmp1[0:N, :], np.zeros((N, 1))))
    data1 = np.column_stack(((tmp1[0:N, 0] + r), (-tmp1[0:N, 1] - d), np.ones((N, 1))))
    data = np.row_stack((data, data1))
    # plot(data(1:N, 1), data(1:N, 2), '.r', data(N+1:end, 1), data(N+1:end, 2), '.b');
    # title(['Fig. 1.8 Double moon data set, d = ' num2str(d)]),
    # axis([-r-w2 2 * r+w2 -r-w2-d r+w2])
    return data


def k_means(data, k, threshold):
    indices = random.sample(range(len(data)), k)
    mu = np.array([data[i] for i in sorted(indices)])
    old_cost = 0
    while True:
        distances = {}
        for i in range(k):
            distances[i] = np.sum(((data[:] - mu[i]) ** 2), axis=1)

        clusters = {}
        for i in range(k):
            idx = np.where(distances[i] <= list((distances[j] for j in range(k) if j != i)))
            clusters[i] = data[idx[1], :]
            mu[i] = np.mean(clusters[i], axis=0)
        # cost calculation
        cost = 0
        for i in range(k):
            cost += np.sum(((clusters[i][:] - mu[i]) ** 2))

        if cost - old_cost <= threshold:
            break
        old_cost = cost

    return mu


# initialization

NumberOfOutputUnits = 1
NumberOfHiddenUnits = 4
NumberOfInputsUnits = 2

data = dbmoon(1000)
target = data[:, 2:3]
data = data[:, 0:2]

k_means_threshold = 0.00001

initial_segma = np.random.rand(1, NumberOfHiddenUnits)
initial_weights = np.random.rand(NumberOfHiddenUnits, NumberOfOutputUnits)
initial_mu = k_means(data, NumberOfHiddenUnits, k_means_threshold)

stepsize_w = 0.1
stepsize_mu = 0.1
stepsize_segma = 0.1