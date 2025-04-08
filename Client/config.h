
/*
#define GATE_3 10
#define GATE_2 0
#define GATE_1 0
#define GATE_0 1
*/
#define GATE_3 192
#define GATE_2 168
#define GATE_1 68
#define GATE_0 1
//192.168.68.63
#define IP_3 GATE_3
#define IP_2 GATE_2
#define IP_1 GATE_1
#define IP_0 128

#define PORT_USED 60000

//you have to keep the "" for SSID, PASSWORD, and HOSTNAME.
//#define SSID_STR "NETGEAR37"
//#define PASSWORD_STR "jollymoon093"

#define SSID_STR "YASAM"
#define PASSWORD_STR "oscarathome"

#define HOSTNAME "ESP32-VL53l1x"

#define TIMEOUT 3000
//L and H are low and high bound respectivly
#define L_BOUND_X 1
#define H_BOUND_X 13

#define L_BOUND_Y 1
#define H_BOUND_Y 13

//#define MAX_X (H_BOUND_X - L_BOUND_X)
//#define MAX_Y (H_BOUND_Y - L_BOUND_Y)

#define SENSOR_MAX_X 16
#define SENSOR_MAX_Y 16
#define MAX_X SENSOR_MAX_X
#define MAX_Y SENSOR_MAX_Y
//SET THE PINS OF WHERE IT IS HOOKED UP
//#define IRQ_PIN 3 //34 //39
//#define XSHUT_PIN 2 //35 //34
#define IRQ_PIN 39 //34 //39
#define XSHUT_PIN 34 //35 //34
//in ms
#define TIME_BUDGET 50

