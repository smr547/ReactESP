#include <Arduino.h>
#include <ReactESP.h>

using namespace reactesp;

#define LED_PIN 2

int led_state = 0;
int cnt = 0;

EventLoop event_loop;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");
  pinMode(LED_PIN, OUTPUT);

  Serial.println("Setting up timed events");

  // toggle LED every 400 ms
  auto* timer1 = event_loop.onPauseableRepeat(400, []() {
    led_state = !led_state;
    digitalWrite(LED_PIN, led_state);
    Serial.println("timer1 fired");
    Serial.print(event_loop.getTimedEventQueueSize());
    Serial.println(" events in timed queue");
  });

  // Additionally, toggle LED every 1020 ms.
  // This adds an irregularity to the LED blink pattern.
  auto* timer2 = event_loop.onRepeat(1020, [timer1]() {
    led_state = !led_state;
    digitalWrite(LED_PIN, led_state);
    Serial.println("timer2 fired");

    cnt += 1;
    if (cnt == 5) {
      if (timer1->isPaused()) {
        Serial.println("restarting timer1");
        timer1->restart();
      } else {
        Serial.println("pausing timer1");
        timer1->pause();

        Serial.println("paused timer1");
      }
      // timer1->pause();

      cnt = 0;
      Serial.print(event_loop.getTimedEventQueueSize());
      Serial.println(" events in timed queue");
    }
  });
}

void loop() { event_loop.tick(); }
