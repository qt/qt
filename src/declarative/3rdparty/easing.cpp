/*
Disclaimer for Robert Penner's Easing Equations license:

TERMS OF USE - EASING EQUATIONS

Open source under the BSD License.

Copyright © 2001 Robert Penner
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the author nor the names of contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2 (M_PI / 2)
#endif


/**
 * Easing equation function for a simple linear tweening, with no easing.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @return		The correct value.
 */
static float easeNone(float t, float b, float c, float d)
{
    return c*t/d + b;
}
	
/**
 * Easing equation function for a quadratic (t^2) easing in: accelerating from zero velocity.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @return		The correct value.
 */
static float easeInQuad(float t, float b, float c, float d) 
{
    float t_adj = (float)t / (float)d;
    return c*t_adj*t_adj + b;
}
	
/**
* Easing equation function for a quadratic (t^2) easing out: decelerating to zero velocity.
*
* @param t		Current time (in frames or seconds).
* @param b		Starting value.
* @param c		Change needed in value.
* @param d		Expected easing duration (in frames or seconds).
* @return		The correct value.
*/
static float easeOutQuad(float t, float b, float c, float d) 
{
    float t_adj = (float)t / (float)d;
    return -c *t_adj*(t_adj-2) + b;
}
	
/**
* Easing equation function for a quadratic (t^2) easing in/out: acceleration until halfway, then deceleration.
*
* @param t		Current time (in frames or seconds).
* @param b		Starting value.
* @param c		Change needed in value.
* @param d		Expected easing duration (in frames or seconds).
* @return		The correct value.
*/
static float easeInOutQuad(float t, float b, float c, float d) 
{
    float t_adj = 2.0f * (float)t / (float)d;
    if (t_adj < 1) {
        return c/2*t_adj*t_adj + b;
    } else {
        --t_adj;
        return -c/2 * ((t_adj)*(t_adj-2) - 1) + b;
    }
}
	
/**
* Easing equation function for a quadratic (t^2) easing out/in: deceleration until halfway, then acceleration.
*
* @param t		Current time (in frames or seconds).
* @param b		Starting value.
* @param c		Change needed in value.
* @param d		Expected easing duration (in frames or seconds).
* @return		The correct value.
*/
static float easeOutInQuad(float t, float b, float c, float d) 
{
    if (t < d/2) return easeOutQuad (t*2, b, c/2, d);
    return easeInQuad((t*2)-d, b+c/2, c/2, d);
}
	
/**
* Easing equation function for a cubic (t^3) easing in: accelerating from zero velocity.
*
* @param t		Current time (in frames or seconds).
* @param b		Starting value.
* @param c		Change needed in value.
* @param d		Expected easing duration (in frames or seconds).
* @return		The correct value.
*/
static float easeInCubic(float t, float b, float c, float d) 
{
    float t_adj = (float)t / (float)d;
    return c*t_adj*t_adj*t_adj + b;
}
	
/**
* Easing equation function for a cubic (t^3) easing out: decelerating from zero velocity.
*
* @param t		Current time (in frames or seconds).
* @param b		Starting value.
* @param c		Change needed in value.
* @param d		Expected easing duration (in frames or seconds).
* @return		The correct value.
*/
static float easeOutCubic(float t, float b, float c, float d) 
{
    float t_adj = (float)t / (float)(d) - 1.0f;
    return c*(t_adj*t_adj*t_adj + 1) + b;
}
	
/**
* Easing equation function for a cubic (t^3) easing in/out: acceleration until halfway, then deceleration.
*
* @param t		Current time (in frames or seconds).
* @param b		Starting value.
* @param c		Change needed in value.
* @param d		Expected easing duration (in frames or seconds).
* @return		The correct value.
*/
static float easeInOutCubic(float t, float b, float c, float d) 
{
    float t_adj = 2.0f * (float)t / (float)(d);
    if(t_adj < 1) return c/2*t_adj*t_adj*t_adj + b;
    else {
        t_adj -= 2;
        return c/2*(t_adj*t_adj*t_adj + 2) + b;
    }
}
	
