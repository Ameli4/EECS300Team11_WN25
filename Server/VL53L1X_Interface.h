//#param once
#include <cstdint>
#include "config.h"
//can call to see current settings.

typedef struct Vl531X_control{

  //bit 0 go
  uint16_t control;
  uint16_t ROI_X;
  uint16_t ROI_Y;
  uint16_t time_budget;
  uint32_t XY_count; //MAX_X*MAX_Y
  uint8_t X[MAX_X*MAX_Y];
  uint8_t Y[MAX_X*MAX_Y];

} Vl531X_control_t __attribute__((packed));

//returned data.
typedef struct Vl531X_data{

//Returns same size every time in pixel order.
//If a certain pixel isnt used the time will be 0xDEADBEEF and the distance value to 0
uint32_t time_since_startup_ms[MAX_X][MAX_Y];
uint16_t distance[MAX_X][MAX_Y];

}Vl531X_data_t;
bool VL53L1X_interface_get_status(void);
void VL53L1X_interface_update(Vl531X_control_t* control);
void VL53L1X_interface_update(void);
void VL53L1X_interface_init(void);
void VL53L1X_interface_process(void);
void VL53L1X_interface_debug_print(void);
void VL53L1X_interface_get_simple_data(uint16_t* data,uint32_t* size);

//void VL53L1X_interface_get_ptrs(Vl531X_control_t* control, Vl531X_data_t* data);