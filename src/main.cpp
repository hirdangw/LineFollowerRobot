/*
 * AUTHOR: NGUYEN TIEN HAI DANG PH18448
 * K17.1.1  FPT POLYTECHNIC 
 * MAJOR IN AUTOMATION 
 * FRAMEWORK: ARDUINO
 * EDITOR: VISUAL STUDIO CODE 
 */
 //
#include <Arduino.h>
unsigned int sp = 95;                   // khởi tạo biến giá trị tốc độ xe
#define signalPin 16
#define TX 13 // gán chân TX là chân số 13
#define RX 12 // gán chân RX là chân số 12
#define s1 11 // gán chân cảm biến thứ nhất là chân số 11
#define s2 10 // gán chân cảm biến thứ hai là chân số 10
#define s3 9  // gán chân cảm biến thứ ba là chân số 9
#define s4 8  // gán chân cảm biến thứ tư là chân số 8
#define s5 7  // gán chân cảm biến thứ năm là chân số 7
#define s6 14
#define s7 15
#define enA 6 // gán chân enA là chân số 6
#define enB 5 // gán chân enB là chân số 5
#define in4 A7// gán chân in4 vào chân A3
#define in3 A6// gán chân in3 vào chân A2
#define in2 A5// gán chân in2 vào chân A1
#define in1 A4// gán chân in1 vào chân A0
unsigned int counting =0;
int hic = 0;
boolean statics = false;
int current_position = 35;
boolean last_right = false, last_left = false;
const int trig = 2;
const int echo = 3;
unsigned long duration;
int distance;
#include<Servo.h>
Servo ser;
#include<SoftwareSerial.h>              // add thư viện SoftwareSerial
SoftwareSerial hi(RX, TX);              // khởi tạo kết nối nối tiếp
#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
int sensor[5] = {0, 0, 0, 0, 0};        // khai báo mảng giá trị đọc cảm biến
unsigned int spA = sp;                  // tốc độ bánh xe bên trái
unsigned int spB = sp;              // tốc độ bánh xe bên phải
char dk = 0;                            // biến dk để gán giá trị điều khiển nhận từ bluetooth

void hienthi()
{
  lcd.setCursor(1,0); lcd.print(digitalRead(s1));lcd.setCursor(4,0); lcd.print(digitalRead(s2));lcd.setCursor(7,0); lcd.print(digitalRead(s3));
  lcd.setCursor(10,0); lcd.print(digitalRead(s4));lcd.setCursor(13,0); lcd.print(digitalRead(s5));
}
void ht_read()
{
  lcd.setCursor(1,1); lcd.print(sensor[0]);lcd.setCursor(4,1); lcd.print(sensor[1]);lcd.setCursor(7,1); lcd.print(sensor[2]);
  lcd.setCursor(10,1); lcd.print(digitalRead(s4));lcd.setCursor(13,1); lcd.print(digitalRead(s5));
}