/**
* Easing equation function for a cubic (t^3) easing out/in: deceleration until halfway, then acceleration.
*
* @param t		Current time (in frames or seconds).
* @param b		Starting value.
* @param c		Change needed in value.
* @param d		Expected easing duration (in frames or seconds).
* @return		The correct value.
*/
static float easeOutInCubic(float t, float b, float c, float d) 
{
    if (t < d/2) return easeOutCubic (t*2, b, c/2, d);
    return easeInCubic((t*2)-d, b+c/2, c/2, d);
}

/**
* Easing equation function for a quartic (t^4) easing in: accelerating from zero velocity.
*
* @param t		Current time (in frames or seconds).
* @param b		Starting value.
* @param c		Change needed in value.
* @param d		Expected easing duration (in frames or seconds).
* @return		The correct value.
*/
static float easeInQuart(float t, float b, float c, float d) 
{
    float t_adj = (float)t / (float)d;
    return c*t_adj*t_adj*t_adj*t_adj + b;
}
	
/**
* Easing equation function for a quartic (t^4) easing out: decelerating from zero velocity.
*
* @param t		Current time (in frames or seconds).
* @param b		Starting value.
* @param c		Change needed in value.
* @param d		Expected easing duration (in frames or seconds).
* @return		The correct value.
*/
static float easeOutQuart(float t, float b, float c, float d) 
{
    float t_adj = (float)t / (float)d - 1.0f;
    return -c * (t_adj*t_adj*t_adj*t_adj - 1) + b;
}
	
/**
* Easing equation function for a quartic (t^4) easing in/out: acceleration until halfway, then deceleration.
*
* @param t		Current time (in frames or seconds).
* @param b		Starting value.
* @param c		Change needed in value.
* @param d		Expected easing duration (in frames or seconds).
* @return		The correct value.
*/
static float easeInOutQuart(float t, float b, float c, float d) 
{
    float t_adj = 2.0f * (float)t / (float)d;
    if (t_adj < 1) return c/2*t_adj*t_adj*t_adj*t_adj + b;
    else {
        t_adj -= 2.0f;
        return -c/2 * (t_adj*t_adj*t_adj*t_adj - 2) + b;
    }
} 

/**
* Easing equation function for a quartic (t^4) easing out/in: deceleration until halfway, then acceleration.
*
* @param t		Current time (in frames or seconds).
* @param b		Starting value.
* @param c		Change needed in value.
* @param d		Expected easing duration (in frames or seconds).
* @return		The correct value.
*/
static float easeOutInQuart(float t, float b, float c, float d) 
{
    if (t < d/2) return easeOutQuart (t*2, b, c/2, d);
    return easeInQuart((t*2)-d, b+c/2, c/2, d);
}
	
/**
 * Easing equation function for a quintic (t^5) easing in: accelerating from zero velocity.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @return		The correct value.
 */
static float easeInQuint(float t, float b, float c, float d) 
{
    float t_adj = (float)t/(float)d;
    return c*t_adj*t_adj*t_adj*t_adj*t_adj + b;
}
	
/**
 * Easing equation function for a quintic (t^5) easing out: decelerating from zero velocity.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @return		The correct value.
 */
static float easeOutQuint(float t, float b, float c, float d) 
{
    float t_adj = (float)t/(float)d - 1.0f;
    return c*(t_adj*t_adj*t_adj*t_adj*t_adj + 1) + b;
}
	
/**
 * Easing equation function for a quintic (t^5) easing in/out: acceleration until halfway, then deceleration.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @return		The correct value.
 */
static float easeInOutQuint(float t, float b, float c, float d) 
{
    float t_adj = 2.0f * (float)t/(float)d;
    if (t_adj < 1) return c/2*t_adj*t_adj*t_adj*t_adj*t_adj + b;
    else {
        t_adj -= 2.0f;
        return c/2*(t_adj*t_adj*t_adj*t_adj*t_adj + 2) + b;
    }
}
	
