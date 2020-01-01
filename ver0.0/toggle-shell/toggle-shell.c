/*
 * Copyright (c) 2010, Swedish Institute of Computer Science
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include <stdio.h>
#include <string.h>

#include "contiki.h"
#include "dev/serial-line.h"
#include "dev/bme280/bme280.h"
#include "dev/bme280/bme280-sensor.h"
#include "dev/zoul-sensors.h"

PROCESS(toggle_shell, "toggle shell");
AUTOSTART_PROCESSES(&toggle_shell);

PROCESS_THREAD(toggle_shell, ev, data)
{

  PROCESS_BEGIN();

  SENSORS_ACTIVATE(cc2538_temp_sensor);
  int temp;
  for(;;) {
    PROCESS_WAIT_EVENT_UNTIL(ev == serial_line_event_message && data != NULL);
    if(strcmp("READTEMP",(char *)data)==0){
        temp=cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
        printf("%d\n",temp);
    }
    else if(strcmp("EXIT",(char *)data)==0){
        printf("exiting now!!!\n");
        break;
    }
    else{
        //printf("Query \"%s\" \n", (char *)data );
    }
  }


  PROCESS_END();
}