void pause(int spa, int spb)
{ //xe dừng
  analogWrite(enA, 0); analogWrite(enB, 0); // băm xung ra 2 bên với lần lượt 2 tham số
  digitalWrite(in1,0); digitalWrite(in2,0);                    // dừng động cơ cả hai bên
  digitalWrite(in3,0); digitalWrite(in4,0);
}//pause
void forward(int spa, int spb)
{ // xe tiến thẳng
  analogWrite(enA, spa); analogWrite(enB, spb); // thiết lập tốc độ băm xung
  digitalWrite(in1,1); digitalWrite(in2,0);                          // cho motor bên trái quay thuận
  digitalWrite(in3,1); digitalWrite(in4,0);                          // cho motor bên phải quay thuận
}//forward
void backward(int spa, int spb)
{ //xe lùi
  analogWrite(enA, spa); analogWrite(enB, spb); // thiết lập tốc độ băm xung
  digitalWrite(in1,0); digitalWrite(in2,1);                          // cho motor bên trái quay ngược
  digitalWrite(in3,0); digitalWrite(in4,1);                          // cho motor bên trái quay ngược
}
void left(int spa, int spb)
{ //xe quay trái, 2 bên quay ngược chiều nhau
  analogWrite(enA, spa); analogWrite(enB, spb); // thiết lập tốc độ băm xung
  digitalWrite(in1,0); digitalWrite(in2,1);                          // cho motor bên trái quay ngược
  digitalWrite(in3,1); digitalWrite(in4,0);                          // cho motor bên phải quay thuận
  last_left = true;
  last_right = false;
}//left
void right(int spa, int spb)
{ //xe quay phải và 2 bên quay ngược chiều nhau
  analogWrite(enA, spa ); analogWrite(enB, spb); // thiết lập tốc độ băm xung
  digitalWrite(in1,1); digitalWrite(in2,0);                           // cho motor bên phải quay ngược
  digitalWrite(in3,0); digitalWrite(in4,1);                           // cho motor bên trái quay thuận
  last_right = true;
  last_left = false;
  
}//right
void half_right(int spa, int spb)
{ //xe quay phải và chỉ bên trái quay
  analogWrite(enA, spa); analogWrite(enB, spb); // thiết lập tốc độ băm xung
  digitalWrite(in1,1); digitalWrite(in2,0);                          // cho motor bên phải quay thuận
  digitalWrite(in3,0); digitalWrite(in4,0);                           // dừng động cơ bên phải dừng
  last_right = true;
  last_left = false;
}
void half_left(int spa, int spb)
{ //xe quay trái và chỉ bánh bên phải quay
  analogWrite(enB, spb); 
  digitalWrite(in1,0); digitalWrite(in2,0);                           
  digitalWrite(in3,1); digitalWrite(in4,0);                     
  last_left = true;
  last_right = false;
}
void read_sensor_values()
{ //đọc 5 giá trị cảm biến và gán vào mảng lưu giá trị
  sensor[0] = digitalRead(s1);
  sensor[1] = digitalRead(s2);
  sensor[2] = digitalRead(s3);
  sensor[3] = digitalRead(s4);
  sensor[4] = digitalRead(s5);
}
void Delay(float delayTime, void (func)())
{ // hàm trễ không bị delay
  unsigned long endTime = millis() + delayTime;
  while (millis() < endTime)
  {
    func();
  }
}
int get_distance()
{ 
  digitalWrite(trig, 0);
  delayMicroseconds(2);
  digitalWrite(trig, 1);
  delayMicroseconds(10);
  digitalWrite(trig, 0);
  //Chân echo nhận xung phản xạ lại
  //Và đo độ rộng xung cao ở chân echo
  duration = pulseIn (echo, HIGH);
  distance = int (duration / 2 / 29.412);
  //In lên Serial kết quả
//  Serial.print("Khoảng cách: ");
//  Serial.print(distance);
//  Serial.println("cm   ");
//  lcd.setCursor(0,1); lcd.print(distance); lcd.print("    ");
  return distance; 
}
void auto_run()
{
  oops:
  read_sensor_values();
  oops2:
  get_distance();
  if (distance >=5 && distance <= 17)
  { hic++;
    lcd.setCursor(11,1); lcd.print("hic " + String(hic));
    counting +=1;
    if ( counting == 1)
    {
      // left(90,90); delay(80);
      while ( digitalRead(s5) == 1)
      {
        left(90,90);
      }
      pause(0,0); delay(1);
      if (digitalRead(s4) == 1)
      {
        while ( digitalRead(s4) == 1)
          {
            // forward(90,90);
            analogWrite(enA, 90); analogWrite(enB, 90); 
            digitalWrite(in1,1); digitalWrite(in2,0);                          
            digitalWrite(in3,1); digitalWrite(in4,0);
          }
      }
      else if (digitalRead(s3) == 1)
      {
        while ( digitalRead(s3) == 1)
          {
            // forward(90,90);
            analogWrite(enA, 90); analogWrite(enB, 90); 
            digitalWrite(in1,1); digitalWrite(in2,0);                          
            digitalWrite(in3,1); digitalWrite(in4,0);
          }
      }
      
    }
    else if ( counting == 2)
    {
      // left(90,90);
      analogWrite(enA, 90); analogWrite(enB, 90); 
        digitalWrite(in1,0); digitalWrite(in2,1);                          
        digitalWrite(in3,1); digitalWrite(in4,0);                          
      delay(280);
      while ( digitalRead(s1) == 1)
      {
        // forward(90,90);
        analogWrite(enA, 90); analogWrite(enB, 90); // thiết lập tốc độ băm xung
        digitalWrite(in1,1); digitalWrite(in2,0);                          // cho motor bên trái quay thuận
        digitalWrite(in3,1); digitalWrite(in4,0);
      }
      ser.write(75);
    }
    else if ( counting == 3)
    {
      while (digitalRead(s5) == 1 && (digitalRead(s1)==0 || digitalRead(s2)==0 || digitalRead(s3)==0|| digitalRead(s4)==0)  )
      {
        // left(90,90);
        analogWrite(enA, 90); analogWrite(enB, 90); 
        digitalWrite(in1,0); digitalWrite(in2,1);                          
        digitalWrite(in3,1); digitalWrite(in4,0);
      }
      while (digitalRead(s5) == 0)
      {
        //left(90,90);
        analogWrite(enA, 90); analogWrite(enB, 90); 
        digitalWrite(in1,0); digitalWrite(in2,1);                          
        digitalWrite(in3,1); digitalWrite(in4,0);
      }
      pause(0, 0);
      // then go straight 
      while( digitalRead(s5) == 1)
      {
      //    forward(90,90);
            analogWrite(enA, 0); analogWrite(enB, 90); 
            digitalWrite(in1,1); digitalWrite(in2,0);                          
            digitalWrite(in3,1); digitalWrite(in4,0);
      }
       //half_left(0,90);
        analogWrite(enA, 0); analogWrite(enB, 90); 
        digitalWrite(in1,0); digitalWrite(in2,0);                          
        digitalWrite(in3,1); digitalWrite(in4,0);
        delay(100);
      // while ( digitalRead(s5) == 0)
      // {
      //   //half_left(0,90);
      //   analogWrite(enA, 0); analogWrite(enB, 90); 
      //   digitalWrite(in1,0); digitalWrite(in2,0);                          
      //   digitalWrite(in3,1); digitalWrite(in4,0);
      // }
      //---------------------------------------
      // if (digitalRead(s4) == 1)
      // {
      //   while ( digitalRead(s4) == 1)
      //     {
      //       // forward(90,90);
      //       analogWrite(enA, 90); analogWrite(enB, 90); 
      //       digitalWrite(in1,1); digitalWrite(in2,0);                          
      //       digitalWrite(in3,1); digitalWrite(in4,0);
      //     }
      //     delay(100);
      // }
      // else if (digitalRead(s3) == 1)
      // {
      //   while ( digitalRead(s3) == 1)
      //     {
      //       // forward(90,90);
      //       analogWrite(enA, 90); analogWrite(enB, 90); 
      //       digitalWrite(in1,1); digitalWrite(in2,0);                          
      //       digitalWrite(in3,1); digitalWrite(in4,0);
      //     }
      //     delay(100);
      // }
      // ser.write(5);
    }
    // else if (counting == 4||counting >4)
    // {
    //   while (digitalRead(s5) == 1)
    //   {
    //   analogWrite(enA, 100); analogWrite(enB, 100); 
    //   digitalWrite(in1,0); digitalWrite(in2,1);                          
    //   digitalWrite(in3,1); digitalWrite(in4,0); 
    //   }
    //   while ( digitalRead(s5) == 0 )
    //   {
    //   analogWrite(enA, 100); analogWrite(enB, 100); 
    //   digitalWrite(in1,0); digitalWrite(in2,1);                          
    //   digitalWrite(in3,1); digitalWrite(in4,0);
    //   }
    // }
    
  }
  
  if (((sensor[0] == 1) && (sensor[1] == 0) && (sensor[2] == 0) && (sensor[3] == 0) && (sensor[4] == 0)) ||// 10000
  ((sensor[0] == 1) && (sensor[1] == 1) && (sensor[2] == 0) && (sensor[3] == 0) && (sensor[4] == 0)) )  //11000
  {
          if(digitalRead(s4) == 0 || digitalRead(s1) == 1)              
          {      
            // right(100, 85); 
            analogWrite(enA, 100 ); analogWrite(enB, 85); 
            digitalWrite(in1,1); digitalWrite(in2,0);                          
            digitalWrite(in3,0); digitalWrite(in4,1);                           
            last_right = true;
            last_left = false;                   
            delay(80);
            digitalRead(s4);        
          }
  }
  else if (((sensor[0]==0)&&(sensor[1]==0)&&(sensor[2]==0)&&(sensor[3]==0)&&(sensor[4] == 1))|| //00001
           ((sensor[0]==0)&&(sensor[1]==0)&&(sensor[2]==0)&&(sensor[3]==1)&&(sensor[4] == 1)) ) //00011
  { 
              if (digitalRead(s2) == 0 || digitalRead(s5)== 1)        
              {
                // left(85, 100);   
                analogWrite(enA, 85); analogWrite(enB, 100); 
                digitalWrite(in1,0); digitalWrite(in2,1);                          
                digitalWrite(in3,1); digitalWrite(in4,0);                          
                last_left = true;
                last_right = false;            
                delay(80);
                digitalRead(s2);  
              }
  }
  else if ((sensor[0] == 1) && (sensor[1] == 1) && (sensor[2] == 1) && (sensor[3] == 1) && (sensor[4] == 0)) //11110
  {
    // right(115, 100); 
            analogWrite(enA, 125 ); analogWrite(enB, 100); 
            digitalWrite(in1,1); digitalWrite(in2,0);                          
            digitalWrite(in3,0); digitalWrite(in4,1);                           
            last_right = true;
            last_left = false;  
  }
  else if ((sensor[0] == 1) && (sensor[1] == 1) && (sensor[2] == 1) && (sensor[3] == 0) && (sensor[4] == 0)) //11100
  { // xe lệch phải
    // half_right(sp + 40 ,0);
    analogWrite(enA, sp+50); analogWrite(enB, 0); 
    digitalWrite(in1,1); digitalWrite(in2,0);                          
    digitalWrite(in3,0); digitalWrite(in4,0);                         
    last_right = true;
    last_left = false;
  }
  else if ((sensor[0] == 1) && (sensor[1] == 1) && (sensor[2] == 1) && (sensor[3] == 0) && (sensor[4] == 1)) //11101
  { //xe hơi lệch phải
    // forward(sp + 20, 0); 
        analogWrite(enA, sp+30); analogWrite(enB, 0); // thiết lập tốc độ băm xung
        digitalWrite(in1,1); digitalWrite(in2,0);                          // cho motor bên trái quay thuận
        digitalWrite(in3,1); digitalWrite(in4,0);
        last_right = false;
        last_left = true; 
  }
  else if ((sensor[0] == 1) && (sensor[1] == 1) && (sensor[2] == 0) && (sensor[3] == 0) && (sensor[4] == 1)) //11001
  { //xe có xu hướng lệch phải
    // forward(sp + 25, sp );
        analogWrite(enA, sp+25); analogWrite(enB, sp); // thiết lập tốc độ băm xung
        digitalWrite(in1,1); digitalWrite(in2,0);                          // cho motor bên trái quay thuận
        digitalWrite(in3,1); digitalWrite(in4,0);
        last_right = false;
        last_left = true; 
  }
  else if ((sensor[0] == 1) && (sensor[1] == 1) && (sensor[2] == 0) && (sensor[3] == 1) && (sensor[4] == 1)) //11011
  { //xe cân vạch
    lcd.setCursor(0,0); lcd.print("can");
    // forward(sp, sp );
        analogWrite(enA, sp); analogWrite(enB, sp); // thiết lập tốc độ băm xung
        digitalWrite(in1,1); digitalWrite(in2,0);                          // cho motor bên trái quay thuận
        digitalWrite(in3,1); digitalWrite(in4,0);
  }
  else if ((sensor[0] == 1) && (sensor[1] == 0) && (sensor[2] == 0) && (sensor[3] == 1) && (sensor[4] == 1)) //10011
  { //xe có xu hướng lệch trái
    // forward(sp, sp + 25);
        analogWrite(enA, sp); analogWrite(enB, sp+25); // thiết lập tốc độ băm xung
        digitalWrite(in1,1); digitalWrite(in2,0);                          // cho motor bên trái quay thuận
        digitalWrite(in3,1); digitalWrite(in4,0);
        last_right = true;
        last_left = false; 
  }
  else if ((sensor[0] == 1) && (sensor[1] == 0) && (sensor[2] == 1) && (sensor[3] == 1) && (sensor[4] == 1)) //10111
  { //xe hơi lệch trái
    // forward(0 , sp + 10);
    analogWrite(enA, 0); analogWrite(enB, sp+10); // thiết lập tốc độ băm xung
        digitalWrite(in1,1); digitalWrite(in2,0);                          // cho motor bên trái quay thuận
        digitalWrite(in3,1); digitalWrite(in4,0);
        last_right = true;
        last_left = false; 
  }
  else if ((sensor[0] == 0) && (sensor[1] == 0) && (sensor[2] == 1) && (sensor[3] == 1) && (sensor[4] == 1)) //00111
  { //xe lệch trái
    // half_left(0,sp +45);
    analogWrite(enA, 0); analogWrite(enB, sp+45); 
  digitalWrite(in1,0); digitalWrite(in2,0);                         
  digitalWrite(in3,1); digitalWrite(in4,0);                         
  last_right = true;
  last_left = false;
  }
  else if ((sensor[0] == 0) && (sensor[1] == 1) && (sensor[2] == 1) && (sensor[3] == 1) && (sensor[4] == 1)) //01111
  { //xe lệch trái
    // left(115, 100);
    analogWrite(enA, 115); analogWrite(enB, 100); 
      digitalWrite(in1,0); digitalWrite(in2,1);                          
      digitalWrite(in3,1); digitalWrite(in4,0);                          
      last_left = true;
      last_right = false;
  }
  else if ((sensor[0] == 0) && (sensor[1] == 0) && (sensor[2] == 0) && (sensor[3] == 0) && (sensor[4] == 0)) //00000
  { 
        if ( last_right == true && last_left == false)
        {
    //    delay(30);
        // forward(105, 90);
        analogWrite(enA, 105); analogWrite(enB, 90); // thiết lập tốc độ băm xung
        digitalWrite(in1,1); digitalWrite(in2,0);                          // cho motor bên trái quay thuận
        digitalWrite(in3,1); digitalWrite(in4,0);
        delay(90);
        pause(0, 0); delay(1);
        }
        else if  (last_right == false && last_left == true)
        {
    //      delay(30);
          // forward(90, 105);
        analogWrite(enA, 90); analogWrite(enB, 105); // thiết lập tốc độ băm xung
        digitalWrite(in1,1); digitalWrite(in2,0);                          // cho motor bên trái quay thuận
        digitalWrite(in3,1); digitalWrite(in4,0);
          delay(95); 
        pause(0, 0); delay(1);
        }
          int f = digitalRead(s6);
          int g = digitalRead(s7);
          if ( f == 1 && g == 1  ) 
        {
          pause(0,0); lcd.setCursor(0,0); lcd.print("win");dk = '0'; 
        }
//    read_sensor_values();
  }
  
  //=======================================================================================================
  else if ( (sensor[0]==1)&&(sensor[1] == 1)&&(sensor[2] == 1) && (sensor[3] == 1) && (sensor[4] == 1) ) //11111
  { 
    // if (last_right == true && last_left == false)
    // {
    //   backward(90,50);
    // }
    // else if (last_right == false && last_left == true)
    // {
    //   backward(50,90);
    // }
    backward(90,90);
  }
  //================================bắt đầu các trường hợp lỗi
  // else
  // { 
  //   left(90,90); delay(100);
  //   if ((sensor[0] == 0) && (sensor[1] == 0) && (sensor[2] == 0) && (sensor[3] == 1) && (sensor[4] == 0)) // 00010 v
  //   {
  //     // left(90,90); 
  //     analogWrite(enA, 90); analogWrite(enB, 90); 
  //     digitalWrite(in1,0); digitalWrite(in2,1);                          
  //     digitalWrite(in3,1); digitalWrite(in4,0);                          
  //     last_left = true;
  //     last_right = false;
  //   }
  //   else if ((sensor[0] == 0) && (sensor[1] == 0) && (sensor[2] == 1) && (sensor[3] == 1) && (sensor[4] == 0)) // 00110 v
  //   {
  //     // left(90,90); 
  //     analogWrite(enA, 90); analogWrite(enB, 90); 
  //     digitalWrite(in1,0); digitalWrite(in2,1);                          
  //     digitalWrite(in3,1); digitalWrite(in4,0);                          
  //     last_left = true;
  //     last_right = false;
  //   }
  //    else if ((sensor[0] == 0) && (sensor[1] == 0) && (sensor[2] == 1) && (sensor[3] == 1) && (sensor[4] == 0)) // 01011 v
  //   {
  //     // left(90,90);
  //     analogWrite(enA, 90); analogWrite(enB, 90); 
  //     digitalWrite(in1,0); digitalWrite(in2,1);                          
  //     digitalWrite(in3,1); digitalWrite(in4,0);                          
  //     last_left = true;
  //     last_right = false; 
  //   }
  //   else if ((sensor[0] == 1) && (sensor[1] == 1) && (sensor[2] == 0) && (sensor[3] == 1) && (sensor[4] == 0)) // 11010 v
  //   {
  //     // left(90,90);
  //     analogWrite(enA, 90); analogWrite(enB, 90); 
  //     digitalWrite(in1,0); digitalWrite(in2,1);                          
  //     digitalWrite(in3,1); digitalWrite(in4,0);                          
  //     last_left = true;
  //     last_right = false;
  //   }
  //    else if ((sensor[0] == 1) && (sensor[1] == 0) && (sensor[2] == 0) && (sensor[3] == 0) && (sensor[4] == 1)) // 10001
  //   {
  //     // left(90,90);
  //     analogWrite(enA, 90); analogWrite(enB, 90); 
  //     digitalWrite(in1,0); digitalWrite(in2,1);                          
  //     digitalWrite(in3,1); digitalWrite(in4,0);                          
  //     last_left = true;
  //     last_right = false;
  //   }
  //   goto oops;
  // }//kết thúc các trường hợp lỗi
  
}
void counter()
{
  counting += 1;
}
//-------------------------------------------------------------------------------------------------------
void setup() {
  digitalWrite(signalPin, statics);
  lcd.init(); lcd.backlight(); lcd.print("done");
  Serial.begin(9600); hi.begin(9600);// bắt đầu mở cổng kết nối nối tiếp tốc độ 9600
  //khai báo các chân nối cảm biến là đầu vào
  pinMode(s1, INPUT_PULLUP); pinMode(s2, INPUT_PULLUP);
  pinMode(s3, INPUT_PULLUP); pinMode(s4, INPUT_PULLUP);
  pinMode(s5, INPUT_PULLUP);pinMode(s6, INPUT_PULLUP);pinMode(s7, INPUT_PULLUP);
  // khai báo các chân điều khiển tốc độ và chiều quay là đầu ra
  pinMode(enA, OUTPUT); pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT); pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT); pinMode(in4, OUTPUT);
  pinMode(trig, OUTPUT); //Chân trig xuất tín hiệu
  pinMode(echo, INPUT); //Chân echo nhận tín hiệu
  pinMode(signalPin,OUTPUT); //kích ngắt
  ser.attach(4); ser.write(30);
  // attachInterrupt(4, counter, CHANGE);
  dk = '0';// gán biến dk =0
  pause(0, 0);
  analogWrite(enA, 0); analogWrite(enB, 0); // không cho phép xe chạy khi mới khởi tạo, chờ lệnh điều khiển của người dùng
  Serial.println("setup is done");//in lên màn hình máy tính khi setup xong
  lcd.clear();
}//setup()

