#include <Arduino_GFX_Library.h>
#include "esp_camera.h"

/* * EKRAN AYARLARI 
 * SCL (SCK): GPIO 14

SDA (MOSI): GPIO 13

RES (Reset): GPIO 12

DC (Data/Command): GPIO 2

 */
Arduino_DataBus *bus = new Arduino_ESP32SPI(2, -1, 14, 13, -1);

/* * Ekran Sürücüsü: ST7789 (Yaygın CS'siz ekranlar için)
 * Eğer ekranın farklıysa (ILI9341 vb.) burayı değiştir.
 * RES pinini 12'ye bağladık.
 */
Arduino_GFX *gfx = new Arduino_ST7789(
  bus, 12 /* RST */, 0 /* Rotation */, true /* IPS */, 
  240 /* W */, 240 /* H */, 0, 0, 0, 0
);

/* KAMERA PİNLERİ (AI-THINKER MODÜLÜ) */
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void setup() {
  Serial.begin(115200);

  // 1. Ekranı Başlat
  if (!gfx->begin()) {
    Serial.println("Ekran hatası!");
  }
  
  // Renk hatasını önlemek için Hex kodu (0x0000 = Siyah) kullanıyoruz
  gfx->fillScreen(0x0000); 
  gfx->setRotation(1); 

  // 2. Kamera Yapılandırması
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_RGB565; // Renkli TFT için gerekli format
  
  // Çözünürlük: Ekranın 240x240 ise bu ayar idealdir
  config.frame_size = FRAMESIZE_240X240; 
  config.fb_count = 1;
  config.fb_location = CAMERA_FB_IN_PSRAM; // PSRAM kullanımı

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Kamera başlatılamadı: 0x%x", err);
    return;
  }
  
  Serial.println("Kamera ve Ekran Hazır!");
}

void loop() {
  // Kameradan görüntü al
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("FB Alınamadı");
    delay(100);
    return;
  }

  // Görüntüyü ekrana çiz (Big Endian RGB565)
  gfx->draw16bitBeRGBBitmap(0, 0, (uint16_t *)fb->buf, fb->width, fb->height);

  // Belleği geri ver (Unutulursa sistem kilitlenir)
  esp_camera_fb_return(fb);
}
