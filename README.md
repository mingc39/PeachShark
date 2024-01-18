# Peach Shark

Peach Shark는 Voicemeeter Banana 용 애드온입니다.

Voicemeeter remote API를 활용하여 멀티채널 사운드카드 하나에 2채널 스피커 여러개를 연결할 수 있도록 도와줍니다.

![image1](https://raw.githubusercontent.com/mingc39/PeachShark/main/image1.png)

Peach Shark는 보이스미터의 여러 2채널 사운드 출력을 하나의 8채널(또는 4, 6 채널) 사운드로 변환해줍니다.

# 사용 전 준비사항

Voicemeeter가 반드시 필요합니다.

Voicemeeter Banana는 Voicemeeter 웹페이지에서 찾을 수 있습니다.

https://vb-audio.com/Voicemeeter/banana.htm

Voicemeeter, Voicemeeter Banana는 Peach Shark를 사용해주세요.

Voicemeeter Potato는 Peach Shark Pro를 사용해주세요.

단, Voicemeeter에서 Peach Shark 사용 시 Voicemeeter Banana 용 기능은 작동하지 않으며 오디오 버스 이름이 Voicemeeter Banana 기준으로 표시됩니다.

사운드카드를 멀티채널(4~7.1)로 설정하고 스피커를 연결합니다.

# 사용 방법

왼쪽 ch0,1, ch2,3 ... 은 실제 출력에 사용할 채널들 입니다.

ch0,1 ~ ch6,7은 A1의 채널입니다.

위쪽 X, A1, A2, A3, B1, B2는 원래 채널들 입니다.

여기서 X는 Voicemeeter에서 나오는 출력을 별도로 지정하지 않고 Voicemeeter상에서 원래 출력되어야할 채널을 선택함을 의미합니다.

2개의 라디오 버튼 중 왼쪽은 좌우 변경 없이, 오른쪽은 좌우 변경을 의미합니다.

파일 메뉴에서 설정을 저장/불러오기 할 수 있습니다. 저장/불러오기 위치는 프로그램이 저장된 폴더의 peach.cfg 파일입니다.

파일 메뉴에서 트레이에서 시작을 체크할 경우 프로그램 실행시 시스템 트레이에서 실행되고 별도의 창이 뜨지 않습니다.

최소화시 시스템 트레이로 최소화되며 작업표시줄에는 표시되지 않습니다.

컴퓨터 시작시 자동으로 실행되도록 하려면 Peach Shark를 시작프로그램으로 등록하세요.

시작프로그램으로 실행시 Voicemeeter보다 먼저 켜져 정상 작동하지 않는 경우에는 지연 시작 기능을 사용하세요.

파일 메뉴에서 지연 시작 시간 선택 후 -delay 인자를 추가하여 실행 시 해당 시간동안 대기 후 PeachShark를 시작합니다.

# 사용 예시

![image2](https://raw.githubusercontent.com/mingc39/PeachShark/main/image2.png)

위의 이미지에서 A2 ~ B2는 X가 선택되어 있으므로 Voicemeeter에서 출력하는 그대로 출력됩니다.

A1은 채널별로 설정에 따라 다른 채널의 사운드가 출력됩니다.

A1의 0 ~ 1 채널은 기존의 A1의 0 ~ 1 채널이 좌우 변경 없이 그대로 출력됩니다. (A1ch.0->A1ch.0, A1ch.1->A1ch.1)

A1의 2 ~ 3 채널은 기존의 A2의 0 ~ 1 채널이 좌우 변경 되어 출력됩니다. (A2ch.1->A1ch.2, A2ch.0->A1ch.3)

A1의 4 ~ 5 채널은 기존의 A1의 0 ~ 1 채널이 좌우 변경 없이 그대로 출력됩니다. (A2ch.0->A1ch.4, A1ch.2->A1ch.5)

A1의 6 ~ 7 채널은 기존의 A1의 0 ~ 1 채널이 좌우 변경 없이 그대로 출력됩니다. (A3ch.0->A1ch.6, A1ch.1->A1ch.7)

# 주의사항

Voicemeeter, Voicemeeter Banana: PeachShark 사용

Voicemeeter Potato: PeachShark Pro 사용

PeachShark와 PeachShark Pro를 반대로 사용할 경우 정상작동하지 않을 수 있습니다.

Voicemeeter에서 PeachShark 사용 시 PeachShark의 A1과 A2가 각각 Voicemeeter의 A와 B입니다. PeachShark의 A3, B1, B2는 사용되지 않습니다.

32비트에서는 정상작동하지 않을 수 있습니다.

프로그램 아이콘은 mrlimeshark가 그려주었습니다!

![icon](https://raw.githubusercontent.com/mingc39/PeachShark/main/VoiceMeeterAddon/image.ico)
