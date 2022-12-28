#include <SoftwareSerial.h>
#include <Wire.h>
#include <MsTimer2.h>

#define CMD_SIZE 60

char sendBuf[CMD_SIZE];
char sendId[10] = "ARD_NAME"; 

//조이스틱 
int startY = 3;

const int varXPin = A3;
const int pushPin = 3;
int ReadLx = 0;

//시작상태인지 체크
bool isStart = false;

//시간 측정 변수
unsigned long cur_time;
unsigned long old_time = millis()/1000;

SoftwareSerial BTSerial(7,6);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println("hello");
  
  BTSerial.begin(9600);

  // 조이스틱 선언부분
  pinMode(varXPin, INPUT); 
  pinMode(pushPin, INPUT_PULLUP);

  //타이머 설정
  MsTimer2::set(100, move); //1초 주기로 move() 실행
  MsTimer2::start ();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (BTSerial.available()) {       
    Serial.write(BTSerial.read());
  }
  if (Serial.available()) {         
    BTSerial.write(Serial.read());
  }

  //게임 스타트 부분
  int push = digitalRead(pushPin);
  cur_time = millis()%60;

  if (push == 0){ // 3초 누르면 게임시작
    if(fabs(cur_time - old_time)>=3){
      Serial.println (fabs(cur_time - old_time));
      
      isStart=true;
      Serial.println("게임 시작");

      //3초 눌러서 게임 시작했단걸 전송
      sprintf(sendBuf, "[%s]START@%d\r\n", sendId, isStart);
      BTSerial.write(sendBuf, strlen(sendBuf));

      Serial.print ("3sec click:");
      Serial.println (isStart);
    }
  }
 }

void move() {
    //움직이는 기능 -> timer를 통해 1초에 한번씩 움직임을 확인하고 그 시간동안은 ledDown이 잠시 멈추도록 구현??
    ReadLx = analogRead(varXPin); //x축으로 이동한 값 읽기
    //Serial.println(ReadLx);
    int mapLx = map(ReadLx, 0, 1023, -512, 512); //범위조정
    
    //오른쪽으로 한 칸 이동
    if(mapLx < -300) //이동한 정도가 너무 예민하지 않도록 조절하기
    {
      if(startY < 7){ //양쪽 모서리가 아닌지 확인 
        //lc.setLed(0,7,startY,false); //첫번째 행부터 끄기
        startY += 1;
        //lc.setLed(0,7,startY,true); //입력받은 열만 킴
        //오른쪽 한 칸 이동한거 정보 보내기
        sprintf(sendBuf, "[%s]MOVE@%d\r\n", sendId, startY);
        BTSerial.write(sendBuf, strlen(sendBuf));
      }
    }
     
    //왼쪽으로 한 칸 이동
    if(mapLx > 300)
    {
      if(startY > 0){ //양쪽 모서리가 아닌지 확인 
        //lc.setLed(0,7,startY,false); //첫번째 행부터 끄기
        startY -= 1;
        //lc.setLed(0,7,startY,true); //입력받은 열만 킴
        //왼쪽 한 칸 이동한거 정보 보내기
        sprintf(sendBuf, "[%s]MOVE@%d\r\n", sendId, startY);
        BTSerial.write(sendBuf, strlen(sendBuf));
        
      }
    }
}
