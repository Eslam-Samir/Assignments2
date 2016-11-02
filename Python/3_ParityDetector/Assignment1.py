import numpy as np

# Functions Definitions
def Sigmoid(x):
    return (1./(1+np.exp(-x)))

def SigmoidDerivative(x):
    return (Sigmoid(x)*(1. - Sigmoid(x)))

def ForwardPropagation(data, w1, w2, b1, b2, linearOutputUnit = False):
    net1   = np.dot(data, w1) + b1
    z2     = Sigmoid(net1)
    net2   = np.dot(z2, w2) + b2
    
    if linearOutputUnit:
        output = net2   # linear output unit case 
    else:
        output = Sigmoid(net2) # non-linear output unit case
        
    return (net1, z2, net2, output)

def PredictOutput(data, w1, w2, b1, b2, linearOutputUnit = False):
    return ForwardPropagation(data, w1, w2, b1, b2, linearOutputUnit)[3]

def CalcError(target, output):
    return 0.5 * sum((target - output)**2)

# Gradient Descend
def TrainNeuralNetwork(data, target, w1, w2, b1, b2, learningRate = 1, numIterations = 10000, linearOutputUnit = False):
    net1, z2, net2, output = ForwardPropagation(data, w1, w2, b1, b2, linearOutputUnit)

    itr = 1
    while(itr <= numIterations):
        # Calculate delta Error
        # output layer
        if linearOutputUnit:
            delta2 = -(target - output)    # linear output unit case 
        else:
            delta2 = np.multiply(-(target - output), SigmoidDerivative(net2)) # non-linear output unit case
            
        dEdW2 = np.dot(z2.T, delta2)
        dEdB2 = np.sum(delta2)

        # hidden layer
        delta1 = np.dot(delta2, w2.T) * SigmoidDerivative(net1)
        dEdW1 = np.dot(data.T, delta1)
        dEdB1 = np.sum(delta1)

        # Update Weights
        w1 -= learningRate*dEdW1
        w2 -= learningRate*dEdW2

        # Update Bias
        b1 -= learningRate*dEdB1
        b2 -= learningRate*dEdB2

        # update output and error
        net1, z2, net2, output = ForwardPropagation(data, w1, w2, b1, b2, linearOutputUnit)
        error = CalcError(target, output)

        # display error and iteration number 
        if itr == numIterations:
            print "\n########## Final Iteration ##########"						
            print 'iteration %d: error = %f' %(itr, error)
            print "#####################################\n"	
        elif (itr == 1) or (itr % 10000 == 0):
            print 'iteration %d: error = %f' %(itr, error)
        itr += 1

    return error

################################################################################################################

# Dataset
train_data = np.array([[0,0,0],[0,0,1],[0,1,0],[0,1,1],[1,0,0],[1,0,1],[1,1,0],[1,1,1]])
target     = np.array([[0],[1],[1],[0],[1],[0],[0],[1]])

# Number of inputs
inputLayerSize = 3

# Number of output units
outputLayerSize = 1

################################################################################################################

BestError = float('+inf')
BestHiddenUnitsCount = 0
error = 0
output = [0 for i in range(0,9)]

for i in range(1,10):
	print '############################# TEST %d #############################' % i
	print '######################### %d Hidden Unit ##########################' % i
	print '##################################################################\n'
	
	# Number of hidden layer units
	hiddenLayerSize = i

	# Initialize weights with random values
	w1 = np.random.randn(inputLayerSize, hiddenLayerSize)    # dimensions: 3xn
	w2 = np.random.randn(hiddenLayerSize, outputLayerSize)   # dimensions: nx1 
	
	# Initialize bias with random values
	b1 = np.random.randn(1, hiddenLayerSize)   # dimensions: 1xn 
	b2 = np.random.randn(1, outputLayerSize)   # dimensions: 1x1 
	
	learningRate = 0.01
	numIterations = 100000

	# Train the model
	error = TrainNeuralNetwork(train_data, target, w1, w2, b1, b2, learningRate, numIterations)

	# Save the best result	
	if error < BestError:
		BestError = error
		BestHiddenUnitsCount = i
		output = PredictOutput(train_data, w1, w2, b1, b2)
	
print "Best Error is", BestError, "When", BestHiddenUnitsCount, " Hidden Units are Used"

################################################################################################################

print '\n##################################################################'
print '####################### TEST the Best Model #######################'
print '###################### Using %d Hidden Units #######################' % BestHiddenUnitsCount
print '##################################################################\n'

print "Inputs:\n"
print train_data
print "\nPredicted Outputs Using Non-linear Output Unit:\n"
print output

################################################################################################################

print '\n##################################################################'
print '########## Using Linear Output Unit and %d Hidden units ###########' % BestHiddenUnitsCount
print '##################################################################\n'

# Initialize weights with random values
w1 = np.random.randn(inputLayerSize, BestHiddenUnitsCount)    # dimensions: 3xn
w2 = np.random.randn(BestHiddenUnitsCount, outputLayerSize)   # dimensions: nx1 

# Initialize bias with random values
b1 = np.random.randn(1, BestHiddenUnitsCount)   # dimensions: 1xn 
b2 = np.random.randn(1, outputLayerSize)   # dimensions: 1x1 

learningRate = 0.01
numIterations = 100000

# Train the model
error = TrainNeuralNetwork(train_data, target, w1, w2, b1, b2, learningRate, numIterations, True)
print "Error for using linear output unit:", error

output = PredictOutput(train_data, w1, w2, b1, b2, True)
print "\nPredicted Outputs Using Linear Output Unit:\n"
print output
