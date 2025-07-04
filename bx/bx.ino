#include <Servo.h> 
#include <Wire.h> 
#include <SPI.h> 
#include <MFRC522.h> 
#include <LiquidCrystal_I2C.h> 
// Định nghĩa các chân kết nối 
#define SS_PINin 37 
#define RST_PIN 5 
#define SS_PINout 42 
#define servo_Pin 6 
#define servo_Pout 7 
MFRC522 mfrc522_in(SS_PINin, RST_PIN); 
MFRC522 mfrc522_out(SS_PINout, RST_PIN); 
Servo servo_in; 
Servo servo_out; 
LiquidCrystal_I2C lcd(0x27, 20, 4); 
int ir_car1 = 8; 
int ir_car2 = 9; 
int ir_car3 = 10; 
int ir_car4 = 11; 
int ir_sensor_in = 12; 
int ir_sensor_out = 13; 
int S1 = 0, S2 = 0, S3 = 0, S4 = 0; 
int slot = 4; 


void setup() { 
Serial.begin(9600); 
pinMode(ir_car1, INPUT); 
pinMode(ir_car2, INPUT); 
pinMode(ir_car3, INPUT); 
pinMode(ir_car4, INPUT); 
pinMode(ir_sensor_in, INPUT); 
pinMode(ir_sensor_out, INPUT); 
SPI.begin(); 
mfrc522_in.PCD_Init(); 
mfrc522_out.PCD_Init(); 
servo_in.attach(servo_Pin); 
servo_out.attach(servo_Pout); 
servo_in.write(0); 
servo_out.write(0); 
Serial.println("BAI GUI XE"); 
lcd.init(); 
lcd.backlight(); 
lcd.setCursor(0, 0); 
lcd.print("BAI GUI XE"); 
delay(2000); 
lcd.clear(); 
Read_Sensor(); 
updateSlotOnLCD(slot); 
 
} 
 
void loop() { 
  Read_Sensor(); 
  lcd.setCursor(0, 0); 
  lcd.print("Cho Trong: "); 
  lcd.print(slot); 
  lcd.print("  "); 
  lcd.setCursor(0, 1); 
  if (S1 == 1) { 
    lcd.print("S1:D "); 
  } else { 
    lcd.print("S1:T"); 
  } 
  lcd.setCursor(10, 1); 
  if (S2 == 1) { 
    lcd.print("S2:D "); 
  } else { 
    lcd.print("S2:T"); 
  } 
  lcd.setCursor(0, 2); 
  if (S3 == 1) { 
    lcd.print("S3:D "); 
  } else { 
    lcd.print("S3:T"); 
  } 
  lcd.setCursor(10, 2); 
  if (S4 == 1) { 
    lcd.print("S4:D"); 
 
  } else { 
    lcd.print("S4:T"); 
  } 
  checkRFID_in(); 
  checkRFID_out(); 
} 
void Read_Sensor() { 
  S1 = 0, S2 = 0, S3 = 0, S4 = 0; 
  if (digitalRead(ir_car1) == 0) { S1 = 1; } 
  if (digitalRead(ir_car2) == 0) { S2 = 1; } 
  if (digitalRead(ir_car3) == 0) { S3 = 1; } 
  if (digitalRead(ir_car4) == 0) { S4 = 1; } 
} 
 