/**
 * Easing equation function for a quintic (t^5) easing out/in: deceleration until halfway, then acceleration.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @return		The correct value.
 */
static float easeOutInQuint(float t, float b, float c, float d) 
{
    if (t < d/2) return easeOutQuint (t*2, b, c/2, d);
    return easeInQuint((t*2)-d, b+c/2, c/2, d);
}
	
/**
 * Easing equation function for a sinusoidal (sin(t)) easing in: accelerating from zero velocity.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @return		The correct value.
 */
static float easeInSine(float t, float b, float c, float d) 
{
    float t_adj = (float)t/(float)d;
    return -c * ::cos(t_adj * M_PI_2) + c + b;
}
	
/**
 * Easing equation function for a sinusoidal (sin(t)) easing out: decelerating from zero velocity.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @return		The correct value.
 */
static float easeOutSine(float t, float b, float c, float d) 
{
    float t_adj = (float)t/(float)d;
    return c * ::sin(t_adj * M_PI_2) + b;
}
	
/**
 * Easing equation function for a sinusoidal (sin(t)) easing in/out: acceleration until halfway, then deceleration.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @return		The correct value.
 */
static float easeInOutSine(float t, float b, float c, float d) 
{
    float t_adj = (float)t/(float)d;
    return -c/2 * (::cos(M_PI*t_adj) - 1) + b;
}
	
/**
 * Easing equation function for a sinusoidal (sin(t)) easing out/in: deceleration until halfway, then acceleration.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @return		The correct value.
 */
static float easeOutInSine(float t, float b, float c, float d) 
{
    if (t < d/2) return easeOutSine (t*2, b, c/2, d);
    return easeInSine((t*2)-d, b+c/2, c/2, d);
}
	
/**
 * Easing equation function for an exponential (2^t) easing in: accelerating from zero velocity.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @return		The correct value.
 */
static float easeInExpo(float t, float b, float c, float d) 
{
    return (t==0) ? b : c * ::pow(2, 10 * ((float)t/(float)d - 1)) + b - c * 0.001;
}

/**
 * Easing equation function for an exponential (2^t) easing out: decelerating from zero velocity.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @return		The correct value.
 */
static float easeOutExpo(float t, float b, float c, float d) 
{
    return (t==d) ? b+c : c * 1.001 * (-::pow(2, -10 * (float)t/(float)d) + 1) + b;
}

/**
 * Easing equation function for an exponential (2^t) easing in/out: acceleration until halfway, then deceleration.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @return		The correct value.
 */
static float easeInOutExpo(float t, float b, float c, float d) 
{
    if (t==0) return b;
    if (t==d) return b+c;
    float t_adj = 2.0f * (float)t/(float)d;
    if (t_adj < 1) return c/2 * ::pow(2, 10 * (t_adj - 1)) + b - c * 0.0005;
    return c/2 * 1.0005 * (-::pow(2, -10 * (t_adj - 1)) + 2) + b;
}

/**
 * Easing equation function for an exponential (2^t) easing out/in: deceleration until halfway, then acceleration.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @return		The correct value.
 */
static float easeOutInExpo(float t, float b, float c, float d) 
{
    if (t < d/2) return easeOutExpo (t*2, b, c/2, d);
    return easeInExpo((t*2)-d, b+c/2, c/2, d);
}
	
/**
 * Easing equation function for a circular (sqrt(1-t^2)) easing in: accelerating from zero velocity.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @return		The correct value.
 */
static float easeInCirc(float t, float b, float c, float d) 
{
    float t_adj = (float)t / (float)d;
    return -c * (::sqrt(1 - t_adj*t_adj) - 1) + b;
}

/**
 * Easing equation function for a circular (sqrt(1-t^2)) easing out: decelerating from zero velocity.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @return		The correct value.
 */
static float easeOutCirc(float t, float b, float c, float d) 
{
    float t_adj = (float)t / (float)d - 1.0f;
    return c * ::sqrt(1 - t_adj * t_adj) + b;
}

