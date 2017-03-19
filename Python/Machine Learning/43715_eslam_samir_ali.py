import numpy as np
import matplotlib.pyplot as plt


# ------------------------------- MAP Methods --------------------------------- #

def MAP_Discriminant(x, mu, covariance, probability):
    x_mu = np.subtract(x, mu)                       # x - mu
    x_mu_t = np.transpose(x_mu)                     # (x - mu)t
    covariance_inverse = np.linalg.inv(covariance)  # covariance ^ -1
    d = mu.shape[0]                                 # num of dimensions
    det = np.linalg.det(covariance)                 # | covariance |
    # g(x):
    disc = -0.5 * np.dot(x_mu_t, np.dot(covariance_inverse, x_mu)) - (d / 2) * np.log(np.pi / 2) - 0.5 * np.log(
        det) + np.log(probability)
    return disc


def MAP_Classifier(test, mu, cov, prior):
    disc = [MAP_Discriminant(test, mu[0], cov[0], prior[0]),
            MAP_Discriminant(test, mu[1], cov[1], prior[1]),
            MAP_Discriminant(test, mu[2], cov[2], prior[2])]

    return disc.index(max(disc)) + 1


# ----------------------- Mahalanobis Distance Method ------------------------- #

def Mahalanobis_Distance(x, mu, covariance):
    x_mu = np.subtract(x, mu)
    x_mu_t = np.transpose(x_mu)
    covariance_inverse = np.linalg.inv(covariance)
    distance = np.dot(x_mu_t, np.dot(covariance_inverse, x_mu))
    return distance

# ------------------------------- ML Methods --------------------------------- #

def ML_Discriminant(x, mu, covariance):
    x_mu = np.subtract(x, mu)                       # x - mu
    x_mu_t = np.transpose(x_mu)                     # (x - mu)t
    covariance_inverse = np.linalg.inv(covariance)  # covariance ^ -1
    d = mu.shape[0]                                 # num of dimensions
    det = np.linalg.det(covariance)                 # | covariance |
    # g(x):
    disc = -0.5 * np.dot(x_mu_t, np.dot(covariance_inverse, x_mu)) - (d / 2) * np.log(np.pi / 2) - 0.5 * np.log(det)
    return disc


def ML_Classifier(test, mu, cov):
    disc = [ML_Discriminant(test, mu[0], cov[0]),
            ML_Discriminant(test, mu[1], cov[1]),
            ML_Discriminant(test, mu[2], cov[2])]

    return disc.index(max(disc)) + 1

# ------------------------ Mean And Covariance Methods ------------------------ #

def mean(x):
    num_samples = 10
    s = np.transpose([np.sum(x, axis=1)])
    mu = (1.0 / num_samples) * s
    return mu


def covariance(x, mu):
    num_samples = x.shape[1]
    x_mu = np.subtract(x, mu)
    x_mu_t = np.transpose(x_mu)
    return (1.0 / num_samples) * np.dot(x_mu, x_mu_t)

# --------------------------------- Train Data ---------------------------------- #

x1 = np.array([[-5.01, -5.43, 1.08, 0.86, -2.67, 4.94, -2.51, -2.25, 5.56, 1.03],
               [-8.12, -3.48, -5.52, -3.78, 0.63, 3.29, 2.09, -2.13, 2.86, -3.33],
               [-3.68, -3.54, 1.66, -4.11, 7.39, 2.08, -2.59, -6.94, -2.26, 4.33]])
x2 = np.array([[-0.91, 1.30, -7.75, -5.47, 6.14, 3.60, 5.37, 7.18, -7.39, -7.50],
               [-0.18, -2.06, -4.54, 0.50, 5.72, 1.26, -4.63, 1.46, 1.17, -6.32],
               [-0.05, -3.53, -0.95, 3.92, -4.85, 4.36, -3.65, -6.66, 6.30, -0.31]])
x3 = np.array([[5.35, 5.12, -1.34, 4.48, 7.11, 7.17, 5.75, 0.77, 0.90, 3.52],
               [2.26, 3.22, -5.31, 3.42, 2.39, 4.33, 3.97, 0.27, -0.43, -0.36],
               [8.13, -2.66, -9.87, 5.19, 9.21, -0.98, 6.65, 2.41, -8.71, 6.43]])
prior = [0.8, 0.1, 0.1]

# --------------------------------- Test Data ----------------------------------- #

p = [np.array([[1], [2], [1]]), np.array([[5], [3], [1]]), np.array([[0], [0], [0]]), np.array([[1], [0], [0]])]

# ---------------------------- Mean and Covariance ------------------------------ #

mu = [mean(x1), mean(x2), mean(x3)]
cov = [covariance(x1, mu[0]), covariance(x2, mu[1]), covariance(x3, mu[2])]

print "\n######################### Mean And Covariance #########################\n"
for i in range(len(mu)):
    print 'Class %d:' % (i+1)
    print "Mean:\n", mu[i]
    print "Covariance Matrix:\n", cov[i], "\n"

