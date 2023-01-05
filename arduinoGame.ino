#include <MsTimer2.h>
#include <LedControl.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiEsp.h>
#include <SoftwareSerial.h>

#define AP_SSID "iotemb"
#define AP_PASS "iotemb00"
#define SERVER_NAME "192.168.0.69"
#define SERVER_PORT 5000
#define LOGID "HSY_ARD"
#define PASSWD "PASSWD"

#define WIFIRX 6 // 6:RX-->ESP8266 TX
#define WIFITX 7 // 7:TX -->ESP8266 RX
#define ARR_CNT 3
#define CMD_SIZE 50

// 매트릭스 연결
const int DIN = 12;
const int CS = 11;
const int CLK = 10;
LedControl lc = LedControl(DIN, CLK, CS, 1);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// 조이스틱
const int varXPin = A3;
const int pushPin = 3;
int ReadLx = 0;

// 내 초기 위치(마지막 행의 열 위치)
int startY = 3;

// 시작상태인지 체크
bool isStart = false;

// life 초깃값
int life = 3;

// 게임 시간 측정
String gameTime_s;
String gameTime_m;
int minute2, minute1, sec2, sec1; //각각 분과 초의 자릿수. 한 글자씩 출력
unsigned long oldTime = 0; //이전시간
unsigned long millisTime = 0; //현재시간

//leddown 딜레이 제어 변수
unsigned long led_old_time = 0;

// 와이파이 설정
WiFiEspClient client;
SoftwareSerial wifiSerial(WIFIRX, WIFITX);

//SQL 시리얼 통신
char sendId[10] = "HSY_SQL";


//전송용 전역변수
char *pArray[ARR_CNT] = {0};
int timeSec = 0; // 전송하는 초
char getSensorId[10];
char sendBuf[CMD_SIZE];

bool button = false;
long time = millis();

int col = 0;
int row = 0;

bool lightFlag = false; //lightInit 한번만

void setup()
{
  Serial.begin(115200); // DEBUG
  wifi_Setup();

  Serial.setTimeout(10);
  lc.shutdown(0, false); // lc 초기화
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

  lcd.init();      // lcd 초기화
  lcd.backlight(); // lcd 백라이트

  // 조이스틱 선언부분
  pinMode(varXPin, INPUT);
  pinMode(pushPin, INPUT_PULLUP);

  lc.setLed(0, 7, startY, true); //입력받은 열만 킴

  //led down 1초에 한번씩 실행하게 조절하는 변수
  led_old_time = millis();

  MsTimer2::set(100, ledFor);
}

void loop()
{
  if (client.available())
  {
    socketEvent();
  }

  // 게임 시작 부분
  if (isStart == true)
  {
    if (button == false) {
      MsTimer2::start();
      button = true;
    }
    
    if(lightFlag == false){
        lightInit();
        lightFlag = true;
    }
    
    gameTime();    
  

    // 죽었을 경우
    if (life == 0)
    {
      // 서버 전송
      // 이름, over -> 인식되면 등록

      //int -> str
      gameTime_s = String(sec2) + String(sec1);
      gameTime_m = String(minute2) + String(minute1);

      char s_buffer[gameTime_s.length() + 1];
      char m_buffer[gameTime_m.length() + 1];

      gameTime_s.toCharArray(s_buffer, gameTime_s.length() + 1);
      gameTime_m.toCharArray(m_buffer, gameTime_m.length() + 1);

      timeSec = atoi(m_buffer) * 60 + atoi(s_buffer);
      sprintf(sendBuf, "[%s]OVER@%d\r\n", sendId, timeSec);
      client.write(sendBuf, strlen(sendBuf));
      client.flush();

      // lc 초기화
      MsTimer2::stop();
      lc.shutdown(0, false);
         
      //led
      lcd.setCursor(0, 0);
      lcd.print("  Game Over..  ");
      lcd.setCursor(0, 1);
      lcd.print("   :(   ");
      
      //초기화
      lightFlag = false;
      button = false;
      isStart = false;
      life = 3;

      minute2 = 0;
      minute1 = 0;
      sec2 = 0;
      sec1 = 0;
      
      delay(5000); // 5초 딜레이 후 밑의 else 코드 진행 (* 표시한 부분)
    }
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("  Push Joystic  ");
    lcd.setCursor(0, 1);
    lcd.print("   game start!  ");
  }
}

void lightInit()
{
  // lcd 백라이트 초기화 함수
  lcd.init();      // I2C LCD를 초기화 합니다..
  lcd.backlight(); // I2C LCD의 백라이트를 켜줍니다.
}

