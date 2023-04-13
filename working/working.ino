#include "esp_camera.h"
#include "Arduino.h"
#include "SD_MMC.h"

const int captureInterval = 10000; //capture interval in ms
int imageCount = 1; //current image count
String imageName; //name of the current image

void setup() {
  Serial.begin(115200);

  //initialize camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_UXGA;
  config.jpeg_quality = 10;
  config.fb_count = 2;

  //initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  Serial.println("Camera init successful");

  //initialize SD card
  if (!SD_MMC.begin()) {
    Serial.println("SD card initialization failed");
    return;
  }

  Serial.println("Sd card init successful");
}

void loop() {
  //wait for 30 seconds before capturing the next image
  delay(captureInterval);

  //capture image
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  //rename image
  imageName = String(imageCount) + ".jpg";
  Serial.println("Image name: " + imageName);

  //write image to file
  File file = SD_MMC.open("/" + imageName, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file");
    return;
  }

  file.write(fb->buf, fb->len); //write image data to file
  file.close();

  //free image buffer
  esp_camera_fb_return(fb);

  //increment image count
  imageCount++;

  //if image count is greater than 6, reset image count to 1
  if (imageCount == 7) {
    imageCount = 1;
  }
}