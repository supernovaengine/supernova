//
// (c) 2021 Eduardo Doria.
//

#ifndef EASE_H
#define EASE_H

#define _USE_MATH_DEFINES
#include <math.h>

#define S_LINEAR 0
#define S_EASE_QUAD_IN 1
#define S_EASE_QUAD_OUT 2
#define S_EASE_QUAD_IN_OUT 3
#define S_EASE_CUBIC_IN 4
#define S_EASE_CUBIC_OUT 5
#define S_EASE_CUBIC_IN_OUT 6
#define S_EASE_QUART_IN 7
#define S_EASE_QUART_OUT 8
#define S_EASE_QUART_IN_OUT 9
#define S_EASE_QUINT_IN 10
#define S_EASE_QUINT_OUT 11
#define S_EASE_QUINT_IN_OUT 12
#define S_EASE_SINE_IN 13
#define S_EASE_SINE_OUT 14
#define S_EASE_SINE_IN_OUT 15
#define S_EASE_EXPO_IN 16
#define S_EASE_EXPO_OUT 17
#define S_EASE_EXPO_IN_OUT 18
#define S_EASE_CIRC_IN 19
#define S_EASE_CIRC_OUT 20
#define S_EASE_CIRC_IN_OUT 21
#define S_EASE_ELASTIC_IN 22
#define S_EASE_ELASTIC_OUT 23
#define S_EASE_ELASTIC_IN_OUT 24
#define S_EASE_BACK_IN 25
#define S_EASE_BACK_OUT 26
#define S_EASE_BACK_IN_OUT 27
#define S_EASE_BOUNCE_IN 28
#define S_EASE_BOUNCE_OUT 29
#define S_EASE_BOUNCE_IN_OUT 30

namespace Supernova {
    namespace Ease {
        inline float linear(float time){
            return time;
        }

        inline float easeInQuad(float time){
            return time * time;
        }

        inline float easeOutQuad(float time){
            return time * (2 - time);
        }

        inline float easeInOutQuad(float time){
            if ((time *= 2) < 1) {
                return 0.5 * time * time;
            }

            time--;
            return - 0.5 * (time * (time - 2) - 1);
        }

        inline float easeInCubic(float time){
            return time * time * time;
        }

        inline float easeOutCubic(float time){
            time--;
            return time * time * time + 1;
        }

        inline float easeInOutCubic(float time){
            if ((time *= 2) < 1) {
                return 0.5 * time * time * time;
            }
            time -= 2;
            return 0.5 * (time * time * time + 2);
        }

        inline float easeInQuart(float time){
            return time * time * time * time;
        }

        inline float easeOutQuart(float time){
            time--;
            return 1 - (time * time * time * time);
        }

        inline float easeInOutQuart(float time){
            if ((time *= 2) < 1) {
                return 0.5 * time * time * time * time;
            }
            time -= 2;
            return - 0.5 * (time * time * time * time - 2);
        }

        inline float easeInQuint(float time){
            return time * time * time * time * time;
        }

        inline float easeOutQuint(float time){
            time--;
            return time * time * time * time * time + 1;
        }

        inline float easeInOutQuint(float time){
            if ((time *= 2) < 1) {
                return 0.5 * time * time * time * time * time;
            }

            time -= 2;
            return 0.5 * (time * time * time * time * time + 2);
        }

        inline float easeInSine(float time){
            return 1 - cos(time * M_PI / 2);
        }

        inline float easeOutSine(float time){
            return sin(time * M_PI / 2);
        }

        inline float easeInOutSine(float time){
            return 0.5 * (1 - cos(M_PI * time));
        }

        inline float easeInExpo(float time){
            return time == 0 ? 0 : pow(1024, time - 1);
        }

        inline float easeOutExpo(float time){
            return time == 1 ? 1 : 1 - pow(2, - 10 * time);
        }

        inline float easeInOutExpo(float time){
            if (time == 0) {
                return 0;
            }

            if (time == 1) {
                return 1;
            }

            if ((time *= 2) < 1) {
                return 0.5 * pow(1024, time - 1);
            }

            return 0.5 * (- pow(2, - 10 * (time - 1)) + 2);
        }

        inline float easeInCirc(float time){
            return 1 - sqrt(1 - time * time);
        }

        inline float easeOutCirc(float time){
            time--;
            return sqrt(1 - (time * time));
        }

        inline float easeInOutCirc(float time){
            if ((time *= 2) < 1) {
                return - 0.5 * (sqrt(1 - time * time) - 1);
            }

            time -= 2;
            return 0.5 * (sqrt(1 - time * time) + 1);
        }

        inline float easeInElastic(float time){
            if (time == 0) {
                return 0;
            }

            if (time == 1) {
                return 1;
            }

            return -pow(2, 10 * (time - 1)) * sin((time - 1.1) * 5 * M_PI);
        }

        inline float easeOutElastic(float time){
            if (time == 0) {
                return 0;
            }

            if (time == 1) {
                return 1;
            }

            return pow(2, -10 * time) * sin((time - 0.1) * 5 * M_PI) + 1;
        }

        inline float easeInOutElastic(float time){
            if (time == 0) {
                return 0;
            }

            if (time == 1) {
                return 1;
            }

            time *= 2;

            if (time < 1) {
                return -0.5 * pow(2, 10 * (time - 1)) * sin((time - 1.1) * 5 * M_PI);
            }

            return 0.5 * pow(2, -10 * (time - 1)) * sin((time - 1.1) * 5 * M_PI) + 1;
        }

        inline float easeInBack(float time){
            float s = 1.70158;

            return time * time * ((s + 1) * time - s);
        }

        inline float easeOutBack(float time){
            float s = 1.70158;

            time--;
            return time * time * ((s + 1) * time + s) + 1;
        }

        inline float easeInOutBack(float time){
            float s = 1.70158 * 1.525;

            if ((time *= 2) < 1) {
                return 0.5 * (time * time * ((s + 1) * time - s));
            }

            time -= 2;
            return 0.5 * (time * time * ((s + 1) * time + s) + 2);
        }

        inline float easeOutBounce(float time){
            if (time < (1 / 2.75)) {
                return 7.5625 * time * time;
            } else if (time < (2 / 2.75)) {
                time -= (1.5 / 2.75);
                return 7.5625 * time * time + 0.75;
            } else if (time < (2.5 / 2.75)) {
                time -= (2.25 / 2.75);
                return 7.5625 * time * time + 0.9375;
            } else {
                time -= (2.625 / 2.75);
                return 7.5625 * time * time + 0.984375;
            }
        }

        inline float easeInBounce(float time){
            return 1 - easeOutBounce(1 - time);
        }

        inline float easeInOutBounce(float time){
            if (time < 0.5) {
                return easeInBounce(time * 2) * 0.5;
            }

            return easeOutBounce(time * 2 - 1) * 0.5 + 0.5;
        }
    }
}


#endif //EASE_H
