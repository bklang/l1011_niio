/*********************************************************************
*    debug_inputs.c
*    Reads and prints the input state of all 96 lines
*********************************************************************/
#include <stdio.h>
#include <string.h>
#include "NIDAQmx.h"

#define DAQmxErrChk(functionCall) { if( DAQmxFailed(error=(functionCall)) ) { goto Error; } }

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

  // As a giant bitfield
  printf("012345670123456701234567012345670123456701234567012345670123456701234567012345670123456701234567\n");
  for(i=0; i<num_chans; i++) {
     for(j=0; j<8; j++) {
       printf("%X", !((r_data[i] >> j) & 1));
     }
  }
  printf("\n");

  // By port
  for(i=0; i<num_chans; i++) {
     printf("Port %d: 0x%X\n", i, r_data[i]);
  }
  printf("\n");

  
  printf("Read %d samples\n", read);
  printf("-----------------------------------------------------\n");
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