void gameTime() //게임 생존 시간 측정 함수
{
  if (millis() - oldTime >= 1000) { //1초단위로 출력

    oldTime = millis();
    millisTime = millis() / 1000;
    sec1 = millisTime % 10; //1초

    if (sec1 == 0 ) {
      sec2 ++;
    }

    if (sec2 >= 6) {
      minute1++;
      sec2 = 0;
    }

    sprintf(sendBuf, "%d%d:%d%d", minute2, minute1, sec2, sec1);
    lcd.setCursor(0, 0);
    lcd.print(sendBuf);

    sprintf(sendBuf, "Life:%d", life);
    lcd.setCursor(0, 1);
    lcd.print(sendBuf);

  }
}

void ledFor()
{
  if (row == 0)
  {
    lc.setLed(0, row, col, true); // 첫번째 행 켜기
  }
  else if (row == 1)
  {
    lc.setLed(0, row, col, true); // 첫번째 행 켜기
  }
  else if (row == 6)
  { // 마지막줄에서 못피했는지 체크
    lc.setLed(0, row - 2, col, false); // 첫번째 행부터 끄기
    lc.setLed(0, row, col, true);      // 입력받은 열만 킴

    if (col == startY) // 6번째 행의 col위치와 7번째 행의 내 위치가 같으면 => 마주쳤으면
    {
      life -= 1;
      lc.setLed(0, row - 1, col, false); // 5행 끄기
      lc.setLed(0, row, col, false);     // 6행 끄기

      if (life == 0)
      {
        row = 10;
      }
    }
  }
  else
  {
    lc.setLed(0, row - 2, col, false); // 첫번째 행부터 끄기
    lc.setLed(0, row, col, true);      // 입력받은 열만 킴
  }

  if (row == 8)
  { // 마주치지 않고 끝까지 내려왔다면
    lc.setLed(0, 6, col, false); // 첫번째 행부터 끄기
    // delay(500);
    //  ****** 이 부분에서 뒤의 7행의 도트와 부딪히는 경우 고려
    if (col == startY) // 7번째 행의 col위치와 7번째 행의 내 위치가 같으면 => 마주쳤으면
    {
      life -= 1;
    }
    // ********
    lc.setLed(0, 7, col, false);
  }


  if (row > 7) {
    row = 0;
    col = random(8); // 0부터 7까지의 값 중 하나를 랜덤하게 열 값으로 받는다.
  } else {
    row ++;
  }
}

void move()
{
  lc.setRow(0, 7, B00000000); //첫번째 행부터 끄기
  lc.setLed(0, 7, startY, true); //입력받은 열만 킴
}

void socketEvent()
{
  //START, FINISH 값 서버와 소통하는 부분
  int i = 0;
  char *pToken;
  char *pArray[ARR_CNT] = {0};
  char recvBuf[CMD_SIZE] = {0};
  int len;

  sendBuf[0] = '\0';
  len = client.readBytesUntil('\n', recvBuf, CMD_SIZE);

  pToken = strtok(recvBuf, "[@]");
  while (pToken != NULL)
  {
    pArray[i] = pToken;
    if (++i >= ARR_CNT)
      break;
    pToken = strtok(NULL, "[@]");
  }

  if (!strcmp(pArray[1], "MOVE"))
  {
    startY = atoi(pArray[2]); // str -> int 변환 함수
    move();
  }
  else if (!strncmp(pArray[1], " New", 4)) // New Connected
  {
    return;
  }
  else if (!strncmp(pArray[1], " Alr", 4)) // Already logged
  {
    client.stop();
    server_Connect();
    return;
  }
  else if (!strcmp(pArray[1], "START"))
  {
    // GAME START
    isStart = true;
  }
  client.write(sendBuf, strlen(sendBuf));
  client.flush();
}


// 와이파이 함수
void wifi_Setup()
{
  wifiSerial.begin(19200);
  wifiSerial.setTimeout(10);
  wifi_Init();
  server_Connect();
}

void wifi_Init()
{
  do
  {
    WiFi.init(&wifiSerial);
    if (WiFi.status() == WL_NO_SHIELD) {}
    else
      break;
  } while (1);

  while (WiFi.begin(AP_SSID, AP_PASS) != WL_CONNECTED) {}
}

int server_Connect()
{
  if (client.connect(SERVER_NAME, SERVER_PORT))
  {
    client.print("[" LOGID ":" PASSWD "]");
  }
}
