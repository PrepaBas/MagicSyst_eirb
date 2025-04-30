void securityTaskcode(void* parameters){
    /* vTaskDelay(pdMS_TO_TICKS(4500));
    int* robot_stop_ptr = (int*)parameters;
    *robot_stop_ptr = 1;
    vTaskSuspend(currentTask);
    remaining_steps = 200;
    new_position();
    vTaskDelay(pdMS_TO_TICKS(5000));
    *robot_stop_ptr = 0; */
    for(;;){
      vTaskDelay(pdMS_TO_TICKS(10));
      //robot.motors.param.max_speed = robot.motors.param.min_speed+(ms++%50000);
    }
  }
  