
#include <ESP32Servo.h>
#include "NewPing.h"
#include "observer.h"
#include "advanced_movement.h"

void stop_robot(int *robot_stop_ptr)
{
  *robot_stop_ptr = 1;
  vTaskSuspend(currentTask);
  remaining_steps = 200;
  new_position();
  vTaskDelay(pdMS_TO_TICKS(5000));
  *robot_stop_ptr = 0;
}

void securityTaskcode(void *parameters)
{
  NewPing fc(17, 17, 200); // front center
  NewPing fr(17, 17, 200); // front right
  NewPing fl(17, 17, 200); // front left
  NewPing rr(17, 17, 200); // rear right
  NewPing rl(17, 17, 200); // rear left
  NewPing cn(17, 17, 200); // cans near
  NewPing cf(17, 17, 200); // cans far

  int *robot_stop_ptr = (int *)parameters;

  for (;;)
  {
    unsigned long fc_distance = fc.ping_cm();
    unsigned long fr_distance = fr.ping_cm();
    unsigned long fl_distance = fl.ping_cm();
    unsigned long rr_distance = rr.ping_cm();
    unsigned long rl_distance = rl.ping_cm();
    unsigned long cn_distance = cn.ping_cm();
    unsigned long cf_distance = cf.ping_cm();
    vTaskDelay(pdMS_TO_TICKS(10));
    // robot.motors.param.max_speed = robot.motors.param.min_speed+(ms++%50000);
  }
}
