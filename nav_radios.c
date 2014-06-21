/*********************************************************************
*    nav_radios.c
*    Reads the current frequency from the two VHF NAV radios
*********************************************************************/
#include <stdio.h>
#include <string.h>
#include "NIDAQmx.h"

#define DAQmxErrChk(functionCall) { if( DAQmxFailed(error=(functionCall)) ) { goto Error; } }

#define DAQ_DEV "Dev1"

#define NAV1_TENS DAQ_DEV "/line6:8"
#define NAV1_ONES DAQ_DEV "/line9:13"
#define NAV1_TENTHS DAQ_DEV "/line14:18"
#define NAV1_HUNTHS DAQ_DEV "/line19:20"

#define NAV2_TENS DAQ_DEV "/line27:29"
#define NAV2_ONES DAQ_DEV "/line30:34"
#define NAV2_TENTHS DAQ_DEV "/line35:36," DAQ_DEV "/line39:41"
#define NAV2_HUNTHS DAQ_DEV "/line42:43"

// For use outside of channels
int nav1_bitfield[15] = { 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20};
int nav2_bitfield[15] = {27,28,29,30,31,32,33,34,35,36,39,40,41,42,43};

int bit_read(uInt32 *r_data, int pin);
int swcode2int(int code);
float read_nav(uInt32 *r_data, int *bitfield);

int main (int argc, char *argv[])
{
  // Task parameters
  int32        error = 0;
  TaskHandle   taskHandle = 0;
  int32        i,j,tmp;
  const uInt32 num_chans = 12;
  char         errBuff[2048];

  // Read parameters
  uInt32        r_data [num_chans];
  int32        read;

  memset(r_data, 0, sizeof(r_data));

  printf("Creating DI channels for %d channels...", num_chans);
  fflush(stdout);
  // Create Digital Input (DI) Task and Channel
  DAQmxErrChk (DAQmxCreateTask ("", &taskHandle));

/*
  // NAV1
  DAQmxErrChk (DAQmxCreateDIChan(taskHandle,NAV1_TENS,"NAV1_TENS",DAQmx_Val_ChanForAllLines));
  DAQmxErrChk (DAQmxCreateDIChan(taskHandle,NAV1_ONES,"NAV1_ONES",DAQmx_Val_ChanForAllLines));
  DAQmxErrChk (DAQmxCreateDIChan(taskHandle,NAV1_TENTHS,"NAV1_TENTHS",DAQmx_Val_ChanForAllLines));
  DAQmxErrChk (DAQmxCreateDIChan(taskHandle,NAV1_HUNTHS,"NAV1_HUNTHS",DAQmx_Val_ChanForAllLines));

  //NAV2
  DAQmxErrChk (DAQmxCreateDIChan(taskHandle,NAV2_TENS,"NAV2_TENS",DAQmx_Val_ChanForAllLines));
  DAQmxErrChk (DAQmxCreateDIChan(taskHandle,NAV2_ONES,"NAV2_ONES",DAQmx_Val_ChanForAllLines));
  DAQmxErrChk (DAQmxCreateDIChan(taskHandle,NAV2_TENTHS,"NAV2_TENTHS",DAQmx_Val_ChanForAllLines));
  DAQmxErrChk (DAQmxCreateDIChan(taskHandle,NAV2_HUNTHS,"NAV2_HUNTHS",DAQmx_Val_ChanForAllLines));
*/

  DAQmxErrChk (DAQmxCreateDIChan(taskHandle,"Dev1/port0",NULL,DAQmx_Val_ChanForAllLines));
  DAQmxErrChk (DAQmxCreateDIChan(taskHandle,"Dev1/port1",NULL,DAQmx_Val_ChanForAllLines));
  DAQmxErrChk (DAQmxCreateDIChan(taskHandle,"Dev1/port2",NULL,DAQmx_Val_ChanForAllLines));
  DAQmxErrChk (DAQmxCreateDIChan(taskHandle,"Dev1/port3",NULL,DAQmx_Val_ChanForAllLines));
  DAQmxErrChk (DAQmxCreateDIChan(taskHandle,"Dev1/port4",NULL,DAQmx_Val_ChanForAllLines));
  DAQmxErrChk (DAQmxCreateDIChan(taskHandle,"Dev1/port5",NULL,DAQmx_Val_ChanForAllLines));
  DAQmxErrChk (DAQmxCreateDIChan(taskHandle,"Dev1/port6",NULL,DAQmx_Val_ChanForAllLines));
  DAQmxErrChk (DAQmxCreateDIChan(taskHandle,"Dev1/port7",NULL,DAQmx_Val_ChanForAllLines));
  DAQmxErrChk (DAQmxCreateDIChan(taskHandle,"Dev1/port8",NULL,DAQmx_Val_ChanForAllLines));
  DAQmxErrChk (DAQmxCreateDIChan(taskHandle,"Dev1/port9",NULL,DAQmx_Val_ChanForAllLines));
  DAQmxErrChk (DAQmxCreateDIChan(taskHandle,"Dev1/port10",NULL,DAQmx_Val_ChanForAllLines));
  DAQmxErrChk (DAQmxCreateDIChan(taskHandle,"Dev1/port11",NULL,DAQmx_Val_ChanForAllLines));
  printf("done.\n");

  // Start Task (configure channel)
  printf("Starting task...");
  fflush(stdout);
  DAQmxErrChk (DAQmxStartTask (taskHandle));
  printf("done.\n");

while( TRUE ) {
  printf("Reading from channels\n");
  // Read from channel
  DAQmxErrChk (DAQmxReadDigitalU32(taskHandle,1,1.0,DAQmx_Val_GroupByChannel,r_data,num_chans,&read,NULL));

  printf("NAV1: %f\n", read_nav(r_data, nav1_bitfield));
  printf("NAV2: %f\n", read_nav(r_data, nav2_bitfield));
  
  sleep( 1 );
}

Error:

  if (DAQmxFailed (error))
     DAQmxGetExtendedErrorInfo (errBuff, 2048);

  if (taskHandle != 0)
  {
     DAQmxStopTask (taskHandle);
     DAQmxClearTask (taskHandle);
  }

  if (error)
	  printf ("DAQmx Error %d: %s\n", error, errBuff);

  return 0;
}


