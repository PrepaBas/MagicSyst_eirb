#include "Arduino.h"
#include "table.h"
#include <vector>
#include "rank.h"
#include "basic_strat.h"

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
  return 3;
}

std::vector<Macro> begin_macro()
{
  std::vector<Macro> macros;
  Macro macro(deposit_bl_cans, (struct position){30, 30}, (struct position){50, 50}, 5000, 0, 5);
  macros.push_back(macro);
  macro = {deposit_bl_cans_2, (struct position){30, 30}, (struct position){50, 50}, 5000, 0, 5};
  macros.push_back(macro);
  macro = {deposit_tl_cans, (struct position){30, 30}, (struct position){50, 50}, 5000, 0, 5};
  macros.push_back(macro);
  macro = {go_home, (struct position){30, 30}, (struct position){50, 50}, 5000, 0, 5};
  macros.push_back(macro);
  return macros;
}

void dispatchTaskcode(void *parameters)
{
  vTaskDelay(pdMS_TO_TICKS(3000));
  int *robot_stop_ptr = (int *)parameters;
  /* Creation of function structures */
  std::vector<Macro> list_macro = begin_macro();

  /* Loop while actions are left */
  while (!list_macro.empty())
  {
    /* research max rank */
    Macro best_macro = list_macro[0];
    int best_rank = best_macro.rank();
    int position_macro = 0;
    for (Macro macro : list_macro)
    {
      int macro_rank = macro.rank();
      position_macro++;
      if (macro_rank > best_rank)
      {
        best_macro = macro;
        best_rank = macro_rank;
      }
    }
    position_macro--;
    xTaskCreate(best_macro.function, "currentTask", 10000, NULL, 1, &currentTask);

    while (eTaskGetState(currentTask) != eDeleted)
    { // wait end of task (natural or unatural)
      vTaskDelay(pdMS_TO_TICKS(50));
    }
    while (*robot_stop_ptr == 1)
    { // wait
      vTaskDelay(pdMS_TO_TICKS(50));
    }
    list_macro.erase(list_macro.begin() + position_macro);
  }

  vTaskDelete(NULL);
}

