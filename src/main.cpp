#include <Arduino.h>
#include "led/Led.h"
#include "hcsr04/HC_SR04.h"
#include<cstring>  // Pour pouvoir utiliser strcmp

//Les broches
const int PIN_RED = 4;
const int PIN_GREEN = 5;
const int PIN_YELLOW = 6;

const int PIN_ECHO = 7;
const int PIN_TRIG = 8;

const char* GREEN = "GREEN";
const char* YELLOW = "YELLOW";
const char* RED = "RED";

float refDistance ;

SemaphoreHandle_t distMutex = nullptr;
SemaphoreHandle_t serialMutex = nullptr;

TaskHandle_t greenHandle = nullptr;
TaskHandle_t redHandle = nullptr;
TaskHandle_t yellowHandle = nullptr;

TaskHandle_t sensorHandle = nullptr;
TaskHandle_t displayHandle = nullptr;

//Instances

Led ledGreen(PIN_GREEN, GREEN);
Led ledYellow(PIN_YELLOW, YELLOW);
Led ledRed(PIN_RED, RED);

HC_SR04 sensor(PIN_TRIG, PIN_ECHO);

void vTaskMeasure( void* pvParam){

 float distance;

  while(true){

    distance = sensor.getDistance();

    xSemaphoreTake(distMutex, portMAX_DELAY);
    refDistance = distance;
    xSemaphoreGive(distMutex);

    vTaskDelay(pdMS_TO_TICKS(1000));    // Augmenter le delai pour mieux voir l'effet
  }

}

void vTaskDisplay(void* param){

  float distance;

  while(true){

    xSemaphoreTake(distMutex, portMAX_DELAY);
    distance = refDistance;
    xSemaphoreGive(distMutex);

    xSemaphoreTake(serialMutex, portMAX_DELAY);
    Serial.print("La distnace mesurée est :");
    Serial.println(distance);
    xSemaphoreGive(serialMutex);

    vTaskDelay(pdMS_TO_TICKS(1000));
  }

}

void vTaskLed(void* param){

  Led *led = (Led*)param;
  float distance;
  bool etat;

  while(true){
     etat=false;
     xSemaphoreTake(distMutex, portMAX_DELAY);
     distance = refDistance;
     xSemaphoreGive(distMutex);

    if(distance >=0 && distance < 20 && strcmp(led->getName(), RED) == 0){
      //led->on();
      etat = true;
      xSemaphoreTake(serialMutex, portMAX_DELAY);
      Serial.println("LED rouge on");
      xSemaphoreGive(serialMutex);
     
      
    } else if(distance >=20 && distance < 40 && strcmp(led->getName(), YELLOW) == 0){

      //led->on();
      etat = true;

      xSemaphoreTake(serialMutex, portMAX_DELAY);
      Serial.println("LED jaune on");
      xSemaphoreGive(serialMutex);

    } else if(distance >=40 && strcmp(led->getName(), GREEN) == 0){

      //led->on();
      etat = true;

      xSemaphoreTake(serialMutex, portMAX_DELAY);
      Serial.println("LED vert on");
      xSemaphoreGive(serialMutex);
    }

    if(etat){
      led->on();
    } else {
      led->off();
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void setup() {
  Serial.begin(115200);
  delay(3000);

    //Initialisations

  ledGreen.init();
  ledRed.init();
  ledYellow.init();
  sensor.init();  

  distMutex = xSemaphoreCreateMutex();
  serialMutex= xSemaphoreCreateMutex();

  if (distMutex == nullptr || serialMutex == nullptr) {
    Serial.println("Erreur : creation des mutex impossible");

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }

   xTaskCreate(
        vTaskMeasure,
        "Measure",
        4096,
        nullptr,
        2,
        &sensorHandle
    );

  xTaskCreate(
        vTaskDisplay,
        "Display",
        4096,
        nullptr,
        2,
        &displayHandle
  );

  xTaskCreate(
        vTaskLed,
        GREEN,
        4096,
        &ledGreen,
        2,
        &greenHandle
  );

  xTaskCreate(
        vTaskLed,
        YELLOW,
        4096,
        &ledYellow,
        2,
        &yellowHandle
  );

  xTaskCreate(
        vTaskLed,
        RED,
        4096,
        &ledRed,
        2,
        &redHandle
  );
}

void loop() {
  // put your main code here, to run repeatedly:
}
