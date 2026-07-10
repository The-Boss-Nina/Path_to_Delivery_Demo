#ifndef SPEEDOMETER_H
#define SPEEDOMETER_H

class Speedometer {
public:
    Speedometer();
    void updateSpeed(float speed);
    float getSpeed() const;

private:
    float speed_;
};

#endif // SPEEDOMETER_H