# -------------------------- Test MAP Classification ---------------------------- #

print "\n####################### Test MAP Classification #######################\n"
for i in range(len(p)):
    print 'P%d (%d, %d, %d):' % (i+1, p[i][0], p[i][1], p[i][2]), 'Class', MAP_Classifier(p[i], mu, cov, prior)

# --------------------------- MAP Decision Boundary ----------------------------- #

x_min = [x1[0, :].min(), x2[0, :].min(), x3[0, :].min()]
x_max = [x1[0, :].max(), x2[0, :].max(), x3[0, :].max()]
y_min = [x1[1, :].min(), x2[1, :].min(), x3[1, :].min()]
y_max = [x1[1, :].max(), x2[1, :].max(), x3[1, :].max()]

# draw top view decision boundary at x3 = 1
xx, yy = np.meshgrid(np.arange(min(x_min) - 1, max(x_max) + 1, 0.2), np.arange(min(y_min) - 1, max(y_max) + 1, 0.2))
Z = np.zeros((xx.shape[0], xx.shape[1]))
for i in range(xx.shape[0]):
    for j in range(xx.shape[1]):
            point = np.array([[xx[i, j]], [yy[i, j]], [1]])
            Z[i, j] = MAP_Classifier(point, mu, cov, prior)

fig1 = plt.figure()
# train data
plt.scatter(x1[0, :], x1[1, :], zorder=1, c='b', marker='o', label='Class 1')
plt.scatter(x2[0, :], x2[1, :], zorder=1, c='g', marker='D', label='Class 2')
plt.scatter(x3[0, :], x3[1, :], zorder=1, c='r', marker='^', label='Class 3')
# test data
plt.scatter(p[0][0], p[0][1], zorder=1, c='black', marker='x', label='Test Data')
plt.scatter(p[1][0], p[1][1], zorder=1, c='black', marker='x')
plt.scatter(p[2][0], p[2][1], zorder=1, c='black', marker='x')
plt.scatter(p[3][0], p[3][1], zorder=1, c='black', marker='x')

# decision boundary
plt.contourf(xx, yy, Z, levels=[0.75,1.25,1.75, 2.25, 2.75, 3.25], colors=['b','w','g','w','r'], zorder=-1)
fig1.suptitle('MAP Decision Boundary', fontsize=18)
plt.xlabel('X1', fontsize=16)
plt.ylabel('X2', fontsize=16)
plt.xticks(np.arange(min(x_min) - 0.5, max(x_max) + 1, 2))
plt.yticks(np.arange(min(y_min) - 0.5, max(y_max) + 1, 2))
plt.legend(bbox_to_anchor=(-0.20, 1.1), loc='upper left', ncol=1)

cbar = plt.colorbar(ticks=np.arange(1, 4))
cbar.set_label("class #")

# -------------------------- Test ML Classification ---------------------------- #

print "\n######################## Test ML Classification #######################\n"
for i in range(len(p)):
    print 'P%d (%d, %d, %d):' % (i + 1, p[i][0], p[i][1], p[i][2]), 'Class', ML_Classifier(p[i], mu, cov)

# --------------------------- ML Decision Boundary ----------------------------- #

# draw top view decision boundary at x3 = 1
Z = np.zeros((xx.shape[0], xx.shape[1]))
for i in range(xx.shape[0]):
    for j in range(xx.shape[1]):
        point = np.array([[xx[i, j]], [yy[i, j]], [1]])
        Z[i, j] = ML_Classifier(point, mu, cov)

fig2 = plt.figure()
# train data
plt.scatter(x1[0, :], x1[1, :], zorder=1, c='b', marker='o', label='Class 1')
plt.scatter(x2[0, :], x2[1, :], zorder=1, c='g', marker='D', label='Class 2')
plt.scatter(x3[0, :], x3[1, :], zorder=1, c='r', marker='^', label='Class 3')
# test data
plt.scatter(p[0][0], p[0][1], zorder=1, c='black', marker='x', label='Test Data')
plt.scatter(p[1][0], p[1][1], zorder=1, c='black', marker='x')
plt.scatter(p[2][0], p[2][1], zorder=1, c='black', marker='x')
plt.scatter(p[3][0], p[3][1], zorder=1, c='black', marker='x')

# decision boundary
plt.contourf(xx, yy, Z, levels=[0.75, 1.25, 1.75, 2.25, 2.75, 3.25], colors=['b', 'w', 'g', 'w', 'r'], zorder=-1)
fig2.suptitle('ML Decision Boundary', fontsize=18)
plt.xlabel('X1', fontsize=16)
plt.ylabel('X2', fontsize=16)
plt.xticks(np.arange(min(x_min) - 0.5, max(x_max) + 1, 2))
plt.yticks(np.arange(min(y_min) - 0.5, max(y_max) + 1, 2))

cbar = plt.colorbar(ticks=np.arange(1, 4))
cbar.set_label("Class #")
plt.legend(bbox_to_anchor=(-0.20, 1.1), loc='upper left', ncol=1)
plt.show()