int bit_read(uInt32 *r_data, int pin) {

  int port, bit;
  port = pin / 8;
  bit  = !(( r_data[port] >> (pin % 8)) & 1);
  //printf("Reading pin %d: Port %d, bit %d: %d (%X)\n", pin, port, pin % 8, bit, r_data[port]);
  return bit;
}

int swcode2int(int code) {
  // Taken from the "2 out of 5 switch code"
  // Pins: A,B,C,D,E
  // 0: 01001
  // 1: 11000
  // 2: 10100
  // 3: 01100
  // 4: 01010
  // 5: 00110
  // 6: 00101
  // 7: 00011
  // 8: 10010
  // 9: 10001
  int num_map[10] = {18, 3, 5, 6, 10, 12, 20, 24, 9, 17};
  int real;

  int i;
  for (i=0; i<=9; i++) {
   if (num_map[i] == code) {
     real = i;
   }
  }
  return real;
}

float read_nav(uInt32 *r_data, int *bitfield) {
  int i, swcode = 0;
  float nav_freq = 0.0;

  // Tenths, using pins A, C, E
  swcode += bit_read(r_data, bitfield[0]) << 0;
  swcode += bit_read(r_data, bitfield[1]) << 1;
  swcode += bit_read(r_data, bitfield[2]) << 4;
  nav_freq = swcode2int(swcode) * 10;

  swcode = 0;
  for (i = 0; i < 5; i++) {
    // Ones starts at bitfield[3]
    swcode += bit_read(r_data, bitfield[i+3]) << i;
  }
  nav_freq += swcode2int(swcode) * 1;

  swcode = 0;
  for (i = 0; i < 5; i++) {
    // Tenths starts at bitfield[8]
    swcode += bit_read(r_data, bitfield[i+8]) << i;
  }
  nav_freq += (float)swcode2int(swcode) / 10;

  swcode = 0;
  // Hundredths starts at bitfield[13], using pins B, C
  swcode += bit_read(r_data, bitfield[13]) << 1;
  swcode += bit_read(r_data, bitfield[14]) << 2;
  switch(swcode) {
  case 2:
    nav_freq += 0.00;
    break;
  case 4:
    nav_freq += 0.05;
    break;
  default:
    break;
  }
  return nav_freq + 100;
}
