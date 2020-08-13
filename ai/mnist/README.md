# MNIST 성능 향상 - 2017920051 이제우

본 과제에는 다음과 같은 프로그램 및 라이브러리가 사용되었다:

* Python 3.7.0
* tensorflow 2.1.0-dev20191201
* numpy 1.17.2


## Tensorflow migration

교재에 제시된 코드는 Tensorflow 1을 기반으로 작성되었으며, 이에 따라 Tensorflow 2에서는 동작하지 않는다.
그러므로 코드를 Tensorflow 2에 맞추어 재작성할 필요가 있었다. 예를 들어, 제시된 모델은 Tensorflow 2에서는
다음과 같이 표현된다:

    model = keras.Sequential([
        layers.Reshape(target_shape=[28, 28, 1], input_shape=(28, 28)),
        layers.Conv2D(
            16, 5,
            padding='same',
            kernel_initializer=initializers.TruncatedNormal(stddev=0.1)),
        layers.MaxPooling2D((2, 2), (2, 2), padding='same')
        layers.Flatten()
        layers.Dense(
            1024,
            kernel_initializer=initializers.TruncatedNormal(),
            bias_initializer=initializers.zeros(),
            activation=activations.relu)])

단순히 Tensorflow의 버전을 전환하는 것 외에도, 몇가지 수정이 가해졌다.

* 교재에서는 Batch 크기가 고정되어있으며 임의로 정한 양의 Batch를 처리할 때마다 테스트를 수행하나, 일부 큰 모델을
  큰 Batch 크기로 사용할 경우 메모리 등의 부족으로 인한 예외가 발생하는 것을 확인할 수 있었다. 이에 따라 Batch
  크기에 독립적인 테스트 수행을 위해 테스트를 한 Epoch마다 수행하는 것으로 했다.

* 교재와 최대한 같은 조건을 사용할 경우에도 다른 결과가 나오는 것을 확인할 수 있었다. 예를 들어, 교재에 예시로
  제안된 모델을 4000 step 동안 훈련시킬 경우 교재에서는 약 98%의 정확도를 달성하지만, 재작성된 코드는 약 97.5%의
  정확도를 달성했다. 이에 더해 테스트간 간격이 한 Epoch로 길어진 것, 모델을 오랫동안 훈련할 경우 정확도가 상승할
  가능성등을 고려하여, 임의로 제한을 20 Epoch로 설정했다.


## 결과

각 모델의 훈련 과정 경과 및 훈련 종료 후 모델은 `outputs` 디렉토리와 `models` 디렉토리에 저장되어있다.

### 기존 모델 (`original`)

기존 모델을 훈련시킨 결과 98.50%의 정확도를 달성할 수 있었다.

훈련 경과의 16 ~ 20 Epoch 구간은 다음과 같다:

    Epoch: 16, Loss: 783.317383, Accuracy: 0.985019
    Epoch: 17, Loss: 760.780273, Accuracy: 0.985076
    Epoch: 18, Loss: 760.642212, Accuracy: 0.985072
    Epoch: 19, Loss: 996.706116, Accuracy: 0.984953
    Epoch: 20, Loss: 830.893311, Accuracy: 0.985040

17 ~ 19 Epoch 구간은 정확도가 오히려 감소하는 것을 확인할 수 있으며, 이는 Overfitting의 결과라고 볼 수 있다.


### `original`의 필터 크기를 3x3으로 교체 (`3x3`)

98.39%의 정확도를 달성했다. `original` 모델괴 비교할 경우, 모든 Epoch에서 정확도가 더 낮게 나타나는 것을 확인할 수 있다.
필터의 크기를 줄였을 때 정확도가 낮아졌으므로, 반대로 필터의 크기를 늘렸을 때 정확도가 높아질 것으로 기대할 수 있다.


### `original`의 필터의 크기를 7x7로 교체 (`7x7`)

98.52%의 정확도를 달성했다. `3x3`에서의 기대와는 달리, 필터의 크기를 늘렸음에도 정확도의 상승은 그리 크지 않으며, 일부 Epoch에서는
오히려 `original`보다 정확도가 낮게 나타기도 한다. 이는 이런 구조의 모델을 이용해 MNIST 데이터셋을 분석할 경우 필터의 크기는 5x5가
적합하다는 의미로 해석할 수 있을 것 같다.

흥미롭게도, `original`과는 달리 정확도가 감소하는 구간이 존재하지 않는다. 즉, 정확도만을 가지고 볼 경우 모델의 복잡도가 증가했음에도
Overfitting은 오히려 적게 일어났다는 것으로 볼 수 있다.


### `original`의 필터의 갯수를 32개로 증가 (`32_filters`)

98.54%의 정확도를 달성했다.


### `original`의 필터의 갯수를 32개로 증가, 필터의 크기를 7x7로 교체 (`32_filters_7x7`)

98.46%의 정확도를 달성했다.


### `original`에 Convolution 및 Pooling layer 추가 (`2_conv_layers`)

98.86%의 정확도를 달성했다. 레이어의 추가를 통해 다른 변경을 통해 얻을 수 없었던 수준의 정확도 증가를 얻을 수 있었다.


### `2_conv_layers`에 Dense layer 추가 (`2_conv_2_dense`)

98.83%의 정확도를 달성했다. `2_conv_layers`에 레이어를 추가함으로 유사한 성능 향상을 얻을 수 있을 것으로 기대했으나,
실제로는 정확도가 약간 감소했거나 큰 차이가 없는 것으로 나타났다.


### `2_conv_2_dense`에 Dropout layer 다수 추가 (`2_conv_2_dense_dropout`)

각 Convolution layer 및 Computation layer 직후에 Training시 25%의 출력을 Drop하는 Dropout 레이어를 추가했으며,
98.97%의 정확도를 달성했다. 이것은 Overfitting의 감소로 인해 테스트 데이터에 대한 정확도가 증가한 것이라고 볼 수 있다.


### `2_conv_2_dense_dropout`의 Dropout 비율을 40%로 증가 (`2_conv_2_dense_dropout_v2`)

18 Epoch에 99.00%의 정확도를 달성했다. `2_conv_2_dense_dropout`과 비교했을 떄 Epoch 10까지의 정확도는 낮게 나타나지만
이후의 정확도는 높게 나타난다. 이는 높은 Dropout 비율로 인해 학습이 약간 느리지만, Overfitting으로 인한 정확도 감소의 영향을
덜 받았기 때문이라고 볼 수 있다.


### `2_conv_2_dense_dropout`의 Dropout 비율을 50%로 증가 (`2_conv_2_dense_dropout_v3`)

20 Epoch에 99.00%의 정확도를 달성했다. 이는 Dropout 비율의 상승으로 인해 `2_conv_2_dense_dropout_v2`에 비해
느리게 학습한 결과로 볼 수 있을 것이다.


### `2_conv_2_dense_dropout_v2`에 Convolution layer 추가 (`big`)

현재까지 가장 높은 정확도 상승을 보여준 변경은 `original` -> `2_conv_layers`에서 Convolution layer를 추가한 것으로,
이로부터 Convolution layer를 더 추가하면 더 높은 정확도를 더 빠르게 얻을 수 있을 것을 기대할 수 있다.

이를 확인하기 위해 `2_conv_2_dense_dropout_v2`의 각 Pooling layer 전에 하나씩, 총 2개의 Convolution layer를 추가했으며,
그 결과 13 Epoch만에 99.01%의 정확도를 달성할 수 있었다.