
#include "VL53L1X_Interface.h"
#include "Adafruit_VL53L1X.h"
//#include "PC_Connection.h"



Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(XSHUT_PIN, IRQ_PIN);

uint8_t VL53L1X_mapping[16][16] = {
{128,136,144,152,160,168,176,184,  192,200,208,216,224,232,240,248},
{129,137,145,153,161,169,177,185,  193,201,209,217,225,233,241,249},
{130,138,146,154,162,170,178,186,  194,202,210,218,226,234,242,250},
{131,139,147,155,163,171,179,187,  195,203,211,219,227,235,243,251},
{132,140,148,156,164,172,180,188,  196,204,212,220,228,236,244,252},
{133,141,149,157,165,173,181,189,  197,205,213,221,229,237,245,253},
{134,142,150,158,166,174,182,190,  198,206,214,222,230,238,246,254},
{135,143,151,159,167,175,183,191,  199,207,215,223,231,239,247,255},

{127,119,111,103, 95, 87, 79, 71,   63, 55, 47, 39, 31, 23, 15,  7},
{126,118,110,102, 94, 86, 78, 70,   62, 54, 46, 38, 30, 22, 14,  6},
{125,117,109,101, 93, 85, 77, 69,   61, 53, 45, 37, 29, 21, 13,  5},
{124,116,108,100, 92, 84, 76, 68,   60, 52, 44, 36, 28, 20, 12,  4},
{123,115,107, 99, 91, 83, 75, 67,   59, 51, 43, 35, 27, 19, 11,  3},
{122,114,106, 98, 90, 82, 74, 66,   58, 50, 42, 34, 26, 18, 10,  2},
{121,113,105, 97, 89, 81, 73, 65,   57, 49, 41, 33, 25, 17,  9,  1},
{120,112,104, 96, 88, 80, 72, 64,   56, 48, 40, 32, 24, 16,  8,  0}};


Vl531X_control_t* control_g;
Vl531X_data_t* data_g;
//lets you know if you need to re-init
bool VL53L1X_status = false;

