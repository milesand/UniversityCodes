# 인공지능 과제 #2 Perceptron learning 구현

2017920051 이제우

## 설명
cwd가 이 파일이 있는 폴더인 상태에서, 다음과 같이 빌드한다:

    g++ -I . src/main.cpp

다음과 같이 실행한다(실행 파일이 ./a.out일 떄:):

    ./a.out < [TRAINING_DATA] > [OUTPUT_PATH]

* input 차원이 N인 1-layer perceptron 구현은 include/perceptron.hpp에 포함되어있다.
  * forward: `StaticPerceptron<N>::run`
  * backward: `StaticPerceptron<N>::learn_step`
* Activation function은 Binary step이다.
* Learning rate의 값은 0.01이다.
* src/main.cpp에는 다음 내용이 구현되어있다:
  * Perceptron의 threshold 및 weight 값들을 [-1.0, 1.0] 구간의 랜덤 값으로 초기화.
  * stdin으로부터 훈련 데이터를 읽는다.
  * 각 Iteration 마다:
    * 훈련 데이터를 입력해 틀린 결과의 수를 센다.
    * 현재 Perceptron의 threshold 및 weight 값, 그리고 틀린 결과의 수를 csv 형식으로 stdout에 출력한다.
    * 틀린 결과가 없었다면, 종료한다.
    * Perceptron을 훈련 데이터를 이용해 1회 학습시킨다.
* training_data 디렉토리에는 프로그램의 입력으로 쓸 수 있는 훈련 데이터가 있다.
* output 디렉토리에는 프로그램의 가능한 출력과 그 시각화 이미지가 저장되어있다.
  * xor의 경우, 단일 퍼셉트론으로 계산할 수 없음이 알려져있다. output 디렉토리의 xor.csv의 내용은
    첫 50줄만을 기록한 것이다.

## Visualization

데이터의 시각화는 `visualize.py`를 통해 실행한다. `Python 3`, `numpy`, `matplotlib`이 설치된 상태에서,
다음과 같이 실행한다:

    python visualize.py [TRAINING_DATA] [FILE]

이후 `[FILE]`의 위치에 이름이 같은 디렉토리가 생성되며, 다음을 포함한다:
* `errors.png`: Iteration에 따른 오차값 (틀린 결과 수) 그래프.
* `0.png`, ... : 해당 Iteration의 퍼셉트론 상태의 시각화.