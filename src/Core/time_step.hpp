#ifndef SLASHENGINE_CORE_TIME_STEP_H_
#define SLASHENGINE_CORE_TIME_STEP_H_

namespace slash {

class TimeStep {
public:
  explicit TimeStep(double time = 0.0) : time_(time) {}

  explicit operator double() const { return time_; }

  [[nodiscard]] double GetSecond() const { return time_; }
  [[nodiscard]] double GetMilliseconds() const { return time_ * 1000.0; }

private:
  double time_;
};

} // namespace slash

#endif // SLASHENGINE_CORE_TIME_STEP_H_