/**
 * Easing equation function for a circular (sqrt(1-t^2)) easing in/out: acceleration until halfway, then deceleration.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @return		The correct value.
 */
static float easeInOutCirc(float t, float b, float c, float d) 
{
    float t_adj = 2.0f * (float)t / (float)d;
    if (t_adj < 1) return -c/2 * (::sqrt(1 - t_adj*t_adj) - 1) + b;
    else {
        t_adj -= 2.0f;
        return c/2 * (::sqrt(1 - t_adj*t_adj) + 1) + b;
    }
}

/**
 * Easing equation function for a circular (sqrt(1-t^2)) easing out/in: deceleration until halfway, then acceleration.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @return		The correct value.
 */
static float easeOutInCirc(float t, float b, float c, float d) 
{
    if (t < d/2) return easeOutCirc (t*2, b, c/2, d);
    return easeInCirc((t*2)-d, b+c/2, c/2, d);
}

/**
 * Easing equation function for an elastic (exponentially decaying sine wave) easing in: accelerating from zero velocity.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @param a		Amplitude.
 * @param p		Period.
 * @return		The correct value.
 */
static float easeInElastic(float t, float b, float c, float d) 
{
    if (t==0) return b;  
    float t_adj = (float)t / (float)d;
    if (t_adj==1) return b+c;  
    
    qreal p = d * 0.3f;
    qreal a = c;
    qreal s = p / 4.0f;

    t_adj -= 1.0f;
    return -(a*::pow(2,10*t_adj) * ::sin( (t_adj*d-s)*(2*M_PI)/p )) + b;
}

/**
 * Easing equation function for an elastic (exponentially decaying sine wave) easing out: decelerating from zero velocity.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @param a		Amplitude.
 * @param p		Period.
 * @return		The correct value.
 */
static float easeOutElastic(float t, float b, float c, float d)
{
    if (t==0) return b;  
    float t_adj = (float)t / (float)d;
    if (t_adj==1) return b+c;  

    qreal p = d * 0.3f;
    qreal a = c;
    qreal s = p / 4;

    return (a*::pow(2,-10*t_adj) * ::sin( (t_adj*d-s)*(2*M_PI)/p ) + c + b);
}

/**
 * Easing equation function for an elastic (exponentially decaying sine wave) easing in/out: acceleration until halfway, then deceleration.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @param a		Amplitude.
 * @param p		Period.
 * @return		The correct value.
 */
static float easeInOutElastic(float t, float b, float c, float d)
{
    if (t==0) return b;  
    float t_adj = 2.0f * (float)t / (float)d;
    if (t_adj==2) return b+c;  

    // XXX
    qreal p = d * 0.3f * 1.5f;
    qreal a = c;
    qreal s = p / 4;

    if (t_adj < 1) return -.5*(a*::pow(2,10*(t_adj-1)) * ::sin( ((t_adj-1)*d-s)*(2*M_PI)/p )) + b;
    return a*::pow(2,-10*(t_adj-1)) * ::sin( ((t_adj-1)*d-s)*(2*M_PI)/p )*.5 + c + b;
}

/**
 * Easing equation function for an elastic (exponentially decaying sine wave) easing out/in: deceleration until halfway, then acceleration.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @param a		Amplitude.
 * @param p		Period.
 * @return		The correct value.
 */
static float easeOutInElastic(float t, float b, float c, float d)
{
    if (t < d/2) return easeOutElastic (t*2, b, c/2, d);
    return easeInElastic((t*2)-d, b+c/2, c/2, d);
}
	
/**
 * Easing equation function for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) easing in: accelerating from zero velocity.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @param s		Overshoot ammount: higher s means greater overshoot (0 produces cubic easing with no overshoot, and the default value of 1.70158 produces an overshoot of 10 percent).
 * @return		The correct value.
 */
static float easeInBack(float t, float b, float c, float d)
{
    // XXX
    qreal s = 1.70158;

    float t_adj = (float)t / (float)d;
    return c*(t_adj)*t_adj*((s+1)*t_adj - s) + b;
}

