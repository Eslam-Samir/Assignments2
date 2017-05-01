import numpy as np
import sklearn


# load file
def prepare_data(positive_file, negative_file):
    positive_data = np.zeros((50, 1))
    for line in positive_file:
        features = line.split(' ')
        data_point = []
        for x in features[1:51]:
            f = x.split(':')
            data_point.append(float(f[1]))
        positive_data = np.hstack((positive_data, np.array([data_point]).T))

    positive_data = positive_data[:, 1:]

    negative_data = np.zeros((50, 1))
    for line in negative_file:
        features = line.split(' ')
        data_point = []
        for x in features[1:51]:
            f = x.split(':')
            data_point.append(float(f[1]))
        negative_data = np.hstack((negative_data, np.array([data_point]).T))

    negative_data = negative_data[:, 1:]
    train_data = np.hstack((positive_data[:, :2499], negative_data[:, :2499]))
    train_labels = np.hstack((np.ones(2500), -1*np.ones(2500)))
    test_data = np.hstack((positive_data[:, 2500:], negative_data[:, 2500:]))
    test_labels = np.hstack((np.ones(2500), -1 * np.ones(2500)))
    return train_data, train_labels, test_data, test_labels

positive_labels_file = open('positive.dat', 'r')
negative_labels_file = open('negative.dat', 'r')
train_data, train_labels, test_data, test_labels = prepare_data(positive_labels_file, negative_labels_file)