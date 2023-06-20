#include <mbed.h>
//Bibliothèque de l'écran d'affichage
#include <threadLvgl.h>
//Biliothèque du capteur DHT22
#include "DHT.h"
#include <cstdio>
#include "mbed.h"

//Declaration des pointeurs
lv_meter_indicator_t * indic1;
lv_meter_indicator_t * indic2;
lv_meter_indicator_t * indic3;
static lv_obj_t *meter_temp;
lv_obj_t *bar;

//Initialisation des fonctions
static void set_temp(lv_obj_t * bar, int32_t temp);
static void set_value(lv_meter_indicator_t * indic, int32_t v);
static void event_cb(lv_event_t * e);

void lv_example_meter_2(void);
void lv_example_label_1(void);
void lv_example_event_1(void);
void lv_example_chart_1(void);
void lv_example_bar_3(void);

//Declaration des variables globales
lv_coord_t tabTemp[11],tabHum[11],test1=50,test2=50;
#define PIN_GATE_IN 2
#define IRQ_GATE_IN  0
#define PIN_LED_OUT 13


DHT sensor(D2,DHT22);

ThreadLvgl threadLvgl(30);

int main() {
    //Création des Widgets
    threadLvgl.lock();
    lv_example_meter_2();
    lv_example_bar_3();
    lv_example_label_1();
    lv_example_event_1();
    lv_example_chart_1();
    threadLvgl.unlock();
    //Lecture de la broche A1, l'amplitude du son (envelope)
    AnalogIn ain(A1);
    int ain1=0,ain2=0;
    int error =0;
    float temp, humidity;
    int i=0;

    while (1) {
        i+=1;

        //Affichage du son, avec filtre de lissage
        ain2=ain1;
        ain1=ain;
        threadLvgl.lock();
        set_temp(bar,(ain*0.5+ain1*0.3+ain2*0.2)*200);
        threadLvgl.unlock();

        //Le son doit être échantilloné plus vite que la température et l'humidité
        //On ne les lit donc que toutes les 12 boucles
        if(i>=12)
        {
        //Vérification qu'on reçoit bien les données
        error=sensor.readData();
        if(error==0)
        {
            //Lecture des données de température et humidité
            temp=sensor.ReadTemperature(CELCIUS);
            humidity=sensor.ReadHumidity();
            printf("\n Temperature (chaude) %.2f \n",temp);
            printf("Humidity %.2f \n",humidity);
            //Remplissage de tableaux pour le graphe
            for(int n=0;n<10;n=n+1)
            {
                tabTemp[n]=tabTemp[n+1];
                tabHum[n]=tabHum[n+1];
            }
            tabTemp[10]=temp;
            tabHum[10]=humidity;
            i=0;
            //Affichage des données de température et humidité
            threadLvgl.lock();
            set_value(indic1,temp);
            set_value(indic3,humidity);
            threadLvgl.unlock();
        }
        else
        {
            printf("Error %d\n",error);
        }
        }
        //50ms de délai permet d'avoir un affichage du son fluide mais pas (trop) épileptique
        ThisThread::sleep_for(50ms);
    }
}

//Cette fonction sert à mettre à jour les valeurs du cadran
static void set_value(lv_meter_indicator_t * indic, int32_t v)
{
    lv_meter_set_indicator_end_value(meter_temp, indic, v);
}

//Cette fonction sert à créer le cadran
void lv_example_meter_2(void)
{
    meter_temp = lv_meter_create(lv_scr_act());
    //lv_obj_center(meter_temp);
    lv_obj_set_pos(meter_temp,50,50);
    lv_obj_set_size(meter_temp, 200, 200);

    /*Remove the circle from the middle*/
    lv_obj_remove_style(meter_temp, NULL, LV_PART_INDICATOR);

    /*Add a scale first*/
    lv_meter_scale_t * scale = lv_meter_add_scale(meter_temp);
    lv_meter_set_scale_ticks(meter_temp, scale, 11, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter_temp, scale, 1, 2, 30, lv_color_hex3(0xeee), 15);
    lv_meter_set_scale_range(meter_temp, scale, 0, 60, 270, 90);

    /*Add a three arc indicator*/
    indic1 = lv_meter_add_arc(meter_temp, scale, 10, lv_palette_main(LV_PALETTE_RED), 0);
    indic2 = lv_meter_add_arc(meter_temp, scale, 10, lv_palette_main(LV_PALETTE_GREEN), -10);
    indic3 = lv_meter_add_arc(meter_temp, scale, 10, lv_palette_main(LV_PALETTE_BLUE), -20);
}