/**
 * Easing equation function for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) easing out: decelerating from zero velocity.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @param s		Overshoot ammount: higher s means greater overshoot (0 produces cubic easing with no overshoot, and the default value of 1.70158 produces an overshoot of 10 percent).
 * @return		The correct value.
 */
static float easeOutBack(float t, float b, float c, float d)
{
    // XXX
    qreal s = 1.70158;

    float t_adj = (float)t / (float)d - 1.0f;
    return c*(t_adj*t_adj*((s+1)*t_adj + s) + 1) + b;
}

/**
 * Easing equation function for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) easing in/out: acceleration until halfway, then deceleration.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @param s		Overshoot ammount: higher s means greater overshoot (0 produces cubic easing with no overshoot, and the default value of 1.70158 produces an overshoot of 10 percent).
 * @return		The correct value.
 */
static float easeInOutBack(float t, float b, float c, float d)
{
    // XXX
    qreal s = 1.70158;

    float t_adj = 2.0f * (float)t / (float)d;
    if (t_adj < 1) {
        s *= 1.525f;
        return c/2*(t_adj*t_adj*((s+1)*t_adj - s)) + b;
    } else {
        t_adj -= 2;
        s *= 1.525f;
        return c/2*(t_adj*t_adj*((s+1)*t_adj + s) + 2) + b;
    }
}

/**
 * Easing equation function for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) easing out/in: deceleration until halfway, then acceleration.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @param s		Overshoot ammount: higher s means greater overshoot (0 produces cubic easing with no overshoot, and the default value of 1.70158 produces an overshoot of 10 percent).
 * @return		The correct value.
 */
static float easeOutInBack(float t, float b, float c, float d)
{
    if (t < d/2) return easeOutBack (t*2, b, c/2, d);
    return easeInBack((t*2)-d, b+c/2, c/2, d);
}
	
/**
 * Easing equation function for a bounce (exponentially decaying parabolic bounce) easing out: decelerating from zero velocity.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @return		The correct value.
 */
static float easeOutBounce(float t, float b, float c, float d)
{
    float t_adj = (float)t / (float)d;
    if ((t_adj) < (1/2.75)) {
        return c*(7.5625*t_adj*t_adj) + b;
    } else if (t_adj < (2/2.75)) {
        t_adj -= (1.5f/2.75f);
        return c*(7.5625*t_adj*t_adj + .75) + b;
    } else if (t_adj < (2.5/2.75)) {
        t_adj -= (2.25f/2.75f);
        return c*(7.5625*t_adj*t_adj + .9375) + b;
    } else {
        t_adj -= (2.65f/2.75f);
        return c*(7.5625*t_adj*t_adj + .984375) + b;
    }
}

/**
 * Easing equation function for a bounce (exponentially decaying parabolic bounce) easing in: accelerating from zero velocity.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @return		The correct value.
 */
static float easeInBounce(float t, float b, float c, float d)
{
    return c - easeOutBounce (d-t, 0, c, d) + b;
}
	
	
/**
 * Easing equation function for a bounce (exponentially decaying parabolic bounce) easing in/out: acceleration until halfway, then deceleration.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @return		The correct value.
 */
static float easeInOutBounce(float t, float b, float c, float d)
{
    if (t < d/2) return easeInBounce (t*2, 0, c, d) * .5 + b;
    else return easeOutBounce (t*2-d, 0, c, d) * .5 + c*.5 + b;
}
	
/**
 * Easing equation function for a bounce (exponentially decaying parabolic bounce) easing out/in: deceleration until halfway, then acceleration.
 *
 * @param t		Current time (in frames or seconds).
 * @param b		Starting value.
 * @param c		Change needed in value.
 * @param d		Expected easing duration (in frames or seconds).
 * @return		The correct value.
 */
static float easeOutInBounce(float t, float b, float c, float d)
{
    if (t < d/2) return easeOutBounce (t*2, b, c/2, d);
    return easeInBounce((t*2)-d, b+c/2, c/2, d);
}

