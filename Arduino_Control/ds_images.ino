#include "double_staff.h"
#include "DEMO.hpp"

// uint16_t for mode 0, uint8_t for 1 and 2
ImageProcessor < uint16_t > ipr(image_map, image_info, image_count);

void setup() {
  FastLED.addLeds<NEOPIXEL, PB6>(leds_1, num_leds);
  FastLED.addLeds<NEOPIXEL, PB7>(leds_2, num_leds);
}

const uint32_t frequency = 250U;
const uint32_t time = 5000;

void loop()
{
    colors(darkorange, time);
    ipr.show("15_p.bmp", frequency, time);
    colors(yellow, time);
    ipr.show("13_setka.bmp", frequency, time);
    colors(chartreuse, time);
    ipr.show("31_lp.bmp", frequency, time);
    colors(lime, time);
    ipr.show("35_qp.bmp", frequency, time);
    colors(springgreen, time);
    ipr.show("48_plo.bmp", frequency, time);
    colors(cyan, time);
    ipr.show("29_hi.bmp", frequency, time);
    colors(0, 127, 255, time);
    ipr.show("04_aa.bmp", frequency, time);
    colors(blue, time);
    ipr.show("33_kp.bmp", frequency, time);
    colors(127, 0, 255, time);
    ipr.show("13_n.bmp", frequency, time);
    colors(magenta, time);
    ipr.show("20_u.bmp", frequency, time);
    colors(255, 0, 127, time);
    ipr.show("10_k.bmp", frequency, time);
  
    
}
