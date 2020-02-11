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
#include <stdlib.h>

#include "contiki.h"
#include "dev/serial-line.h"
#include "dev/bme280/bme280.h"
#include "dev/bme280/bme280-sensor.h"
#include "dev/zoul-sensors.h"
#include "sys/node-id.h"
#include "dev/radio.h"
#include "net/netstack.h"
#include "net/packetbuf.h"

#include "os/dev/ble-hal.h"

PROCESS(toggle_shell, "toggle shell");
AUTOSTART_PROCESSES(&toggle_shell);

void substring(char s[], char sub[], int p, int l) {
   int c = 0;

   while (c < l) {
      sub[c] = s[p+c-1];
      c++;
   }
   sub[c] = '\0';
}


PROCESS_THREAD(toggle_shell, ev, data)
{

        PROCESS_BEGIN();

        SENSORS_ACTIVATE(cc2538_temp_sensor);
        SENSORS_ACTIVATE(vdd3_sensor);
        int temp;
        int batteryval;
        radio_value_t value;
        int8_t rssi = 0;
        int tx_level, lqi, cha, cha_min, cha_max, pan_id, rx_mode, tx_mode, tx_min,tx_max;
        char strval[50];
        for(;;) {
                PROCESS_WAIT_EVENT_UNTIL(ev == serial_line_event_message && data != NULL);
                if(strcmp("READTEMP",(char *)data)==0){
                        temp=cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
                        printf("%d\n",temp);
                }
                if(strcmp("READBAT",(char *)data)==0){
                        batteryval=vdd3_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
                        printf("%d\n",batteryval);
                }
                if(strncmp("SETPANID=",(char *)data,strlen("SETPANID="))==0){
                        substring((char *)data,strval,strlen("SETPANID=")+1,strlen(data));
                        pan_id = atoi(strval);                        
                        if(NETSTACK_RADIO.set_value(RADIO_PARAM_PAN_ID, pan_id) ==
                                        RADIO_RESULT_OK) {
                               printf("\"Node_id\": \"%d\",\"Command\": \"SetPanid\",\"Value\": \"%d\",\"Result\": \"Success\"\n",node_id,pan_id);
                        }
                        else{
                                printf("\"Node_id\": \"%d\",\"Command\": \"SetPanid\",\"Value\": \"%d\",\"Result\": \"Failed\"\n",node_id,pan_id);
                        }
                }
                if(strncmp("SETCH=",(char *)data,strlen("SETCH="))==0){
                        substring((char *)data,strval,strlen("SETCH=")+1,strlen(data));
                        cha = atoi(strval);
                        if(NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, cha) ==
                                        RADIO_RESULT_OK) {
                               printf("\"Node_id\": \"%d\",\"Command\": \"SetCh\",\"Value\": \"%d\",\"Result\": \"Success\"\n",node_id,cha);
                        }
                        else{
                                printf("\"Node_id\": \"%d\",\"Command\": \"SetCh\",\"Value\": \"%d\",\"Result\": \"Failed\"\n",node_id, cha);
                        }
                }
                if(strncmp("SETTX=",(char *)data,strlen("SETTX="))==0){
                        substring((char *)data,strval,strlen("SETTX=")+1,strlen(data));
                        tx_level = atoi(strval);
                        if(NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, tx_level) ==
                                        RADIO_RESULT_OK) {
                               printf("\"Node_id\": \"%d\",\"Command\": \"SetTX\",\"Value\": \"%d\",\"Result\": \"Success\"\n",node_id,tx_level);
                        }
                        else{
                                printf("\"Node_id\": \"%d\",\"Command\": \"SetTX\",\"Value\": \"%d\",\"Result\": \"Failed\"\n",node_id,tx_level);
                        }
                }

                if(strcmp("READSENVAL",(char *)data)==0){
                        rssi = 0;
                        lqi=0;
                        cha=0;cha_min=0;cha_max=0;pan_id=0;rx_mode=0;tx_mode=0;tx_min=0;tx_max=0;
                        if(NETSTACK_RADIO.get_value(RADIO_PARAM_RSSI, &value) ==
                                        RADIO_RESULT_OK) {
                                rssi = value;
                        }
                        if(NETSTACK_RADIO.get_value(PACKETBUF_ATTR_LINK_QUALITY, &value) ==
                                        RADIO_RESULT_OK) {
                               lqi = value;
                        }
                        if(NETSTACK_RADIO.get_value(RADIO_PARAM_CHANNEL, &value) ==
                                        RADIO_RESULT_OK) {
                               cha = value;
                        }
                        if(NETSTACK_RADIO.get_value(RADIO_CONST_CHANNEL_MIN, &value) ==
                                        RADIO_RESULT_OK) {
                               cha_min = value;
                        }
                        if(NETSTACK_RADIO.get_value(RADIO_CONST_CHANNEL_MAX, &value) ==
                                        RADIO_RESULT_OK) {
                               cha_max = value;
                        }
                        if(NETSTACK_RADIO.get_value(RADIO_PARAM_PAN_ID, &value) ==
                                        RADIO_RESULT_OK) {
                               pan_id = value;
                        }
                        if(NETSTACK_RADIO.get_value(RADIO_PARAM_RX_MODE, &value) ==
                                        RADIO_RESULT_OK) {
                               rx_mode = value;
                        }
                        if(NETSTACK_RADIO.get_value(RADIO_PARAM_TX_MODE, &value) ==
                                        RADIO_RESULT_OK) {
                               tx_mode = value;
                        }
                        temp=cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
                        batteryval=vdd3_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
                        NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &tx_level);
                        if(NETSTACK_RADIO.get_value(RADIO_CONST_TXPOWER_MIN, &value) ==
                                        RADIO_RESULT_OK) {
                               tx_min = value;
                        }
                        if(NETSTACK_RADIO.get_value(RADIO_CONST_TXPOWER_MAX, &value) ==
                                        RADIO_RESULT_OK) {
                               tx_max = value;
                        }
                        printf("\"NodeId\": \"%d\",\"Temperature\": \"%d\",\"BatteryVal\": \"%d\",\"RssiVal\": \"%d\",\"TXLevel\": \"%d\",\"TXMinVal\": \"%d\",\"TXMaxVal\": \"%d\",\"LqiVal\": \"%d\",\"ChaVal\": \"%d\",\"MinChaVal\": \"%d\",\"MaxChaVal\": \"%d\",\"PanIdVal\": \"%d\",\"RxModeVal\": \"%d\",\"TxModeVal\": \"%d\"\n",node_id,temp,batteryval,rssi,tx_level,tx_min,tx_max,lqi,cha,cha_min,cha_max, pan_id, rx_mode, tx_mode);
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
                                   