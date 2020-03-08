#ifndef SLASHENGINE_CORE_TIME_STEP_H_
#define SLASHENGINE_CORE_TIME_STEP_H_

namespace slash {

class TimeStep {
public:
  explicit TimeStep(float time = 0.0f) : time_(time) {}

  explicit operator float() const { return time_; }

  [[nodiscard]] float GetSecond() const { return time_; }
  [[nodiscard]] float GetMilliseconds() const { return time_ * 1000.0f; }

private:
  float time_;
};

} // namespace slash

#endif // SLASHENGINE_CORE_TIME_STEP_H_