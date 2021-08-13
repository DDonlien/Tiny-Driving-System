#include <Usb.h>
#include <PS4USB.h>
#include <PS4BT.h>
#include <usbhub.h>
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

USB Usb;

PS4USB PS4(&Usb);

//BTD Btd(&Usb);
//PS4BT PS4(&Btd, PAIR);

int Left_2;
int Right_2;
int X_axis;
int Y_axis;
int R_axis;   //初始化输入参数，X、Y、R由别的模块提供输入。

int pinPWM;
int A_ENA = 2;
int A_forward = 3;
int A_reverse = 4;
int B_ENA = 5;
int B_forward = 6;
int B_reverse = 7;
int C_ENA = 8;
int C_forward = 9;
int C_reverse = 10;
//初始化引脚与pwm输出脚

int A_spd;
int B_spd;
int C_spd;  //第一步计算用的三轮转速。

int A_pwm;
int B_pwm;
int C_pwm;  //三轮平移输出的PWM真值。
int R_pwm;  //转动的PWM真值。


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  for (pinPWM = 0; pinPWM <= 13; pinPWM++){
    pinMode (pinPWM, OUTPUT);
  }
  
  #if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); // Halt
  }
  Serial.print(F("\r\nPS4 USB Library Started"));
  //检测PS4手柄连接，返回连接状态。
}

void loop() {
  //开始读取Dual Shock 4输入数值。
  Usb.Task();
  if(PS4.getAnalogHat(LeftHatX) > 136 || PS4.getAnalogHat(LeftHatX) < 120){
    X_axis = PS4.getAnalogHat(LeftHatX)-127;
  }
  else{
    X_axis = 0;
  }
  //输入X轴值，中央16的范围为死区。
  
  if(PS4.getAnalogHat(LeftHatY) > 136 || PS4.getAnalogHat(LeftHatY) < 118){
    Y_axis = 128-PS4.getAnalogHat(LeftHatY);
  }
  else{
    Y_axis = 0;
  }
  //输入Y轴的值，中央16的范围为死区。

  Left_2 = PS4.getAnalogButton(L2);
  Right_2 = PS4.getAnalogButton(R2);
  R_axis = Right_2-Left_2;
  //输入R的值，无死区（按键的关系），范围-255~255.

  A_spd = -0.471404521*X_axis+0.471404521*Y_axis;
  B_spd = 0.643950551*X_axis+0.172546030*Y_axis;
  C_spd = -0.172546030*X_axis-0.643950551*Y_axis;
  //计算三轮的转速，此处返回整数真值。

  A_pwm = map(A_spd, -93, 93, -170, 170);
  B_pwm = map(B_spd, -93, 93, -170, 170);
  C_pwm = map(C_spd, -93, 93, -170, 170);
  //转换三轮转速为PWM比例，这里平移的速度只能占用三分之二的轮子转速。
  //这里第一个范围为-0.66666~0.66666乘以之前X、Y的范围。

  R_pwm = map(R_axis, -255, 255, -85, 85);

  A_pwm = A_pwm+R_pwm;
  B_pwm = B_pwm+R_pwm;
  C_pwm = C_pwm+R_pwm;  //计算出带转动分量的三轮转速真值。

  analogWrite(A_ENA, abs(A_pwm));
  if(A_pwm>=0){
    analogWrite(A_forward, 1);
    analogWrite(A_reverse, 0);
  }
  else{
    analogWrite(A_forward, 0);
    analogWrite(A_reverse, 1);
  }
  //输出A轮正反转。

  analogWrite(B_ENA, abs(B_pwm));
  if(B_pwm>=0){
    analogWrite(B_forward, 1);
    analogWrite(B_reverse, 0);
  }
  else{
    analogWrite(B_forward, 0);
    analogWrite(B_reverse, 1);
  }
  //输出B轮正反转。

  analogWrite(C_ENA, abs(C_pwm));
  if(C_pwm>=0){
    analogWrite(C_forward, 1);
    analogWrite(C_reverse, 0);
  }
  else{
    analogWrite(C_forward, 0);
    analogWrite(C_reverse, 1);
  }
  //输出C轮正反转。


  //返回三轮PWM转速到串口监视器监视。
  Serial.print("A轮转速：");
  Serial.print(A_spd);
  Serial.print("; ");
  Serial.print("B轮转速：");
  Serial.print(B_spd);
  Serial.print("; ");
  Serial.print("C轮转速：");
  Serial.print(C_spd);
  Serial.println("; ");   //监控三轮转速真值。


  Serial.print("A轮pwm：");
  Serial.print(A_pwm);
  Serial.print("; ");
  Serial.print("B轮pwm：");
  Serial.print(B_pwm);
  Serial.print("; ");
  Serial.print("C轮pwm：");
  Serial.print(C_pwm);
  Serial.println("; ");   //监控三轮转速PWM。
  
  Serial.print("X轴分量：");
  Serial.print(X_axis);
  Serial.print("; ");
  Serial.print("Y轴分量：");
  Serial.print(Y_axis);
  Serial.print("; ");
  Serial.print("R分量：");
  Serial.print(R_axis);
  Serial.println("; ");   //监控X、Y、R分量。 
  Serial.println();

  delay(200);
}