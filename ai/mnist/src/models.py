from tensorflow import keras
from tensorflow.keras import activations, initializers, layers


def start_seq():
    seq = keras.Sequential()
    seq.add(layers.Reshape(target_shape=[28, 28, 1], input_shape=(28, 28)))
    return seq


def make_original_like(conv_filters=16, conv_filter_shape=5, nodes=1024):
    seq = start_seq()

    seq.add(layers.Conv2D(
        conv_filters, conv_filter_shape,
        padding='same',
        kernel_initializer=initializers.TruncatedNormal(stddev=0.1)))
    seq.add(layers.MaxPooling2D((2, 2), (2, 2), padding='same'))
    seq.add(layers.Flatten())
    seq.add(layers.Dense(
        nodes,
        kernel_initializer=initializers.TruncatedNormal(),
        bias_initializer=initializers.zeros(),
        activation=activations.relu))

    return end_seq(seq)


def make_2_conv_layers():
    seq = start_seq()
    seq.add(layers.Conv2D(
        16, 3,
        padding='same',
        activation=activations.relu))
    seq.add(layers.MaxPooling2D((2, 2), padding='same'))
    seq.add(layers.Conv2D(
        16, 3,
        padding='same',
        activation=activations.relu))
    seq.add(layers.Flatten())

    seq.add(layers.Dense(1024, activation=activations.relu))

    return end_seq(seq)


def make_2_conv_2_dense():
    seq = start_seq()
    seq.add(layers.Conv2D(
        16, 3,
        padding='same',
        activation=activations.relu))
    seq.add(layers.MaxPooling2D((2, 2), padding='same'))
    seq.add(layers.Conv2D(
        16, 3,
        padding='same',
        activation=activations.relu))
    seq.add(layers.Flatten())

    seq.add(layers.Dense(1024, activation=activations.relu))
    seq.add(layers.Dense(1024, activation=activations.relu))

    return end_seq(seq)


def make_2_conv_2_dense_dropout(prob):
    seq = start_seq()
    seq.add(layers.Conv2D(
        16, 3,
        padding='same',
        activation=activations.relu))
    seq.add(layers.Dropout(prob))
    seq.add(layers.MaxPooling2D((2, 2), padding='same'))
    seq.add(layers.Conv2D(
        16, 3,
        padding='same',
        activation=activations.relu))
    seq.add(layers.Dropout(prob))
    seq.add(layers.Flatten())

    seq.add(layers.Dense(1024, activation=activations.relu))
    seq.add(layers.Dropout(prob))
    seq.add(layers.Dense(1024, activation=activations.relu))
    seq.add(layers.Dropout(prob))

    return end_seq(seq)


def make_big():
    seq = start_seq()

    seq.add(layers.Conv2D(
        16, 3,
        padding='same',
        activation=activations.relu))
    seq.add(layers.Conv2D(
        16, 3,
        padding='same',
        activation=activations.relu))
    seq.add(layers.MaxPooling2D((2, 2), padding='same'))
    seq.add(layers.Dropout(0.4))

    seq.add(layers.Conv2D(
        16, 3,
        padding='same',
        activation=activations.relu))
    seq.add(layers.Conv2D(
        16, 3,
        padding='same',
        activation=activations.relu))
    seq.add(layers.MaxPooling2D((2, 2), padding='same'))
    seq.add(layers.Dropout(0.4))

    seq.add(layers.Flatten())

    seq.add(layers.Dense(1024, activation=activations.relu))
    seq.add(layers.Dropout(0.4))
    seq.add(layers.Dense(1024, activation=activations.relu))
    seq.add(layers.Dropout(0.4))

    return end_seq(seq)


def end_seq(seq):
    seq.add(layers.Dense(
        10,
        kernel_initializer=initializers.zeros(),
        bias_initializer=initializers.zeros(),
        activation=activations.softmax))
    return seq


MODELS = {
    "original": lambda: make_original_like(16, 5, 1024),
    "3x3": lambda: make_original_like(16, 3, 1024),
    "7x7": lambda: make_original_like(16, 7, 1024),
    "32_filters_3x3": lambda: make_original_like(32, 3, 1024),
    "32_filters": lambda: make_original_like(32, 5, 1024),
    "32_filters_7x7": lambda: make_original_like(32, 7, 1024),
    "2_conv_layers": make_2_conv_layers,
    "2_conv_2_dense": make_2_conv_2_dense,
    "2_conv_2_dense_dropout": lambda: make_2_conv_2_dense_dropout(0.25),
    "2_conv_2_dense_dropout_v2": lambda: make_2_conv_2_dense_dropout(0.4),
    "2_conv_2_dense_dropout_v3": lambda: make_2_conv_2_dense_dropout(0.5),
    "big": make_big,
}
