#include "event_loop.h"

namespace reactesp {

void EventLoop::tickTimed() {
  xSemaphoreTakeRecursive(timed_queue_mutex_, portMAX_DELAY);
  const uint64_t now = micros64();
  TimedEvent* top = nullptr;

  while (true) {
    if (timed_queue.empty()) {
      break;
    }
    top = timed_queue.top();
    if (!top->isEnabled()) {
      timed_queue.pop();
      delete top;
      continue;
    }
    const uint64_t trigger_t = top->getTriggerTimeMicros();
    if (now >= trigger_t) {
      timed_queue.pop();
      top->tick(this);
    } else {
      break;
    }
  }
  xSemaphoreGiveRecursive(timed_queue_mutex_);
}

void EventLoop::tickUntimed() {
  xSemaphoreTakeRecursive(untimed_list_mutex_, portMAX_DELAY);
  for (UntimedEvent* re : this->untimed_list) {
    re->tick(this);
  }
  xSemaphoreGiveRecursive(untimed_list_mutex_);
}

void EventLoop::tick() {
  tickUntimed();
  tickTimed();
}

DelayEvent* EventLoop::onDelay(uint32_t delay, react_callback callback) {
  auto* dre = new DelayEvent(delay, callback);
  dre->add(this);
  return dre;
}

DelayEvent* EventLoop::onDelayMicros(uint64_t delay,
                                       react_callback callback) {
  auto* dre = new DelayEvent(delay, callback);
  dre->add(this);
  return dre;
}

RepeatEvent* EventLoop::onRepeat(uint32_t interval, react_callback callback) {
  auto* rre = new RepeatEvent(interval, callback);
  rre->add(this);
  return rre;
}

RepeatEvent* EventLoop::onRepeatMicros(uint64_t interval,
                                         react_callback callback) {
  auto* rre = new RepeatEvent(interval, callback);
  rre->add(this);
  return rre;
}

StreamEvent* EventLoop::onAvailable(Stream& stream, react_callback callback) {
  auto* sre = new StreamEvent(stream, callback);
  sre->add(this);
  return sre;
}

ISREvent* EventLoop::onInterrupt(uint8_t pin_number, int mode,
                                   react_callback callback) {
  auto* isrre = new ISREvent(pin_number, mode, callback);
  isrre->add(this);
  return isrre;
}

TickEvent* EventLoop::onTick(react_callback callback) {
  auto* tre = new TickEvent(callback);
  tre->add(this);
  return tre;
}

void EventLoop::remove(Event* event) { event->remove(this); }

}  // namespace reactesp
