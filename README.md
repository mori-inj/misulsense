# misulsense

* 관객에게 스위치가 주어지고, 스위치를 누른후 관객 스스로 느끼기에 1초가 지났다고 생각하는 시점에 스위치를 뗀다. 떼는 순간 TV화면의 디지털 시계의 1초는 관객의 1초만큼의 속도로 흐른다.
* Arduino, winapi 사용
* 이미지  
  <p align="center">
    <img src="https://raw.githubusercontent.com/mori-inj/mori-inj.github.com/master/your_time.jpeg" width="350"/>
    <img src="https://raw.githubusercontent.com/mori-inj/mori-inj.github.com/master/nametag.jpeg" width="350"/>
  </p>
* 아두이노와 연결된 스위치가 chatterless switch가 아니므로 특정 시간간격보다 짧은 변화는 무시하는 C코드를 아두이노 위에 돌린다.
* PC(winapi)는 스레드 두 개를 돌린다.  
  하나는 COM port를 통해 시리얼 통신으로 아두이노가 스위치가 눌려있던 시간을 보내기를 blocking 방식으로 기다리고
  다른 하나는 화면에 출력되는 시계를 현재 설정된 시간 간격마다 갱신한다.
* 아두이노와 컴퓨터가 통신하는 코드는 다른 사람의 코드를 참고했다.
