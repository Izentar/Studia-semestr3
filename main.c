#include <stdio.h>
#include <stdlib.h>
#include "Zbior_Julii.h"
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

struct MyBitmap{
  int resolutionX;  // 0
  int resolutionY;  // 4
  int positionMouseX; //8
  int positionMouseY; //12
  ALLEGRO_LOCKED_REGION* innerImage;  //16
  double ReC; //24
  double ImC; //32
  double ReUpperLeft; // do not change 40
  double ImUpperLeft; // do not change 48
  double ReBottomRight; // do not change 56
  double ImBottomRight; // do not change 64
  double defaultReUpperLeft; // do not change 72
  double defaultImUpperLeft; // do not change 80
  double defaultReBottomRight; // do not change 88
  double defaultImBottomRight; // do not change 96
  double test; // 104
  double test2; //112
};

typedef struct MyBitmap MyBitmap;

MyBitmap* create_MyBitmap(int resX, int resY)
{
  MyBitmap *tmp=malloc(sizeof(MyBitmap));
  tmp->resolutionX=resX;
  tmp->resolutionY=resY;
  tmp->positionMouseX=resX/2;
  tmp->positionMouseY=resY/2;
  tmp->innerImage=NULL;
  tmp->ReC=0.0;
  tmp->ImC=1.0;
  tmp->ReUpperLeft=1.5;
  tmp->ImUpperLeft=1.25;
  tmp->ReBottomRight=-1.5;
  tmp->ImBottomRight=-1.25;
  tmp->defaultReUpperLeft=tmp->ReUpperLeft;
  tmp->defaultImUpperLeft=tmp->ImUpperLeft;
  tmp->defaultReBottomRight=tmp->ReBottomRight;
  tmp->defaultImBottomRight=tmp->ImBottomRight;
  tmp->test=-1.1;
  tmp->test2=-1.1;
  return tmp;
}

