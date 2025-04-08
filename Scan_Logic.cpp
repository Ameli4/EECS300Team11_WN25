#include "Scan_Logic.h"
#include "VL53L1X_Interface.h"
#include "HardwareSerial.h"
#include "WirelessCommunication.h"
//we know that there is a 16x16 grid
//for basic we want to make 9 scan zones of activity (4x4x9 => (4x4)x3x3 since it divides into 12x12)
//When we generate that data we should make a mask and a mapping to a smaller region


//Basic scan pattern in default stat

//Read chunk is just the location

int32_t is_person(uint16_t data){

  int32_t return_value = NO_PERSON;

  if (data < MAX_VALUE){
      return_value = PERSON;
  }

  return return_value;

}

//PAY ATTENTION DO NOT ASSUME INDEX WILL BE VALID ALWAYS
int32_t index_of_max_value(uint16_t* data_p, uint32_t size){

uint32_t max_value = 0;

int32_t max_index = NO_PERSON;
for (uint32_t i = 0; i < size; i++) {
      //Checks to see if its valid person if so it will attempt to find max
        if (data_p[i] > max_value & (is_person(data_p[i]) == PERSON)) {
            max_value = data_p[i];
            max_index = i;
        }
    }
    //index
    return max_index;

}
int32_t count_in_room = 0;

int32_t scan_logic_get_count(void){
  if(count_in_room < 0) {
    return 0;
  }
  return count_in_room;
}
void scan_logic_process(void){

  //Vl531X_control_t* control;
  //Vl531X_data_t* data;

  //Extra space just in case we want to expand it.
  uint16_t data_arr[16];
  uint32_t size = 0;


  static bool started = false; //is there someone activly in the area (if not -1 of index_of_max_value)
  static bool ended = false;
  static int32_t start_index = NO_PERSON; //index of where it start
  static int32_t end_index = NO_PERSON;
  Serial.print("Count: ");
  Serial.println(count_in_room);

  VL53L1X_interface_get_simple_data(data_arr,&size);

  //Serial.printf("%04X %04X %04X \n", data_arr[0],data_arr[1],data_arr[2],data_arr[3]);


  //State machine
  int32_t temp_index = index_of_max_value(data_arr, size);

  //If something exists and we are 
  //if ((temp_index != NO_PERSON) & (start_count == false)){
  if (temp_index != NO_PERSON){
      started = true;
  }

  if (started == true){
    if (start_index == NO_PERSON){
      //GET THE FIRST VALUE INDEXED BY THERE BEING A PERSON (RUN ONCE CLEAR LATER)
      start_index = temp_index;

    }
    //If a person is being activly detected and we have its starting index
    else{
      
      if(ended == false){
        if((start_index < size/2) & (is_person(data_arr[size-1]) == PERSON)){

          count_in_room++;
          ended = true;
          end_index = size-1;
          Serial.print("Count: ");
          Serial.println(count_in_room);
        }
        else if((start_index >= size/2) & (is_person(data_arr[0]) == PERSON)){

          count_in_room--;
          ended = true;
          end_index = 0;
          Serial.print("Count: ");
          Serial.println(count_in_room);
        }
      }
      else{
        if(temp_index == NO_PERSON){
          started = false; //is there someone activly in the area (if not -1 of index_of_max_value)
          ended = false;
          start_index = NO_PERSON; //index of where it start
          end_index = NO_PERSON;
        }
      }
    }
  }
  // Serial.print("Started: ");
  // Serial.println(started);
  // Serial.print("Start Index: ");
  // Serial.println(start_index);
  // Serial.print("Ended: ");
  // Serial.println(ended);
  // Serial.print("End Index: ");
  // Serial.println(end_index);
  // Serial.print("temp_index: ");
  // Serial.println(temp_index);

}


