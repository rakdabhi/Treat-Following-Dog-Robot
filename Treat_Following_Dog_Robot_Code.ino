enum MotorState {
  STOPPED = 0,
  CLOCKWISE = 1,
  COUNTER_CLOCKWISE = 2
};

// represents a side of the robot
class WheelSide {
public:
  const int back_motor_pin;
  const int front_motor_pin;
  const int enable;

  WheelSide(int back_motor_pin, int front_motor_pin, int enable)
  : back_motor_pin(back_motor_pin),
    front_motor_pin(front_motor_pin),
    enable(enable) {

    pinMode(this->enable, OUTPUT);
    pinMode(this->back_motor_pin, OUTPUT);
    pinMode(this->front_motor_pin, OUTPUT);

  }

  void set_state(MotorState state, int speed = 255) {
    switch (state) {
      case STOPPED:
        digitalWrite(this->back_motor_pin,  LOW);
        digitalWrite(this->front_motor_pin, LOW);
        break;
      case CLOCKWISE: 
        digitalWrite(this->back_motor_pin,  LOW);
        digitalWrite(this->front_motor_pin, HIGH);
        break;
      case COUNTER_CLOCKWISE:
        digitalWrite(this->back_motor_pin,  HIGH);
        digitalWrite(this->front_motor_pin, LOW);
        break;
    }
    analogWrite(this->enable, speed);
  }
};

enum RobotState {
  STOP,
  FORWARD,
  BACKWARD,
  TURN_LEFT,
  TURN_RIGHT
};

class Robot {
public:
  WheelSide L, R;

  Robot(int L1, int L2, int L_enable, int R1, int R2, int R_enable)
  : L(L1, L2, L_enable), R(R1, R2, R_enable) {}

  void move(RobotState state, int L_speed = 255, int R_speed = 255) {
    switch (state) {
      case STOP:
        this->L.set_state(STOPPED, L_speed);
        this->R.set_state(STOPPED, R_speed);
        break;
      case FORWARD:
        this->L.set_state(CLOCKWISE, L_speed);
        this->R.set_state(CLOCKWISE, R_speed);
        break;
      case BACKWARD:
        this->L.set_state(COUNTER_CLOCKWISE, L_speed);
        this->R.set_state(COUNTER_CLOCKWISE, R_speed);
        break;
      case TURN_RIGHT:
        this->L.set_state(CLOCKWISE, L_speed);
        this->R.set_state(COUNTER_CLOCKWISE, R_speed);
        break;
      case TURN_LEFT:
        this->L.set_state(COUNTER_CLOCKWISE, L_speed);
        this->R.set_state(CLOCKWISE, R_speed);
        break;
    }
  }

};

class UltrasonicSensor {
public:
  // left and right pins
  const int trig, echo;

  UltrasonicSensor(int trig, int echo)
  : trig(trig), echo(echo) {
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);
  }

  float distance() {
    digitalWrite(this->trig, LOW);
    delayMicroseconds(2);

    digitalWrite(this->trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(this->trig, LOW);

    return pulseIn(this->echo, HIGH) * 0.034 / 2.0;
  } 
};

Robot r{6, 7, 10, 8, 9, 11};
UltrasonicSensor us{2, 3};


// IR
const int ir_left = 4;
const int ir_right = 5;

void setup() {
  // put your setup code here, to run once:
  pinMode(ir_left, INPUT);
  pinMode(ir_right, INPUT);
  Serial.begin(9600);
}

void loop() {
  const float distance = us.distance();
  const int left = digitalRead(ir_left) == LOW; 
  const int right = digitalRead(ir_right) == LOW;


  if (left && !right) {
    r.L.set_state(CLOCKWISE, 5);
    r.R.set_state(CLOCKWISE, 255);
  } else if (!left && right) {
    r.L.set_state(CLOCKWISE, 255);
    r.R.set_state(CLOCKWISE, 5);
  } else if (distance >= 12 && distance <= 30) {
    r.move(FORWARD);
  } else if (distance < 7) {
    r.move(BACKWARD, 75);
  } else {
    r.move(STOP);    
  }
}
