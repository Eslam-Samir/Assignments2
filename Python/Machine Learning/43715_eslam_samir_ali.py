import numpy as np
import matplotlib.pyplot as plt


# ----------------------- Mahalanobis Distance Method ------------------------- #

def Mahalanobis_Distance(x, mu, covariance):
    x_mu = np.subtract(x, mu)                       # x - mu
    x_mu_t = np.transpose(x_mu)                     # (x - mu)t
    covariance_inverse = np.linalg.inv(covariance)  # covariance ^ -1
    distance = np.dot(x_mu_t, np.dot(covariance_inverse, x_mu))
    return distance


# ------------------------------- MAP Methods --------------------------------- #

def MAP_Discriminant(x, mu, covariance, probability):
    distance = Mahalanobis_Distance(x, mu, covariance)
    d = float(mu.shape[0])                                 # num of dimensions
    det = np.linalg.det(covariance)                 # | covariance |
    # g(x):
    disc = -0.5 * distance - (d / 2.0) * np.log(np.pi / 2.0) - 0.5 * np.log(
        det) + np.log(probability)
    return disc


def MAP_Classifier(test, mu, cov, prior):
    disc = [MAP_Discriminant(test, mu[0], cov[0], prior[0]),
            MAP_Discriminant(test, mu[1], cov[1], prior[1]),
            MAP_Discriminant(test, mu[2], cov[2], prior[2])]
    return disc.index(max(disc)) + 1


def MAP_Plot(x1, x2, x3, p, mu, cov, prior):
    fig = plt.figure()
    h = 0.2
    x_min = [x1[0, :].min(), x2[0, :].min(), x3[0, :].min()]
    x_max = [x1[0, :].max(), x2[0, :].max(), x3[0, :].max()]
    y_min = [x1[1, :].min(), x2[1, :].min(), x3[1, :].min()]
    y_max = [x1[1, :].max(), x2[1, :].max(), x3[1, :].max()]
    z_min = [x1[2, :].min(), x2[2, :].min(), x3[2, :].min()]
    z_max = [x1[2, :].max(), x2[2, :].max(), x3[2, :].max()]
    xx1, yy1 = np.meshgrid(np.arange(min(x_min) - 1, max(x_max) + 1, h),
                         np.arange(min(y_min) - 1, max(y_max) + 1, h))
    xx2, zz1 = np.meshgrid(np.arange(min(x_min) - 1, max(x_max) + 1, h),
                         np.arange(min(z_min) - 1, max(z_max) + 1, h))
    yy2, zz2 = np.meshgrid(np.arange(min(y_min) - 1, max(y_max) + 1, h),
                         np.arange(min(z_min) - 1, max(z_max) + 1, h))
    xx = [xx1, xx2, np.ones((yy2.shape))]
    yy = [yy1, np.ones((xx2.shape)), yy2]
    zz = [np.ones((xx1.shape)), zz1, zz2]
    xlabels = ['X1', 'X1', 'X2']
    ylabels = ['X2', 'X3', 'X3']
    dim = [[0, 1], [0, 2], [1, 2]]
    for i in range(3):
        plt.subplot(2, 2, i + 1)
        plt.subplots_adjust(wspace=0.4, hspace=0.4)

        Z = np.zeros((xx[i].shape[0], xx[i].shape[1]))
        for j in range(xx[i].shape[0]):
            for k in range(xx[i].shape[1]):
                point = np.array([[xx[i][j, k]], [yy[i][j,k]], [zz[i][j, k]]])
                Z[j, k] = MAP_Classifier(point, mu, cov, prior)
        if i == 0:
            plt.contourf(xx[i], yy[i], Z, levels=[0.75, 1.25, 1.75, 2.25, 2.75, 3.25], colors=['b', 'w', 'g', 'w', 'r'],
                        zorder=-1)
            cbar = plt.colorbar(ticks=np.arange(1, 4))
            cbar.set_label("class #")
        elif i == 1:
            plt.contourf(xx[i], zz[i], Z, levels=[0.75, 1.25, 1.75, 2.25, 2.75, 3.25], colors=['b', 'w', 'g', 'w', 'r'],
                         zorder=-1)
            cbar = plt.colorbar(ticks=np.arange(1, 4))
            cbar.set_label("class #")
        elif i == 2:
            plt.contourf(yy[i], zz[i], Z, levels=[0.75, 1.25, 1.75, 2.25, 2.75, 3.25], colors=['b', 'w', 'g', 'w', 'r'],
                         zorder=-1)
            cbar = plt.colorbar(ticks=np.arange(1, 4))
            cbar.set_label("class #")

        # train data
        plt.scatter(x1[dim[i][0], :], x1[dim[i][1], :], zorder=1, c='b', marker='o', label='Class 1')
        plt.scatter(x2[dim[i][0], :], x2[dim[i][1], :], zorder=1, c='g', marker='D', label='Class 2')
        plt.scatter(x3[dim[i][0], :], x3[dim[i][1], :], zorder=1, c='r', marker='^', label='Class 3')
        # test data
        plt.scatter(p[0][dim[i][0]], p[0][dim[i][1]], zorder=1, c='black', marker='x', label='Test Data')
        plt.scatter(p[1][dim[i][0]], p[1][dim[i][1]], zorder=1, c='black', marker='x')
        plt.scatter(p[2][dim[i][0]], p[2][dim[i][1]], zorder=1, c='black', marker='x')
        plt.scatter(p[3][dim[i][0]], p[3][dim[i][1]], zorder=1, c='black', marker='x')

        # labels
        plt.xlabel(xlabels[i], fontsize=16)
        plt.ylabel(ylabels[i], fontsize=16)
        plt.legend(bbox_to_anchor=(-0.20, 1.1), loc='upper left', ncol=1, prop={'size': 8})
        plt.xticks(np.arange(-10, 10, 2))
        plt.yticks(np.arange(-10, 10, 2))
    fig.suptitle('MAP Decision Boundary', fontsize=18)