bool VL53L1X_interface_get_status(void){
  return VL53L1X_status;
}
//default init state.
void VL53L1X_interface_init(void){
  //could use new and a constructor but ehh already wrote it.
  control_g = (Vl531X_control_t*) realloc(control_g, sizeof(Vl531X_control_t));
  Serial.println(sizeof(Vl531X_control_t),HEX);
  data_g = (Vl531X_data_t*) realloc(data_g, sizeof(Vl531X_data_t));

  while ((control_g == NULL) || data_g == NULL){
    Serial.println("WE ARE COOKED MALLOC FAILED...");
  }

  for(uint32_t x = 0; x < MAX_X; x++){
   for(uint32_t y = 0; y < MAX_Y; y++){   
    data_g->time_since_startup_ms[x][y] = 0xDEADBEEF;
    data_g->distance[x][y] = 0;
   }
  }


  //Begin i2c wire
   Wire.begin();
   Wire.setClock(400000); // use 400 kHz I2C
   Serial.printf("i2c Clock : %d\n",Wire.getClock());
    //you can pass in 0x29, and &Wire normally but its cleanner rn, Also you can pass in debug true flag
       //if(!vl53.begin()){
   if(!vl53.begin(0x29, &Wire)){
    Serial.print(F("Error on init of VL sensor: "));
    Serial.println(vl53.vl_status);
    VL53L1X_status = false;
    return;
   }

   
   //Check the sensor status
  Serial.println(F("VL53L1X sensor OK!"));
  Serial.print(F("Sensor ID: 0x"));
  Serial.println(vl53.sensorID(), HEX);
  //Start the ranging
  if (! vl53.startRanging()) {
    Serial.print(F("Couldn't start ranging: "));
    Serial.println(vl53.vl_status);
    while (1)       delay(10);
  }
  Serial.println(F("Ranging started"));
  
  //For now we are going set a threshold for distance where
  //Low is 0 since we want to see all distances even the lowest
  //High Should be dynamically gotten at startup and then a bit of a offset to make certain that we ignore the floor (dont want it constantly triggering)
  //(could also add a piece of code if all values are near equal that we set the threshold to that avg value + some offset)
  //2 meeters or so
  //4 meters is total range so 2^16/2 = 2 meters
  uint16_t door_height = (1 << 15);
  uint16_t lowest_human_height = (1 << 14);
  //1 me
  uint16_t sensor_cutoff = door_height - lowest_human_height;
  //window being zero is values closer than sensor_cutoff
  uint16_t window = 0;
  vl53.VL53L1X_SetDistanceThreshold(sensor_cutoff,0,window,1);

  VL53L1X_interface_update();
/*
  //min size of roi is 4x4 // TODO have it check from the thing
  vl53.VL53L1X_SetROI(4,4);
  //because ROI is limited that means that we cannot have a pixel on the +2 or -2 edges so we need bound the 16x16 box to 12x12 total pixels
  //there are many different scanning methods that could be done (look up raster data structures) For us however we will use row as we can abuse
  //the delay effects as a method of detection of direction

  //Largest worry of using this method is that if a object moves fast enough we have aliasing as the scan is occuring (you move faster than the scan or past nyquist)
  //to calculate at what speed aliasing becomes a problem we can use the timebudget per pixel and the positon of where the next pixel will be 
  //(you dont have to move the center one pixel at a time you can make it jump to handle faster motion sacraficing fidelity)
  // You also have to consider the FOV of the sensor that it is viewing  at that time, (this will heavly rely on the hight of the person as closer objects will appear faster)
  //From this concept you will get time_budget

  //This code should probably verify current thresholds to make certain that it did work but eh

  // Valid timing budgets: 15, 20, 33, 50, 100, 200 and 500ms!
  vl53.setTimingBudget(TIME_BUDGET);
  Serial.print(F("Timing budget (ms): "));
  Serial.println(vl53.getTimingBudget());
  */
  VL53L1X_status = true;
}

void VL53L1X_interface_update(Vl531X_control_t* control){
  
   Serial.println((uint32_t)control,HEX);
    if(control->XY_count > MAX_X*MAX_Y){
    control->XY_count = 0;
    Serial.println("Dog you trying to buffer overflow me fr...?");
  }
  
  //first clear it away
  for(uint32_t x = 0; x < MAX_X; x++){
   for(uint32_t y = 0; y < MAX_Y; y++){   
    data_g->time_since_startup_ms[x][y] = 0xDEADBEEF;
    data_g->distance[x][y] = 0;
   }
  }

  vl53.VL53L1X_SetROI(control->ROI_X,control->ROI_Y);
  vl53.VL53L1X_SetDistanceMode(1);
  vl53.VL53L1X_SetInterMeasurementInMs(20);
  Serial.print(F("Timing budget to set (ms): "));
  Serial.println(control->time_budget);
  vl53.setTimingBudget(control->time_budget);
  Serial.print(F("Timing budget (ms): "));
  Serial.println(vl53.getTimingBudget());
  //finish with memcpy to global
  memcpy(control_g,control,sizeof(Vl531X_control_t));

}
//default case
void VL53L1X_interface_update(void){
  control_g->ROI_X = 15;
  control_g->ROI_Y = 4;
  control_g->time_budget = 15;
  control_g->XY_count = 3;
  
  //control_g->X[0] = 186;
 // control_g->X[1] = 191;
 // control_g->X[2] = 67;
 
  uint32_t i = 0;
  for(uint32_t x = 7; x < SENSOR_MAX_X; x+=15){
   for(uint32_t y = 1; y < SENSOR_MAX_Y; y+=4){ 

    control_g->X[i] = x;
    control_g->Y[i] = y;
    i++;
   }
  }
  control_g->XY_count = i;
  Serial.printf("XY_count : %d\n", control_g->XY_count);
  Serial.print(F("Timing budget to set (ms): "));
  Serial.println(control_g->time_budget);

  Serial.println((uint32_t)control_g,HEX);
  VL53L1X_interface_update(control_g);
}
/*
//default case
void VL53L1X_interface_update(void){
  control_g->ROI_X = 4;
  control_g->ROI_Y = 4;
  control_g->time_budget = 50;
  control_g->XY_count = MAX_X*MAX_Y;
  uint32_t i = 0;
  for(uint32_t x = 0; x < MAX_X; x++){
   for(uint32_t y = 0; y < MAX_Y; y++){ 

    control_g->X[i] = x;
    control_g->Y[i] = y;
    i++;
   }
  }
    Serial.print(F("Timing budget to set (ms): "));
  Serial.println(control_g->time_budget);

  Serial.println((uint32_t)control_g,HEX);
  VL53L1X_interface_update(control_g);
}
*/

