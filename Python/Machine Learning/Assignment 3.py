import numpy as np
from sklearn import svm
from sklearn.naive_bayes import GaussianNB
import matplotlib.pyplot as plt


# load files
def prepare_data(positive_file, negative_file):
    positive_data = np.zeros((1, 50))
    for line in positive_file:
        features = line.split(' ')
        data_point = []
        for x in features[1:51]:
            f = x.split(':')
            data_point.append(float(f[1]))
        positive_data = np.vstack((positive_data, np.array([data_point])))

    positive_data = positive_data[1:, :]

    negative_data = np.zeros((1, 50))
    for line in negative_file:
        features = line.split(' ')
        data_point = []
        for x in features[1:51]:
            f = x.split(':')
            data_point.append(float(f[1]))
        negative_data = np.vstack((negative_data, np.array([data_point])))

    negative_data = negative_data[1:, :]
    data = np.vstack((positive_data, negative_data))
    labels = np.hstack((np.ones(5000), -1*np.ones(5000)))
    return data, labels


# split the data
def split_data(data, labels, train_size):
    if train_size > 5000:
        train_size = 5000

    training_data = np.vstack((data[0:(train_size/2)], data[5000:5000+(train_size / 2)]))
    training_labels = np.hstack((labels[0:(train_size / 2)], labels[5000:5000+(train_size / 2)]))

    testing_data = np.vstack((data[2500:5000], data[7500:10000]))
    testing_labels = np.hstack((labels[2500:5000], labels[7500:10000]))

    return training_data, testing_data, training_labels, testing_labels


# train the classifier and calculate train & test errors
def train_polynomial_svm_classifier(degree, training_data, training_labels, testing_data, testing_labels):
    clf = svm.SVC(kernel='poly', degree=degree, max_iter=30000, random_state=False, verbose=False)
    clf.fit(training_data, training_labels)

    training_accuracy = clf.score(training_data, training_labels)
    training_error = 1 - training_accuracy

    testing_accuracy = clf.score(testing_data, testing_labels)
    testing_error = 1 - testing_accuracy

    return training_error, testing_error


def train_rbf_svm_classifier(c, gamma, training_data, training_labels, testing_data, testing_labels):
    clf = svm.SVC(kernel='rbf', C=c,gamma=gamma,  max_iter=30000, random_state=False, verbose=False)
    clf.fit(training_data, training_labels)

    training_accuracy = clf.score(training_data, training_labels)
    training_error = 1 - training_accuracy

    testing_accuracy = clf.score(testing_data, testing_labels)
    testing_error = 1 - testing_accuracy

    return training_error, testing_error


def plot_error(x, train_errors, test_errors, x_label, title, legend, colors):
    fig = plt.figure()
    fig.canvas.set_window_title(title)
    j = 0
    for i in range(len(legend)):
        training_error = [row[i] for row in train_errors]
        testing_error = [row[i] for row in test_errors]
        plt.plot(x, training_error, color=colors[j], linewidth=1.5, linestyle="-", label="Train Error " + legend[i])
        plt.plot(x, testing_error, color=colors[j+1], linewidth=1.5, linestyle="-", label="Test Error " + legend[i])
        j += 2
    plt.xlabel(x_label)
    plt.ylabel("Error")
    plt.axis([min(x), max(x), 0, 1])
    plt.title(title)
    plt.legend(loc='upper left')


def subplot_error(x, train_errors, test_errors, x_label, title):
    fig = plt.figure()
    fig.suptitle(title)
    fig.canvas.set_window_title(title)
    ax1 = plt.subplot2grid(shape=(2, 6), loc=(0, 0), colspan=2)
    ax2 = plt.subplot2grid((2, 6), (0, 2), colspan=2)
    ax3 = plt.subplot2grid((2, 6), (0, 4), colspan=2)
    ax4 = plt.subplot2grid((2, 6), (1, 1), colspan=2)
    ax5 = plt.subplot2grid((2, 6), (1, 3), colspan=2)
    axes = [ax1, ax2, ax3, ax4, ax5]
    for i in range(1, 6):
        axes[i-1].plot(x, train_errors[i-1], color="blue", linewidth=1.5, linestyle="-", label="Train Error")
        axes[i-1].plot(x, test_errors[i-1], color="Red", linewidth=1.5, linestyle="-", label="Train Error")
        axes[i-1].axis([min(x), max(x), 0, 1])
        axes[i-1].set_title(str(i*1000) + " training points")
        axes[i - 1].set_ylabel("Error")
        axes[i - 1].set_xlabel(x_label)
    ax1.legend(loc='upper left')