void loop() {
  if (hi.available()) // nếu chân RX có tín hiệu thì sẽ thực hiện các câu lệnh trong {} dưới đây
  {
    dk = hi.read(); //nếu chân RX có tín hiệu thì sẽ đọc và gán vào biến dk
    //Serial.println("dk " + String(dk));
  }//if Serial.available()
  else if ( dk == 'R') {// nếu dk == R thì xe sẽ chạy dò line tự động
    auto_run();         // hàm auto_run() là hàm giúp xe chạy dò line tự động
  }//R
  else if ( dk == '6') {// nếu dk == 6 thì sẽ tăng tốc độ đặt
    pause(0, 0);
    sp = sp + 5;    // tăng biến sp thêm 5 đơn vị
    spA = sp + 3;  // gán biến spA hơn biến sp 3 đơn vị
    spB = sp + 3; // gán biến spB hơn biến sp 3 đơn vị
    if (sp > 200) {
      sp = 245; // giới hạn độ lớn của biến sp
    }
    Serial.println("speed: " + String(sp) ); //in lên màn hình máy tính thông số vừa được thay đổi
    dk = '0' ;                                 // làm mới chương trình bằng cách gán dk = 0 và chờ lệnh điều khiển mới
  }//faster
  else if (dk == '5') {// nếu dk == 5 thì sẽ giảm tốc độ đặt. trường hợp này ngược lại dk == 6
    pause(0, 0);
    sp = sp - 5;
    spA = sp + 3;
    spB = spA + 3;
    if (sp < 70) {
      sp = 70;
    }
    Serial.println("speed: "  + String(sp) );
    dk = '0';
  }//slow down
  else if (dk == 'P') {// nếu dk == P thì xe dừng
    Serial.println("Pause"); // in lên màn hình máy tính trạng thái của robot
    pause(0, 0);             // gọi hàm pause
    dk = 0;                  // gán lại biến dk = 0 và chờ lệnh điều khiển tiếp theo
  }// P
  else if (dk == '0') {// dk == 0 thì xe dừng
    pause(0, 0);
  }//if
  else if ( dk == '1' ) {                      // dk ==1 thì xe đi thẳng
    Serial.println("forward");                 // in lên màn hình máy tính
    forward(sp, sp);                       // cho xe đi thẳng với tốc độ sp và sp+5
  }//if
  else if ( dk == '2') {                       // nếu dk ==2 thì xe đi ngược lại
    Serial.println("backward");                // in lên màn hình máy tính
    backward(sp, sp);                  // lùi xe với tốc độ sp cho cả hai bên bánh
  }//if
  else if (dk == '3') {                        // nếu dk == 3 thì xe rẽ trái
    Serial.println("left");                    // in lên màn hình máy tính
    half_left(0, sp+5);                          // rẽ trái
  }//if
  else if (dk == '4') {                        // nếu dk == 4 thì xe rẽ phải
    Serial.println("right");
    half_right(sp, 0);                           // quay sang phải
  }//if
  else if (dk == 'a') 
  { 
  counting = 0; ser.write(30); dk = 'R';
  }
  else if ( dk == 'b') 
  { 
    counting = 1; ser.write(30); dk = 'R';
  }
  else if ( dk == 'c')
  {
    counting = 2; ser.write(75);dk = 'R';
  }
  else if ( dk == 'd')
  {
    counting = 3; ser.write(0);dk = 'R';
  }
  else if ( dk == 'e')
  {
    right(90,90); 
    delay(100);
    dk = 'R'; 
  }
//  get_distance(); lcd.setCursor(0,0);lcd.print(distance); lcd.print("   ");
}//loop()