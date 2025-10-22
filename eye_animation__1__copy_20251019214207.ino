#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// Pervanenin dönüşü için genel değişkenler
float prop_angle = 0;
float prop_speed = 22.0; // Pervanenin dönüş hızı

// --- YARDIMCI FONKSİYONLAR ---

void draw_bold_text(String text, int x, int y, int size) {
    display.setTextSize(size);
    display.setTextColor(WHITE);
    display.setCursor(x, y);
    display.print(text);
    display.setCursor(x + 1, y);
    display.print(text);
}

void draw_thin_propeller(int centerX, int centerY, int radius, float angle) {
    display.fillCircle(centerX, centerY, 5, WHITE);
    display.fillCircle(centerX, centerY, 3, BLACK);
    
    for (int i = 0; i < 3; i++) {
        float current_angle_deg = angle + (i * 120);
        float current_angle_rad = radians(current_angle_deg);
        int start_radius = 5;
        float tip_x = centerX + radius * cos(current_angle_rad);
        float tip_y = centerY + radius * sin(current_angle_rad);
        float base_width_angle = 7;
        float p_base1_x = centerX + start_radius * cos(current_angle_rad + radians(base_width_angle));
        float p_base1_y = centerY + start_radius * sin(current_angle_rad + radians(base_width_angle));
        float p_base2_x = centerX + start_radius * cos(current_angle_rad - radians(base_width_angle));
        float p_base2_y = centerY + start_radius * sin(current_angle_rad - radians(base_width_angle));
        
        // DEĞİŞİKLİK: Kavis yönü tam tersine çevrildi
        float mid_x = centerX + (radius * 0.6) * cos(current_angle_rad + radians(25));
        float mid_y = centerY + (radius * 0.6) * sin(current_angle_rad + radians(25));
        
        display.fillTriangle((int)p_base1_x, (int)p_base1_y, (int)tip_x, (int)tip_y, (int)mid_x, (int)mid_y, WHITE);
        display.fillTriangle((int)p_base2_x, (int)p_base2_y, (int)tip_x, (int)tip_y, (int)mid_x, (int)mid_y, WHITE);
    }
}

void spin_propeller_for(int duration_seconds) {
    unsigned long animation_start_time = millis();
    while (millis() - animation_start_time < (duration_seconds * 1000)) {
        prop_angle += prop_speed;
        if (prop_angle >= 360) { prop_angle -= 360; }
        display.clearDisplay();
        draw_thin_propeller(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 30, prop_angle);
        display.display();
    }
}

// YENİ FONKSİYON: Üstten drone animasyonu
void drone_top_view_animation(int duration_seconds) {
    unsigned long animation_start_time = millis();
    int centerX = SCREEN_WIDTH / 2;
    int centerY = SCREEN_HEIGHT / 2;
    int arm_length = 25;
    int prop_radius = 8;

    while (millis() - animation_start_time < (duration_seconds * 1000)) {
        prop_angle += prop_speed;
        if (prop_angle >= 360) { prop_angle -= 360; }
        
        display.clearDisplay();

        // Drone gövdesi
        display.fillRoundRect(centerX - 10, centerY - 10, 20, 20, 4, WHITE);

        // 4 kol ve dönen pervaneler
        for(int i = 0; i < 4; i++) {
            float arm_angle = radians(45 + i * 90); // 45, 135, 225, 315 derece
            int motor_x = centerX + arm_length * cos(arm_angle);
            int motor_y = centerY + arm_length * sin(arm_angle);
            
            // Kol
            display.drawLine(centerX, centerY, motor_x, motor_y, WHITE);
            
            // Pervane diski (dönüş hissi için)
            display.drawCircle(motor_x, motor_y, prop_radius, WHITE);

            // Dönen pervane palleri
            float blade1_rad = radians(prop_angle);
            float blade2_rad = radians(prop_angle + 180);
            display.drawLine(motor_x - prop_radius * cos(blade1_rad), motor_y - prop_radius * sin(blade1_rad),
                             motor_x + prop_radius * cos(blade1_rad), motor_y + prop_radius * sin(blade1_rad), WHITE);
        }
        
        display.display();
    }
}


// --- ANA KURULUM VE DÖNGÜ ---

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  Serial.begin(115200);

  // --- BİRİNCİL AÇILIŞ SEKANSI ---

  // 1. Adım
  display.clearDisplay();
  draw_bold_text("FPV SYS", (SCREEN_WIDTH - (6 * 12))/2, 25, 2); 
  display.display();
  delay(3000);

  // 2. Adım
  display.clearDisplay();
  draw_bold_text("READY", (SCREEN_WIDTH - (5 * 12))/2, 5, 2);
  draw_bold_text("TO", (SCREEN_WIDTH - (2 * 12))/2, 25, 2);
  draw_bold_text("SKY", (SCREEN_WIDTH - (3 * 12))/2, 45, 2);
  display.display();
  delay(3000);

  // 3. Adım
  display.clearDisplay();
  draw_bold_text("ARMED", (SCREEN_WIDTH - (5 * 18))/2, 20, 3);
  display.display();
  for(int i=0; i < 4; i++) { 
    display.invertDisplay(true); delay(80); display.invertDisplay(false); delay(80); 
  }
  delay(1000);

  // 4. Adım
  spin_propeller_for(10);

  // YENİ 4-5 arası adım
  drone_top_view_animation(10);
}

void loop() {
  // --- TEKRARLAYAN UÇUŞ DÖNGÜSÜ ---

  // 5. Adım
  display.clearDisplay();
  draw_bold_text("FPV SYS", (SCREEN_WIDTH - (6 * 12))/2, 25, 2); 
  display.display();
  delay(3000);

  // 6. Adım
  display.clearDisplay();
  draw_bold_text("IN FLIGHT", (SCREEN_WIDTH - (9 * 12))/2, 25, 2);
  display.display();
  delay(3000);

  // 7. Adım
  spin_propeller_for(10);
  
  // YENİ 7-8 arası adım
  drone_top_view_animation(10);

  // 8. Adım: Döngü otomatik olarak 5. adımdan tekrar başlar.
}