void checkRFID_in() { 
  if (mfrc522_in.PICC_IsNewCardPresent() && mfrc522_in.PICC_ReadCardSerial()) { 
    String uid = getUID(mfrc522_in); 
    Serial.print("RFID vào: "); 
    Serial.println(uid); 
 
    if (isAuthorized(uid)) { 
      if (slot > 0) { 
        lcd.setCursor(0, 3); 
        lcd.print("HOP LE "); 
        delay(1000); 
        lcd.setCursor(0, 3); 
        lcd.print("                "); 
        Serial.println("Cửa vào mở"); 
        servo_in.write(90); 
 
        // Hiển thị ô trống 
        String blank = "MOI VAO O: "; 
        bool kt = false; 
        if (S1 == 0) { 
          blank += "1 "; 
          kt = true; 
        } 
        if (S2 == 0) { 
          blank += "2 "; 
          kt = true; 
        } 
        if (S3 == 0) { 
          blank += "3 "; 
          kt = true; 
        } 
        if (S4 == 0) { 
          blank += "4 "; 
          kt = true; 
        } 
 
        if (kt) { 
          lcd.setCursor(0, 3); 
          lcd.print(blank); 
          Serial.println(blank); 
          delay(1500); 
          lcd.setCursor(0, 3); 
          lcd.print("                  "); 
        } 
        bool carPassed = false; 

 
        unsigned long startTime = millis();  // Thời gian bắt đầu đợi 
        while (!carPassed && (millis() - startTime < 10000)) { 
          // Trong vòng 10 giây, kiểm tra cảm biến hồng ngoại 
          if (digitalRead(ir_sensor_in) == 0) { 
            // Xe đã đi qua cổng vào 
            carPassed = true; 
          } 
        } 
        if (carPassed) { 
          delay(1000); 
          Serial.print("xe da qua"); 
          servo_in.write(0); 
          slot = max(0, slot - 1); 
          updateSlotOnLCD(slot); 
          Serial.println("Vao"); 
        } else { 
          Serial.println("k co xe qua"); 
          servo_in.write(0); 
        } 
      } else { 
        lcd.setCursor(0, 3); 
        lcd.print("Het Cho"); 
        delay(3000); 
        lcd.setCursor(0, 3); 
        lcd.print("                      "); 
        Serial.println("Full"); 
      } 
    } else { 
      lcd.setCursor(0, 3); 
 
      lcd.print("THE KHONG HOP LE"); 
      delay(500); 
      lcd.setCursor(0, 3); 
      lcd.print("                                                       "); 
      Serial.println("the khong hop le"); 
    } 
    mfrc522_in.PICC_HaltA(); 
  } 
} 
 
void checkRFID_out() { 
  if (mfrc522_out.PICC_IsNewCardPresent() && mfrc522_out.PICC_ReadCardSerial()) { 
    String uid = getUID(mfrc522_out); 
    Serial.print("RFID ra: "); 
    Serial.println(uid); 
 
    if (isAuthorized(uid)) { 
      Serial.println("Cửa ra mở"); 
      lcd.setCursor(0, 3); 
      lcd.print("HOP LE "); 
      lcd.setCursor(0, 3); 
      delay(1000); 
      lcd.print("                 "); 
      servo_out.write(90); 
      bool carPassed = false; 
      unsigned long startTime = millis();  // Thời gian bắt đầu đợi 
      while (!carPassed && (millis() - startTime < 10000)) { 
        // Trong vòng 10 giây, kiểm tra cảm biến IR 
        if (digitalRead(ir_sensor_out) == 0) { 

          // Xe đã đi qua cổng ra 
          carPassed = true; 
        } 
      } 
 
      if (carPassed) { 
        Serial.print("xe da qua"); 
        delay(1000); 
        servo_out.write(0); 
        slot = min(4, slot + 1);  // Đảm bảo slot không vượt quá 4 
        updateSlotOnLCD(slot); 
        Serial.println("Ra"); 
      } else { 
        servo_out.write(0); 
      } 
    } else { 
      lcd.setCursor(0, 3); 
      lcd.print("THE KHONG HOP LE "); 
      lcd.setCursor(0, 3); 
      delay(3000); 
      lcd.print("                 "); 
      Serial.println("k hop le"); 
    } 
 
    mfrc522_out.PICC_HaltA(); 
  } 
} 
 
String getUID(MFRC522& mfrc522) { 
String uid = ""; 
for (byte i = 0; i < mfrc522.uid.size; i++) { 
uid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""); 
uid += String(mfrc522.uid.uidByte[i], HEX); 
} 
uid.toUpperCase(); 
return uid; 
} 
bool isAuthorized(String uid) { 
// Thêm các UID hợp lệ tại đây 
if (uid == "C33ABBAD" || uid == "037DABAD" || uid == "A351FE34" || uid == "83441230") { 
return true; 
} 
return false; 
} 
void updateSlotOnLCD(int slot) { 
lcd.setCursor(10, 0); 
lcd.print(slot); 
}