//Cette fonction crée les textes affichés à l'écran
void lv_example_label_1(void)
{
    lv_obj_t * label1 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    //Ecriture et couleur du texte
    lv_label_set_text(label1, "  #ff0000 Temperature# #ff00ff /# #0000ff Humidity#");
    //Determine la largeur de la zone de texte
    lv_obj_set_width(label1, 300);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    //Permet de placer un widget à l'endroit choisi
    lv_obj_align(label1, LV_ALIGN_CENTER, -90, -100);

    lv_obj_t * label2 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);  
    lv_obj_set_width(label2, 300);
    lv_label_set_text(label2, "Volume sonore ");
    lv_obj_align(label2, LV_ALIGN_CENTER, 250, -100);

        lv_obj_t * label3 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label3, LV_LABEL_LONG_WRAP);  
    lv_obj_set_width(label3, 300);
    lv_label_set_text(label3, "50");
    lv_obj_align(label3, LV_ALIGN_CENTER, 380, 20);

            lv_obj_t * label4 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label4, LV_LABEL_LONG_WRAP);  
    lv_obj_set_width(label4, 300);
    lv_label_set_text(label4, "0");
    lv_obj_align(label4, LV_ALIGN_CENTER, 388, 82);

                lv_obj_t * label5 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label5, LV_LABEL_LONG_WRAP);  
    lv_obj_set_width(label5, 300);
    lv_label_set_text(label5, "100");
    lv_obj_align(label5, LV_ALIGN_CENTER, 375, -40);

}

//Cette foction sert à mettre à jour la valeur de la barre de son
static void set_temp(lv_obj_t * bar, int32_t temp)
{
    lv_bar_set_value(bar, temp, LV_ANIM_ON);
}

//Cette fonction crée la barre de son
void lv_example_bar_3(void)
{
    static lv_style_t style_indic;

    lv_style_init(&style_indic);
    lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
    //On peut choisir la couleur de la barre en fonction de sa hauteur
    lv_style_set_bg_color(&style_indic, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_bg_grad_color(&style_indic, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_bg_grad_dir(&style_indic, LV_GRAD_DIR_VER);

    bar = lv_bar_create(lv_scr_act());
    lv_obj_add_style(bar, &style_indic, LV_PART_INDICATOR);
    lv_obj_set_size(bar, 20, 200);
    lv_obj_align(bar, LV_ALIGN_CENTER, 150, 20);
    //Permet de choisir l'échelle de l'affichage
    lv_bar_set_range(bar, 0, 40);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, 0);
    lv_anim_set_time(&a, 3000);
    lv_anim_set_playback_time(&a, 3000);
    lv_anim_set_var(&a, bar);
    lv_anim_set_values(&a, -20, 40);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}

//Cette fonction gère le bouton de Refresh
static void event_cb(lv_event_t * e)
{
    LV_LOG_USER("Clicked");

    static uint32_t cnt = 1;
    lv_obj_t * btn = lv_event_get_target(e);
    //On met à jour le graphe
    lv_example_chart_1();
    cnt++;
}

//Cette fonction crée le bouton
void lv_example_event_1(void)
{
    lv_obj_t * btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, 100, 50);
    lv_obj_center(btn);
    lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align(btn, LV_ALIGN_CENTER, 350, -100);

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Refresh");
    lv_obj_center(label);
}

//Cette fonction crée et gère le graphe
void lv_example_chart_1(void)
{
    /*Create a chart*/
    lv_obj_t * chart;
    chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart, 200, 150);
    lv_obj_center(chart);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/
    lv_obj_align(chart, LV_ALIGN_CENTER, 350, 20);

    //On choisi les données affichées et leur couleur
    lv_chart_series_t * ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t * ser2 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_SECONDARY_Y);

//On affiche les tableaux remplis dans la boucle
    for (int i=0;i<10;i++)
    {
        lv_chart_set_next_value(chart, ser1, tabTemp[i]);
    }

    for (int i=0;i<10;i++)
    {
    ser2->y_points[i] = tabHum[i];
    printf("Hum %d: %d",i,tabHum[i]);
    }

    lv_chart_refresh(chart); /*Required after direct set*/
}
