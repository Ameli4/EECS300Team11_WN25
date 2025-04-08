
#include "Adafruit_VL53L1X.h"

#include "VL53L1X_Interface.h"

//#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"

#include "PC_Connection.h"

#include "Scan_Logic.h"
#include "WirelessCommunication.h"
#include "Preferences.h"

void init_non_vol_storage();
void update_non_vol_count();
void update_button_count();

//bool VL53L1X_status = false;
//WiFiClient client;
volatile uint32_t count = 0;
volatile shared_int32 x;
Preferences nonVol;//used to store the count in nonvolatile memory

void setup() {

  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.print("setup() running on core ");
  Serial.println(xPortGetCoreID());
  //wifi_init();
  VL53L1X_interface_init();
  //TODO make better later but this will work
  init_wifi_task();
  init_non_vol_count();//initializes nonvolatile memory and retrieves latest count
  INIT_SHARED_VARIABLE(x, count);//init shared variable used to tranfer info to WiFi core
  /*
  if(!VL53L1X_interface_get_status()){
    //Setup to be server
    init_wifi_task();
     
  }*/

}


void loop() {
  //check_for_connection_process();
  VL53L1X_interface_process();
  scan_logic_process();
  count = scan_logic_get_count();
  update_button_count();//update shared variable x (shared with WiFi task)
  update_non_vol_count();//updates nonvolatile count 
  Serial.println(count);
  VL53L1X_interface_debug_print();
 
  LOCK_SHARED_VARIABLE(x);
  x.value = count;
  UNLOCK_SHARED_VARIABLE(x);   


}

void init_non_vol_count()
{
  nonVol.begin("nonVolData", false);//Create a “storage space” in the flash memory called "nonVolData" in read/write mode
  count = nonVol.getUInt("count", 0);//attempts to retrieve "count" from nonVolData, sets it 0 if not found
}

//updates nonvolatile memery with lates value of count
void update_non_vol_count()
{
  nonVol.putUInt("count", count);//write count to nonvolatile memory
}

//example code that updates a shared variable (which is printed to server)
//under the hood, this implementation uses a semaphore to arbitrate access to x.value
void update_button_count()
{
  //minimized time spend holding semaphore
  LOCK_SHARED_VARIABLE(x);
  x.value = count;
  UNLOCK_SHARED_VARIABLE(x);   
  
}
