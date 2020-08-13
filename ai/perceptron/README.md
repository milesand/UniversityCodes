# 인공지능 과제 #1 Perceptron 구현

2017920051 이제우

## 설명
cwd가 이 파일이 있는 폴더인 상태에서, 다음과 같이 빌드한다:

    g++ -I . src/main.cpp

* input 차원이 N인 1-layer perceptron 구현은 include/perceptron.hpp에 포함되어있다.
* 사용된 Activation function은 Binary step이다.
* src/main.cpp에는 다음 내용이 구현되어있다:
  * Perceptron의 threshold 및 weight 값들을 [-1.0, 1.0] 구간의 랜덤 값으로 초기화.
  * {0.0, 1.0}²를 입력으로 해 AND 게이트의 출력과 비교한다.
  * 틀린 것이 있었을 경우, stdin으로부터 3개의 값을 읽어 threshold 및 weight 값을 재설정한다.

## "어떤 값을 입력해야 무한 loop에서 빨리 나올 수 있는가?"

무한 루프에서 빨리 나오기 위해서는 (명백하게도) 맞는 값을 입력해야 한다. 좀 더 구체적으로,
해당 Perceptron이 AND 게이트와 같이 동작하게 하는 threshold 및 weight 값을 입력해야한다.
즉, threshold를 t, 두 weight를 각각 w1, w2로 표기했을 때

* -t < 0
* -t + w1 < 0
* -t + w2 < 0
* -t + w1 + w2 >= 0

를 만족하는 값이어야 한다. 다시 말해:

* t, w1, w2는 모두 양수여야 한다.
* w1과 w2는 모두 t보다 작아야 하며, 그 합은 t보다 크거나 같아야 한다.

이를 만족하는 (t, w1, w2)의 예시로 (0.7, 0.5, 0.5), (0.5, 0.499, 0.002) 등이 있다.