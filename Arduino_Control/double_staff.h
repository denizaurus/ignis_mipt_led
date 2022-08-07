//#include <FastLED.h>
#include "image_processing.hpp"
#include "ignis_palette.h"


void colors_split(int hex, int first_led, uint32_t wait)
{
    uint32_t t_beg = millis();

    for (int i = 0; i < num_leds; ++i)
    {
        if (i >= first_led) {
            leds_1[i] = hex;
            leds_2[i] = hex;
        } 
        else {
            leds_1[i] = black;
            leds_2[i] = black;
        }
    }
    FastLED.show();
    waitfor(t_beg, wait);
}

void colors_split(int R, int G, int B, int first_led, uint32_t wait) {
     colors_split((R << 16) + (G << 8) + B, first_led, wait);
}


void dualcolors(int hex_1, int hex_2, int first_led_1, int first_led_2, uint32_t wait) {
    
    uint32_t t_beg = millis();

    for (int i = 0; i < num_leds; ++i) {

        if (i >= first_led_1) { leds_1[i] = hex_1; }
        else                  { leds_1[i] = black; }

        if (i >= first_led_2) { leds_2[i] = hex_2; }
        else                  { leds_2[i] = black; }
    }
    FastLED.show();
    waitfor(t_beg, wait);
}

void dualcolors(int R_1, int G_1, int B_1, int R_2, int G_2, int B_2, 
                int first_led_1, int first_led_2, uint32_t wait) {
     dualcolors((R_1 << 16) + (G_1 << 8) + B_1, (R_2 << 16) + (G_2 << 8) + B_2, first_led_1, first_led_2, wait);
}


void colors(int hex, uint32_t wait) {

    uint32_t t_beg = millis();

    for (auto & led : leds_1) { led = hex; };
    for (auto & led : leds_2) { led = hex; };

    FastLED.show();
    waitfor(t_beg, wait);
}

void colors(int R, int G, int B, uint32_t wait) {
    colors((R << 16) + (G << 8) + B, wait);
}

// backwards compatibility
void colors_fast(int R, int G, int B, uint32_t wait) {
    colors((R << 16) + (G << 8) + B, wait);
}


void gradient(int R_1, int G_1, int B_1, int R_2, int G_2, int B_2, uint32_t wait) {
    uint32_t t_beg = millis();

    for (int i = 1; i <= num_leds; ++i)
    {

        leds_1[i - 1].setRGB(
            R_1 * (i - 1) / num_leds * (i - 1) / num_leds + R_2 * (1 - i / num_leds) * (1 - i / num_leds),
            G_1 * (i - 1) / num_leds * (i - 1) / num_leds + G_2 * (1 - i / num_leds) * (1 - i / num_leds),
            B_1 * (i - 1) / num_leds * (i - 1) / num_leds + B_2 * (1 - i / num_leds) * (1 - i / num_leds)
        );

        leds_2[i - 1].setRGB(
            R_1 * i / num_leds * i / num_leds + R_2 * (1 - i / num_leds) * (1 - i / num_leds),
            G_1 * i / num_leds * i / num_leds + G_2 * (1 - i / num_leds) * (1 - i / num_leds),
            B_1 * i / num_leds * i / num_leds + B_2 * (1 - i / num_leds) * (1 - i / num_leds)
        );
    }
    FastLED.show();
    waitfor(t_beg, wait);
}

void gradient(int hex_1, int hex_2, uint32_t wait) {
     gradient(hex_1 >> 16, (hex_1 >> 8) & 0xFF, hex_1 & 0xFF,
              hex_2 >> 16, (hex_2 >> 8) & 0xFF, hex_2 & 0xFF, wait);
}


void strob(int hex, uint32_t time_strob) {

    uint32_t t_beg = millis();

    uint32_t numer_strob = time_strob / (2 * 50);

    for (auto i = 0U; i < numer_strob; ++i) {
        colors(hex,   50); 
        colors(black, 50);
    }
    waitfor(t_beg, time_strob);
}

// find out why there is the <= 100 specialisation
void strob(int R, int G, int B, uint32_t time_strob) {
     strob((R << 16) + (G << 8) + B, time_strob);
}


inline void transition(uint32_t time, const CHSV & color1, const CHSV & color2)
{
    const int size = time / 10;

    CRGB palette[size];
    fill_gradient(palette, 0, color1, size, color2);

    for (int i = 0; i < size; ++i)
    {
        auto t = millis();
        
        fill_solid(leds_1, num_leds, palette[i]);
        fill_solid(leds_2, num_leds, palette[i]);
        FastLED.show();
        waitfor(t, 10);
    }
}

void transition(uint32_t time, CRGB color1, CRGB color2 = black) { // accepts hex
    transition(time, rgb2hsv_approximate(color1), rgb2hsv_approximate(color2));
}

// requires file modding as this has not been merged into FastLED

//inline void transition(uint32_t time, CRGB color1, CRGB color2)
//{
//    transition(time, rgb2hsv_precise(color1), rgb2hsv_precise(color2));
//}

void transition(uint32_t time, int R_1, int G_1, int B_1, int R_2 = 0, int G_2 = 0, int B_2 = 0) {
    transition(time, CRGB(R_1, G_1, B_1), CRGB(R_2, G_2, B_2));
}