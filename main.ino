#include <dht11.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  //引用I2C库

//设置LCD1602设备地址，这里的地址是0x3F，一般是0x20，或者0x27，具体看模块手册
LiquidCrystal_I2C lcd(0x27, 16, 2);
dht11 dht;

// 将 DHT 引脚指定为 8
#define DHT_PIN 8

// HC-SR04 超声波模块测距
#define TRIG_PIN 2  //引脚 Tring 连接 IO D2
#define ECHO_PIN 3  //引脚 Echo 连接 IO D3

bool tag = 0;
const int NUM_READINGS = 10;  //平均距离读数
const int SOUND_SPEED = 343;  //声速（单位：m/s）
const int MAX_LEN = 40;

float len() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  float pulse_duration = pulseIn(ECHO_PIN, HIGH);
  float cm = pulse_duration * 16.3 / 1000 - 1;  //将回波时间转换为厘米单位
  return cm;
}

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  lcd.init();  // 初始化LCD
  lcd.noBacklight();
}

void loop() {
  float distanceSum = 0;  // 初始化距离累加和
  for (int i = 0; i < NUM_READINGS; i++) {
    distanceSum += len();  // 将距离累加到距离累加和中
  }
  float average_distance = distanceSum / NUM_READINGS;  // 计算平均距离
  if (average_distance < MAX_LEN) {
    tag = true;  // 标记需要读取 DHT11 数据并显示到 LCD 上
  } else {
    if (tag == true) {  // 如果之前已经读取过数据且距离超过了阈值
      delay(5000);
      if (len() > MAX_LEN) {
        lcd.noBacklight();  // 关闭 LCD 背光
        tag = false;        // 取消标记
      }
    } else {
      delay(1000);
    }
  }

  if (tag) {
    int check_result = dht.read(DHT_PIN);
    lcd.setCursor(0, 0);  //设置显示指针
    lcd.print("Temp:");   //输出字符到LCD1602上
    lcd.print((float)dht.temperature, 2);
    lcd.setCursor(0, 1);
    lcd.print("Hum:");  //输出字符到LCD1602上
    lcd.print((float)dht.humidity, 2);
    lcd.print('%');
    lcd.backlight();
    delay(1000);
  }
}