print("############################## Preparing Data ##############################\n")

positive_labels_file = open('positive.dat', 'r')
negative_labels_file = open('negative.dat', 'r')
X, Y = prepare_data(positive_labels_file, negative_labels_file)

print("################################# Training #################################")
print("########################## SVM Polynomial Kernel ###########################\n")

train_errors_degree = []
test_errors_degree = []
x_range = []
legend = []

for i in range(1, 6):
    num_points = i * 1000
    train_data, test_data, train_labels, test_labels = split_data(X, Y, num_points)
    print("############################ %d training points ############################\n" % num_points)
    j = 0
    train_error_degree = []
    test_error_degree = []
    while j <= 5:
        print('\nPolynomial of Degree = %d:\n' % j)
        train_error, test_error = train_polynomial_svm_classifier(j, train_data, train_labels, test_data, test_labels)
        train_error_degree.append(train_error)
        test_error_degree.append(test_error)
        if i == 1:
            x_range.append(j)
            legend.append("Degree = " + str(j))
        j += 1
        print("Train Error: %.3f%%" % (train_error * 100))
        print("Test Error: %.3f%%" % (test_error * 100))

    train_errors_degree.append(train_error_degree)
    test_errors_degree.append(test_error_degree)

subplot_error(x_range, train_errors_degree, test_errors_degree, "Polynomial Degree", "Polynomial SVMs")

colors = ['#ff0000', '#990000', '#00ff00', '#009900', '#0000ff', '#000099',
          '#cc6600', '#ff9933', '#9933ff', '#ff99ff', '#003300', '#336600']
plot_error([1000, 2000, 3000, 4000, 5000], train_errors_degree,
           test_errors_degree, "Train Data Size", "Polynomial SVMs", legend, colors)

print("########################## SVM Gaussian Kernel ###########################\n")

train_errors_gamma = []
test_errors_gamma = []
train_errors_c = []
test_errors_c = []

x_range_gamma = []
x_range_c = []

for i in range(1, 6):
    num_points = i * 1000
    train_data, test_data, train_labels, test_labels = split_data(X, Y, num_points)
    print("############################ %d training points ############################\n" % num_points)
    j = 10**-6
    train_error_gamma = []
    test_error_gamma = []
    while j <= 0.1:
        print('\nGamma = %f:\n' % j)
        train_error, test_error = train_rbf_svm_classifier(1, j, train_data, train_labels, test_data, test_labels)
        train_error_gamma.append(train_error)
        test_error_gamma.append(test_error)
        if i == 1:
            x_range_gamma.append(np.log(j)/np.log(10))
        j *= 10
        print("Train Error: %.3f%%" % (train_error * 100))
        print("Test Error: %.3f%%" % (test_error * 100))

    train_errors_gamma.append(train_error_gamma)
    test_errors_gamma.append(test_error_gamma)

    x_range = []
    train_error_c = []
    test_error_c = []
    j = 0.05
    while j <= 0.3:
        print('\nC = %.2f:\n' % j)
        train_error, test_error = train_rbf_svm_classifier(j, 10 ** -4, train_data, train_labels, test_data,
                                                           test_labels)
        train_error_c.append(train_error)
        test_error_c.append(test_error)
        if i == 1:
            x_range_c.append(j)
        j += 0.05
        print("Train Error: %.3f%%" % (train_error * 100))
        print("Test Error: %.3f%%" % (test_error * 100))

    train_errors_c.append(train_error_c)
    test_errors_c.append(test_error_c)

subplot_error(x_range_gamma, train_errors_gamma, test_errors_gamma, "Log(Gamma)", "Gaussian SVMs")

subplot_error(x_range_c, train_errors_c, test_errors_c, "C", "Gaussian SVMs")


print("############################ Bayes Classifier #############################\n")

gnb = GaussianNB()
y_pred = gnb.fit(X, Y).predict(X)
error = float((Y != y_pred).sum()) / len(Y) * 100
print("Bayes Error = %.3f%%" % error)

plt.show()
