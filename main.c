/* Add Header Files */

#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include "HX711.h"
#include "uart.h"
#include "millis.h"
/* Add Function Prototypes */
double digitalSmooth(double rawIn, double *sensSmoothArray);

//set stream pointer
FILE usart_str = FDEV_SETUP_STREAM(USART_Transmit, USART_Receive, _FDEV_SETUP_RW);

/* Add Variables */
double current_reading, current_weight_128, current_weight_32, adjustment_weight_0 = 0, system_lock_weight_array[5] = {0};
int filterSamples = 20;
float ptr[20];

/* Add Constants here */

/* Add function Definations here */


int main(void)
{
    /// Setup
    USART_Init(MYUBRR);
    millis_init();
    sei();
    stdin=stdout=&usart_str;
    HX711_init(128);
    //Averagebegin(average);
    HX711_set_scale(1.f/*242300.88*/);
    //HX711_set_offset(0.f);
    HX711_set_gain(128);
    HX711_tare(10);
    double tare_point_128 = HX711_get_offset();
    double calibration_128 = 222138.f;//220235.f;
    _delay_ms(500);
    // HX711_set_gain(32);
    // HX711_tare(10);
    // double tare_point_32 = HX711_get_offset();
    // double calibration_32 = 55676.f;
    // float change_rate = 0.995;

    double derivative = 0.0, ddrivative = 0.0, change_rate = 0.00100;
    double current_weight_128_dc, previous_weight_128, system_weight, offset_weight, pprevious_weight_128;
    millis_t time_mid, /*time_end,*/ time_start = millis();

    for(int i=0;i<10;i++){
        current_weight_128 = HX711_read_average(10) - tare_point_128;
        current_weight_128 = current_weight_128/calibration_128;
        if(i==8){
            pprevious_weight_128 = current_weight_128;
        }
    }
    previous_weight_128 = current_weight_128;
    bool trend = false, frozen_w = false, system_lock = false;
    double sum_weight = 0.0;
    int count = 0;
    /// Main Loop
    while(1)
    {   
        current_weight_128 = HX711_read_average(10) - tare_point_128;
        current_weight_128 = current_weight_128/calibration_128;
    //    current_weight_128 = sum_weight/average;
        time_mid = millis();
        derivative = (current_weight_128 - previous_weight_128);//(time_mid - time_start)*100000;
        ddrivative = current_weight_128 + pprevious_weight_128 - 2*previous_weight_128;
        time_start = time_mid;
        pprevious_weight_128 = previous_weight_128;
        previous_weight_128 = current_weight_128;
        fprintf(stdout,"S Deri = %.5f", derivative);
        printf(" , ");
        fprintf(stdout,"D Deri = %.5f", ddrivative);
        printf(" , ");
        fprintf(stdout,"%.5f", current_weight_128);
        printf(" , ");
        if(-change_rate < derivative && derivative < change_rate){
            trend = true;
            if(frozen_w == false /*&& current_weight_128 < 0.0050*/){
                offset_weight = current_weight_128;
            }
            else if(frozen_w == true /*&& current_weight_128 > 0.00050*/){
                offset_weight = offset_weight;
            }
            fprintf(stdout,"offset_weight = %.5f", offset_weight);
            printf(" , ");

        }
        else if(derivative >= change_rate){
            system_lock = true;
            frozen_w = true;
            fprintf(stdout,"frozen_weight = %.5f", offset_weight);
            printf(" , ");
            system_lock = true;
            /*for(int i=0;i<5;i++){
                system_lock_weight_array[i] = HX711_read_average(10) - tare_point_128;
                system_lock_weight_array[i] = system_lock_weight_array[i]/calibration_128;
                fprintf(stdout,"array = %.5f", system_lock_weight_array[i]);
                printf(" , ");
            }*/
            trend = false;
        }
        else{
            frozen_w = false;
            fprintf(stdout,"frozen_status = %.5f", offset_weight);
            printf(" , ");
            system_lock = false;
            trend = false;
        }
        if(current_weight_128 < 0.00050 && current_weight_128 > -0.00050){
            current_weight_128_dc = 0.0;
        }
        else /*if(current_weight_128 < -0.00050 && current_weight_128 > 0.00050 )*/{
            current_weight_128_dc = current_weight_128 - offset_weight;
        }
        if(frozen_w == true && trend == false){
            count++;
            fprintf(stdout,"count = %d", count);
            printf(" , ");
            //if(count == 2){
                system_weight = current_weight_128_dc;
            //}
        }
        else if(frozen_w == false){
            count = 0;
            system_weight = current_weight_128_dc;
        }
        //if(system_lock == true && trend == false){
          //  system_weight = current_weight_128;
            //fprintf(stdout,"System Lock = %.5f", current_weight_128_dc);
            //printf(" System Lock trend false");
        //}

        fprintf(stdout,"%.5f", current_weight_128);
        printf(" , ");
        fprintf(stdout,"%.5f", current_weight_128_dc);
        printf(" , ");
        fprintf(stdout,"lock weight = %.3f", system_weight);
        printf(" , ");
        fprintf(stdout,"calibration = %.5f", calibration_128);
        printf(" , ");
        fprintf(stdout,"tare_point = %.5f", tare_point_128);
        printf(" , ");
        printf(" \n ");

        char temp = USART_Receive(NULL);
        if(temp == 'a'){
            tare_point_128 = tare_point_128 + 1;
        }
        else if (temp == 'z'){
            tare_point_128 = tare_point_128 - 1;
        }
        else if (temp == 'x'){
            calibration_128 = calibration_128 - 1;
        }
        else if (temp == 's'){
            calibration_128 = calibration_128 + 1;
        }
        //HX711_power_down();
    }
}