void must_init(bool test, const char *description)
{
    if(test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

int main()
{
    int resolutionX=640, resolutionY=640;

    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_install_mouse(), "mouse");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    ALLEGRO_DISPLAY* disp = al_create_display(resolutionX, resolutionY);
    must_init(disp, "display");

    ALLEGRO_FONT* font = al_create_builtin_font();
    must_init(font, "font");

    /*if(!al_init_image_addon())
    {
        printf("couldn't initialize image addon\n");
        return 1;
    }

    ALLEGRO_BITMAP* imageToDisplay = al_load_bitmap("Zbior_Julii.bmp");*/
    //ALLEGRO_LOCKED_REGION* image=al_lock_bitmap(imageToDisplay, al_get_bitmap_format(imageToDisplay) , ALLEGRO_LOCK_READWRITE);
    //al_unlock_bitmap(imageToDisplay);



    /*if(!imageToDisplay)
    {
        printf("couldn't load image\n");
        return 1;
    }*/

    ALLEGRO_BITMAP *imageToDisplay=al_create_bitmap(resolutionX, resolutionY);
    al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY_32_WITH_ALPHA);
    MyBitmap* myImage=create_MyBitmap(resolutionX, resolutionY);

    must_init(al_init_primitives_addon(), "primitives");

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_mouse_event_source());

    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;
    ALLEGRO_MOUSE_STATE mouseState;

    unsigned char mousePressedL=0, mousePressedR=0, enter=0;
    int approx_avert=1;

    #define KEY_SEEN     1
    #define KEY_RELEASED 2

    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));

    ALLEGRO_LOCKED_REGION* image=al_lock_bitmap(imageToDisplay, al_get_bitmap_format(imageToDisplay) , ALLEGRO_LOCK_READWRITE);
    myImage->innerImage=image;
    printf("Pitch: %d\n", image->pitch);
    printf("Pixel size: %d\n", image->pixel_size);
    al_unlock_bitmap(imageToDisplay);

    al_start_timer(timer);
    while(1)
    {
        al_wait_for_event(queue, &event);
        al_get_mouse_state(&mouseState);

        if((mousePressedL==1||mousePressedL==2)&&(mousePressedR==1||mousePressedR==2)&&(mouseState.buttons & 2||mouseState.buttons & 1))
        {
          mousePressedL=3;
        }
        if(mouseState.buttons == 0 &&mousePressedL==3)
        {
          mousePressedL=0;
          mousePressedR=0;
        }

        if (mouseState.buttons & 1&&mousePressedL==0)
        {
          mousePressedL=1;
        }
        if(mousePressedL==1)
        {
          mousePressedL=2;
        }
        if (mouseState.buttons==0&&mousePressedL==2)
        {
          myImage->positionMouseX=mouseState.x;
          myImage->positionMouseY=mouseState.y;
          mousePressedL=0;
          approx_avert=0;
          redraw=true;
        }

        if (mouseState.buttons & 2&&mousePressedR==0)
        {
          mousePressedR=1;
        }
        if(mousePressedR==1)
        {
          mousePressedR=2;
        }
        if (mouseState.buttons==0&&mousePressedR==2)
        {
          myImage->positionMouseX=mouseState.x;
          myImage->positionMouseY=mouseState.y;
          mousePressedR=0;
          approx_avert=2;
          redraw=true;
        }



        switch(event.type)
        {
            case ALLEGRO_EVENT_TIMER:
                if(key[ALLEGRO_KEY_ESCAPE])
                    done = true;

                for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
                    key[i] &= KEY_SEEN;

                break;

            case ALLEGRO_EVENT_KEY_CHAR:
                if(enter==0&&event.keyboard.keycode==ALLEGRO_KEY_ENTER)
                {
                  key[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
                  printf("Give constant C real: ");
                  if(scanf("%lf", &(myImage->ReC))!=1)
                  {
                    free(myImage);
                    al_destroy_bitmap(imageToDisplay);
                    al_destroy_font(font);
                    al_destroy_display(disp);
                    al_destroy_timer(timer);
                    al_destroy_event_queue(queue);
                    return 1;
                  }
                  printf("Give constant C imaginary: ");
                  if(scanf("%lf", &(myImage->ImC))!=1)
                  {
                    free(myImage);
                    al_destroy_bitmap(imageToDisplay);
                    al_destroy_font(font);
                    al_destroy_display(disp);
                    al_destroy_timer(timer);
                    al_destroy_event_queue(queue);
                    return 1;
                  }
                  redraw=true;
                  enter=1;
                  approx_avert=2;
                  break;
                }



            case ALLEGRO_EVENT_KEY_DOWN:
                key[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
                break;
            case ALLEGRO_EVENT_KEY_UP:
                key[event.keyboard.keycode] &= KEY_RELEASED;
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
        }

        if(done)
            break;

        if(redraw && al_is_event_queue_empty(queue))
        {
            image=al_lock_bitmap(imageToDisplay, al_get_bitmap_format(imageToDisplay) , ALLEGRO_LOCK_READWRITE);

            printf("Mouse position: (%d, %d)\n", myImage->positionMouseX, myImage->positionMouseY);

            // my function
            Zbior_Julii(myImage, approx_avert); //can draw without clicking
            al_unlock_bitmap(imageToDisplay);

            printf("Pixel boundary: %f %f %f %f \n", myImage->ReUpperLeft, myImage->ImUpperLeft, myImage->ReBottomRight, myImage->ImBottomRight);
            printf("Default pixel boundary: %f %f %f %f \n", myImage->defaultReUpperLeft, myImage->defaultImUpperLeft, myImage->defaultReBottomRight, myImage->defaultImBottomRight);
            printf("Test value: %lf %lf \n", myImage->test, myImage->test2);
            printf("Drawing\n");

            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_bitmap(imageToDisplay, 0, 0, 0);

            al_flip_display();

            redraw = false;
            approx_avert=1;
            enter=0;
        }
    }
    free(myImage);
    al_destroy_bitmap(imageToDisplay);
    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
