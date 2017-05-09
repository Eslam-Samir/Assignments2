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


def plot_error(x, train_errors, test_errors, x_label, title):
    fig = plt.figure()
    plt.plot(x, train_errors, color="blue", linewidth=2.5, linestyle="-", label="Train Error")
    plt.plot(x, test_errors, color="Red", linewidth=2.5, linestyle="-", label="Test Error")
    plt.xlabel(x_label)
    plt.ylabel("Error")
    plt.axis([min(x), max(x), 0, 1])
    plt.title(title)
    plt.legend(loc='upper left')


print("############################## Preparing Data ##############################\n")

positive_labels_file = open('positive.dat', 'r')
negative_labels_file = open('negative.dat', 'r')
X, Y = prepare_data(positive_labels_file, negative_labels_file)

print("################################# Training #################################")
print("########################## SVM Polynomial Kernel ###########################\n")

train_error_size = []
test_error_size = []

chosen_degree = 2

for i in range(1, 6):
    num_points = i * 1000
    train_data, test_data, train_labels, test_labels = split_data(X, Y, num_points)
    print("############################ %d training points ############################\n" % num_points)
    j = 0
    x_range = []
    train_error_degree = []
    test_error_degree = []
    while j <= 15:
        print('\nPolynomial of Degree = %d:\n' % j)
        train_error, test_error = train_polynomial_svm_classifier(j, train_data, train_labels, test_data, test_labels)
        train_error_degree.append(train_error)
        test_error_degree.append(test_error)
        x_range.append(j)
        j += 1
        print("Train Error: %.3f%%" % (train_error * 100))
        print("Test Error: %.3f%%" % (test_error * 100))

    plot_error(x_range, train_error_degree, test_error_degree,
               "Polynomial Degree", str(num_points) + " training points")

    train_error_size.append(train_error_degree[chosen_degree])
    test_error_size.append(test_error_degree[chosen_degree])

plot_error([1000, 2000, 3000, 4000, 5000], train_error_size,
           test_error_size, "Train Data Size", "Polynomials of Degree = " + str(chosen_degree))

print("############################ Bayes Classifier #############################\n")

gnb = GaussianNB()
y_pred = gnb.fit(X, Y).predict(X)
error = float((Y != y_pred).sum()) / len(Y) * 100
print("Bayes Error = %.3f%%" % error)

plt.show()
