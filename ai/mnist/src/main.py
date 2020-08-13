from itertools import count
import os
import numpy as np
import tensorflow as tf
from tensorflow.keras import datasets, losses, optimizers, metrics

from models import MODELS

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'

np.random.seed(20160703)
tf.random.set_seed(20160703)

MNIST_TRAIN_COUNT = 60000
BATCH_SIZE = 40
BATCHES_PER_EPOCH = MNIST_TRAIN_COUNT // BATCH_SIZE
MODEL_ID = 'big'


def mnist_dataset():
    (x_train, y_train), (x_test, y_test) = datasets.mnist.load_data()
    x_train = x_train / np.float32(255)
    x_test = x_test / np.float32(255)
    train_dataset = tf.data.Dataset.from_tensor_slices((x_train, y_train))
    return (train_dataset, (x_test, y_test))


train_data, (test_xs, test_ts) = mnist_dataset()
train_data = train_data.shuffle(MNIST_TRAIN_COUNT).batch(BATCH_SIZE).repeat()
train_data_iter = iter(train_data)


model = MODELS[MODEL_ID]()

optimizer = optimizers.Adam(learning_rate=0.0005)
compute_loss = losses.SparseCategoricalCrossentropy()
compute_accuracy = metrics.SparseCategoricalAccuracy()

for epoch in count(1):
    for _ in range(BATCHES_PER_EPOCH):
        batch_xs, batch_ts = next(train_data_iter)
        with tf.GradientTape() as tape:
            outputs = model(batch_xs, training=True)
            loss = compute_loss(batch_ts, outputs)
        grads = tape.gradient(loss, model.trainable_variables)
        optimizer.apply_gradients(zip(grads, model.trainable_variables))

    outputs = model(test_xs, training=False)
    loss = compute_loss(test_ts, outputs)
    accuracy = compute_accuracy(test_ts, outputs)
    print("Epoch: {}, Loss: {:.6f}, Accuracy: {:.6f}"
          .format(epoch, loss * 10000, accuracy))
    if accuracy >= 0.99 or epoch == 20:
        model.save('models/{}-{}'.format(MODEL_ID, epoch))
        break
