import re
from math import log

# load file and fill dictionaries
train_file = open('train', 'r')

spam_count = 0.0
ham_count = 0.0

vocab_spam = {}
vocab_ham = {}
vocab = {}
for line in train_file:
    words = re.split(" ", line)
    for i in range(2, len(words), 2):
        if words[1] == 'ham':
            ham_count += 1
            if words[i] in vocab_ham:
                vocab_ham[words[i]] += float(words[i + 1])
            else:
                vocab_ham[words[i]] = float(words[i + 1])
            if words[i] not in vocab_spam:
                vocab_spam[words[i]] = 0
        elif words[1] == 'spam':
            spam_count += 1
            if words[i] in vocab_spam:
                vocab_spam[words[i]] += float(words[i + 1])
            else:
                vocab_spam[words[i]] = float(words[i + 1])
            if words[i] not in vocab_ham:
                vocab_ham[words[i]] = 0

        if words[i] in vocab:
            vocab[words[i]] += float(words[i + 1])
        else:
            vocab[words[i]] = float(words[i + 1])

# calculate P(spam), P(ham)

p_spam = 1.0*spam_count/(spam_count + ham_count)
p_ham = 1.0 - p_spam

print("P(spam): %f \n" % p_spam)

# calculate log likelihoods

n_spam = sum(vocab_spam.values())
n_ham = sum(vocab_ham.values())

vocabulary = len(vocab)

likelihood_spam = {}
likelihood_ham = {}
for x in vocab.keys():
    likelihood_spam[x] = float(vocab_spam[x] + 1)/float(n_spam + vocabulary)
    likelihood_ham[x] = float(vocab_ham[x] + 1) / float(n_ham + vocabulary)

# top 5 words in spam, ham

top_spam = sorted(likelihood_spam, key=likelihood_spam.get, reverse=True)[:5]
top_ham = sorted(likelihood_ham, key=likelihood_ham.get, reverse=True)[:5]

print('The most likely words given that a document is spam: ')
print(top_spam)
print('\n')
print('The most likely words given that a document is ham: ')
print(top_ham)

# test the classifier on test data

correct = 0.0
total = 0.0
test_file = open('test', 'r')
for line in test_file:
    words = re.split(" ", line)
    spam_log_prob = log(p_spam)
    ham_log_prob = log(p_ham)
    for i in range(2, len(words), 2):
        if words[i] in likelihood_spam:
            spam_log_prob += float(words[i + 1]) * log(likelihood_spam[words[i]])
        if words[i] in likelihood_ham:
            ham_log_prob += float(words[i + 1]) * log(likelihood_ham[words[i]])

    if words[1] == 'spam' and spam_log_prob > ham_log_prob:
        correct += 1
    if words[1] == 'ham' and ham_log_prob > spam_log_prob:
        correct += 1
    total += 1

accuracy = correct/total
print('\naccuracy on test data: %.3f' % accuracy)