# ------------------------------- ML Methods --------------------------------- #

def ML_Discriminant(x, mu, covariance):
    distance = Mahalanobis_Distance(x, mu, covariance)
    d = float(mu.shape[0])                                 # num of dimensions
    det = np.linalg.det(covariance)                 # | covariance |
    # g(x):
    disc = -0.5 * distance - (d / 2.0) * np.log(np.pi / 2.0) - 0.5 * np.log(det)
    return disc


def ML_Classifier(test, mu, cov):
    disc = [ML_Discriminant(test, mu[0], cov[0]),
            ML_Discriminant(test, mu[1], cov[1]),
            ML_Discriminant(test, mu[2], cov[2])]
    return disc.index(max(disc)) + 1


def ML_Plot(x1, x2, x3, p, mu, cov):
    fig = plt.figure()
    h = 0.2
    x_min = [x1[0, :].min(), x2[0, :].min(), x3[0, :].min()]
    x_max = [x1[0, :].max(), x2[0, :].max(), x3[0, :].max()]
    y_min = [x1[1, :].min(), x2[1, :].min(), x3[1, :].min()]
    y_max = [x1[1, :].max(), x2[1, :].max(), x3[1, :].max()]
    z_min = [x1[2, :].min(), x2[2, :].min(), x3[2, :].min()]
    z_max = [x1[2, :].max(), x2[2, :].max(), x3[2, :].max()]
    xx1, yy1 = np.meshgrid(np.arange(min(x_min) - 1, max(x_max) + 1, h),
                           np.arange(min(y_min) - 1, max(y_max) + 1, h))
    xx2, zz1 = np.meshgrid(np.arange(min(x_min) - 1, max(x_max) + 1, h),
                           np.arange(min(z_min) - 1, max(z_max) + 1, h))
    yy2, zz2 = np.meshgrid(np.arange(min(y_min) - 1, max(y_max) + 1, h),
                           np.arange(min(z_min) - 1, max(z_max) + 1, h))
    xx = [xx1, xx2, np.ones((yy2.shape))]
    yy = [yy1, np.ones((xx2.shape)), yy2]
    zz = [np.ones((xx1.shape)), zz1, zz2]
    xlabels = ['X1', 'X1', 'X2']
    ylabels = ['X2', 'X3', 'X3']
    dim = [[0, 1], [0, 2], [1, 2]]
    for i in range(3):
        plt.subplot(2, 2, i + 1)
        plt.subplots_adjust(wspace=0.4, hspace=0.4)
        Z = np.zeros((xx[i].shape[0], xx[i].shape[1]))
        for j in range(xx[i].shape[0]):
            for k in range(xx[i].shape[1]):
                point = np.array([[xx[i][j, k]], [yy[i][j, k]], [zz[i][j, k]]])
                Z[j, k] = ML_Classifier(point, mu, cov)
        if i == 0:
            plt.contourf(xx[i], yy[i], Z, levels=[0.75, 1.25, 1.75, 2.25, 2.75, 3.25], colors=['b', 'w', 'g', 'w', 'r'],
                         zorder=-1)
            cbar = plt.colorbar(ticks=np.arange(1, 4))
            cbar.set_label("class #")
        elif i == 1:
            plt.contourf(xx[i], zz[i], Z, levels=[0.75, 1.25, 1.75, 2.25, 2.75, 3.25], colors=['b', 'w', 'g', 'w', 'r'],
                         zorder=-1)
            cbar = plt.colorbar(ticks=np.arange(1, 4))
            cbar.set_label("class #")
        elif i == 2:
            plt.contourf(yy[i], zz[i], Z, levels=[0.75, 1.25, 1.75, 2.25, 2.75, 3.25], colors=['b', 'w', 'g', 'w', 'r'],
                         zorder=-1)
            cbar = plt.colorbar(ticks=np.arange(1, 4))
            cbar.set_label("class #")

        # train data
        plt.scatter(x1[dim[i][0], :], x1[dim[i][1], :], zorder=1, c='b', marker='o', label='Class 1')
        plt.scatter(x2[dim[i][0], :], x2[dim[i][1], :], zorder=1, c='g', marker='D', label='Class 2')
        plt.scatter(x3[dim[i][0], :], x3[dim[i][1], :], zorder=1, c='r', marker='^', label='Class 3')
        # test data
        plt.scatter(p[0][dim[i][0]], p[0][dim[i][1]], zorder=1, c='black', marker='x', label='Test Data')
        plt.scatter(p[1][dim[i][0]], p[1][dim[i][1]], zorder=1, c='black', marker='x')
        plt.scatter(p[2][dim[i][0]], p[2][dim[i][1]], zorder=1, c='black', marker='x')
        plt.scatter(p[3][dim[i][0]], p[3][dim[i][1]], zorder=1, c='black', marker='x')

        # labels
        fig.suptitle('ML Decision Boundary', fontsize=18)
        plt.xlabel(xlabels[i], fontsize=16)
        plt.ylabel(ylabels[i], fontsize=16)
        plt.legend(bbox_to_anchor=(-0.20, 1.1), loc='upper left', ncol=1, prop={'size': 8})
        plt.xticks(np.arange(-10, 10, 2))
        plt.yticks(np.arange(-10, 10, 2))


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

MAP_Plot(x1, x2, x3, p, mu, cov, prior)

# -------------------------- Test ML Classification ---------------------------- #

print "\n######################## Test ML Classification #######################\n"
for i in range(len(p)):
    print 'P%d (%d, %d, %d):' % (i + 1, p[i][0], p[i][1], p[i][2]), 'Class', ML_Classifier(p[i], mu, cov)

# --------------------------- ML Decision Boundary ----------------------------- #

ML_Plot(x1, x2, x3, p, mu, cov)
plt.show()