void VL53L1X_interface_init(Vl531X_control_t* control){

  control_g = control;

  if(control_g->XY_count > MAX_X*MAX_Y){
    control_g->XY_count = 0;
    Serial.println("Dog you trying to buffer overflow me fr...?");
  }

}

//This is the process that will run and based on the control excuted.
void VL53L1X_interface_process(void){

uint8_t X,Y;
uint8_t a;  

  for(uint32_t i = 0; i < control_g->XY_count; i++){
  
    X = control_g->X[i];
    Y = control_g->Y[i];
    usleep(1000); //Sleep so we are in the ranging state
   vl53.VL53L1X_SetROICenter(VL53L1X_mapping[Y][X]);
   
  //vl53.VL53L1X_GetROICenter(&a);
  //Serial.printf("%d\n",a);
    //vl53.VL53L1X_GetROICenter(a);
    while (!vl53.dataReady()) {
      //vl53.VL53L1X_SetROICenter(VL53L1X_mapping[Y][X]);
      // vl53.VL53L1X_GetROICenter(&a);
      // Serial.printf("%d\n",a);
      // vl53.distance();
    }
   // vl53.distance();
    //usleep(20000);
    //while (!vl53.dataReady()) {
      
   // }
    

    
    data_g->distance[X][Y] = vl53.distance();
    vl53.clearInterrupt();
    data_g->time_since_startup_ms[X][Y] = millis();
    //Serial.printf("%04X %04X : %04X \n",X,Y,data_g->distance[X][Y]);
  }
}
void VL53L1X_interface_debug_print(void){
  //char buffer[255];
//sprintf(buffer, "%08X ", data_g->distance[x][y]);
//Serial.println(buffer);
  //for(uint32_t x = 0; x < MAX_X; x+=15){
   //for(uint32_t y = 0; y < MAX_Y; y+=5){
    static uint32_t previous = 0;
  for(uint32_t x = 7; x < SENSOR_MAX_X; x+=15){
   for(uint32_t y = 1; y < SENSOR_MAX_Y; y+=4){
  //for(uint32_t x = L_BOUND_X; x < H_BOUND_X; x++){
   //for(uint32_t y = L_BOUND_Y; y < H_BOUND_Y; y+distance+){
      //Serial.print(data_g->distance[x][y],HEX);
      
      Serial.printf("%04X %04d ", data_g->distance[x][y],data_g->time_since_startup_ms[x][y]-previous);
      previous = data_g->time_since_startup_ms[x][y];

    }
        Serial.printf("\n");
  }
      //Serial.print("\n\n\n");
}

//we are a bit lazy and doing it quick fix in future
void VL53L1X_interface_get_simple_data(uint16_t* data,uint32_t* size){

  uint32_t i = 0;
  for(uint32_t x = 7; x < SENSOR_MAX_X; x+=15){
   for(uint32_t y = 1; y < SENSOR_MAX_Y; y+=4){

      data[i] = data_g->distance[x][y];
      Serial.printf("%04X ", data_g->distance[x][y]);
      i++;

    }
  *size = i;
  }
}

