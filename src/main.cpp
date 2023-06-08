#include <mbed.h>
#include <threadLvgl.h>
#include "DHT.h"
#include <cstdio>
#include "mbed.h"

lv_meter_indicator_t * indic1;
static lv_obj_t *meter_temp;

//#include <Arduino.h>
static void set_value(lv_meter_indicator_t * indic, int32_t v);
void lv_example_meter_2(void);
#define PIN_GATE_IN 2
#define IRQ_GATE_IN  0
#define PIN_LED_OUT 13
//#define PIN_ANALOG_IN A10

DHT sensor(D2,DHT22);

ThreadLvgl threadLvgl(30);

int main() {
    threadLvgl.lock();
    lv_example_meter_2();

    threadLvgl.unlock();
    AnalogIn ain(A1);

    int error =0;
    float temp, humidity;
    int i=0;

    while (1) {
        i+=1;
        // print the percentage and 16 bit normalized values
       // printf("percentage: %3.3f%%\n", ain.read()*100.0f);
       // printf("normalized: 0x%04X \n", ain.read_u16());
        if (ain>0.3)
        {
            printf("FOOOOOOOOOOOOOOOOOOOORT");
        }
        else
        {
            printf("faible");
        }
        if(i>=300)
        {
        error=sensor.readData();
        if(error==0)
        {
            temp=sensor.ReadTemperature(CELCIUS);
            humidity=sensor.ReadHumidity();
            printf("\n Temperature (chaude) %.2f \n",temp);
            printf("Humidity %.2f \n",humidity);
            i=0;
             /*   threadLvgl.lock();
                set_value(indic1,temp);
                threadLvgl.unlock();*/
        }
        else
        {
            printf("Error %d\n",error);
        }
        }
        // put your main code here, to run repeatedly:
        ThisThread::sleep_for(2ms);
    }
}

static void set_value(lv_meter_indicator_t * indic, int32_t v)
{
    lv_meter_set_indicator_end_value(meter_temp, indic, v);
}


void lv_example_meter_2(void)
{
    meter_temp = lv_meter_create(lv_scr_act());
    lv_obj_center(meter_temp);
    lv_obj_set_size(meter_temp, 200, 200);

    /*Remove the circle from the middle*/
    lv_obj_remove_style(meter_temp, NULL, LV_PART_INDICATOR);

    /*Add a scale first*/
    lv_meter_scale_t * scale = lv_meter_add_scale(meter_temp);
    lv_meter_set_scale_ticks(meter_temp, scale, 11, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter_temp, scale, 1, 2, 30, lv_color_hex3(0xeee), 15);
    lv_meter_set_scale_range(meter_temp, scale, 0, 100, 270, 90);

    /*Add a three arc indicator*/
    lv_meter_indicator_t * indic1 = lv_meter_add_arc(meter_temp, scale, 10, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_indicator_t * indic2 = lv_meter_add_arc(meter_temp, scale, 10, lv_palette_main(LV_PALETTE_GREEN), -10);
    lv_meter_indicator_t * indic3 = lv_meter_add_arc(meter_temp, scale, 10, lv_palette_main(LV_PALETTE_BLUE), -20);
}
