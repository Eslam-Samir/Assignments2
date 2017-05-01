import numpy as np
from sklearn import svm
from sklearn.metrics import accuracy_score
from sklearn.cross_validation import train_test_split


# load file
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


def train_svm_classifier(classifier, train_data, train_labels, test_data, test_labels):
    classifier.fit(train_data, train_labels)

    train_predictions = classifier.predict(train_data)
    train_accuracy = accuracy_score(train_labels, train_predictions, normalize=True)
    train_error = 1 - train_accuracy

    test_predictions = classifier.predict(test_data)
    test_accuracy = accuracy_score(test_labels, test_predictions, normalize=True)
    test_error = 1 - test_accuracy

    return train_error, test_error

positive_labels_file = open('positive.dat', 'r')
negative_labels_file = open('negative.dat', 'r')
X, Y = prepare_data(positive_labels_file, negative_labels_file)

# split the data into half
train_data, test_data, train_labels, test_labels = train_test_split(X, Y, test_size=0.5, random_state=0)
clf = svm.LinearSVC(random_state=False, verbose=False)

train_error, test_error = train_svm_classifier(clf, train_data[:1000], train_labels[:1000], test_data, test_labels)

print("For 1000 training points: ")
print("Train Error: %.3f%%" % (train_error * 100))
print("Test Error: %.3f%%" % (test_error * 100))
