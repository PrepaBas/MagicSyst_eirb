#include "Arduino.h"
#include "table.h"
#include <vector>
#include "rank.h"
#include "basic_strat.h"
#include "advanced_movement.h"
#include "table.h"
#include <ESP32Servo.h>

#define MAP_DIAGONAL_SQUARED 13000000 // 2000**2 + 3000**2
extern struct position position;

Macro::Macro(Function in_function, struct position in_start, struct position in_end, uint16_t in_runtime, uint8_t in_color, int in_scale)
{
  function = in_function;
  start = in_start;
  end = in_end;
  runtime = in_runtime;
  color = in_color;
  scale = in_scale;
}

int Macro::rank()
{
  int rank = 0;
  float delta_x = (position.x - Macro::start.x);
  float delta_y = (position.y - Macro::start.y);
  float absolute_travel = delta_x*delta_x+delta_y*delta_y;
  int percentage_of_map = 100 - absolute_travel / MAP_DIAGONAL_SQUARED * 100; // add 0 - 100 to rank
  rank += percentage_of_map;
  rank = Macro::scale * rank;
  return rank;
}

std::vector<Macro> begin_macro() {
  std::vector<Macro> macros;
  macros.reserve(4);  // optional but efficient

  macros.emplace_back(Macro{deposit_bl_cans,   {300, 30}, {50, 50}, 5000, 0, 5});
  macros.emplace_back(Macro{deposit_bl_cans_2, {30, 250}, {50, 50}, 5000, 0, 5});
  macros.emplace_back(Macro{deposit_tl_cans,   {30, 3000}, {50, 50}, 5000, 0, 5});
  macros.emplace_back(Macro{go_home,           {3, 1000}, {50, 50}, 5000, 0, 0});

  return macros;
}

void dispatchTaskcode(void *parameters)
{
  vTaskDelay(pdMS_TO_TICKS(1000));
  int *robot_stop_ptr = (int *)parameters;
  //xTaskCreate(baniere, "currentTask", 10000, NULL, 1, &currentTask);
  
  //vTaskDelay(pdMS_TO_TICKS(30000));

  /* Creation of function structures */
  std::vector<Macro> list_macro = begin_macro();

  /* Loop while actions are left */
  while (!list_macro.empty())
  {
    /* research max rank */
    int best_rank = list_macro[0].rank();
    int index = 0;
    int position_macro = 0;
    for (Macro macro : list_macro)
    {
      int macro_rank = macro.rank();
      if (macro_rank > best_rank)
      {
        best_rank = macro_rank;
        position_macro = index;
      }   
      index++;
    }
    Macro best_macro = list_macro.at(position_macro);


    xTaskCreate(best_macro.function, "currentTask", 10000, NULL, 1, &currentTask);

    while (eTaskGetState(currentTask) != eDeleted)
    { // wait end of task (natural or unatural)
      vTaskDelay(pdMS_TO_TICKS(50));
    }
    /*
    while (*robot_stop_ptr == 1)
    { // wait
      vTaskDelay(pdMS_TO_TICKS(50));
    }*/
    list_macro.erase(list_macro.begin() + position_macro);
  }

  vTaskDelete(NULL);
}

