attribute highp vec2 vPos;
attribute highp vec2 vTex;                              
attribute highp vec4 vData; //  x = time,  y = lifeSpan, z = size,  w = endSize
attribute highp vec4 vVec; // x,y = constant speed,  z,w = acceleration
attribute highp vec4 vAnimData;// idx, duration, frameCount (this anim), timestamp (this anim)

uniform highp mat4 matrix;                              
uniform highp float timestamp;                          
uniform lowp float opacity;
uniform highp float framecount; //maximum of all anims
uniform highp float animcount;

varying highp vec2 fTexA;
varying highp vec2 fTexB;
varying lowp float progress;
varying lowp vec4 fColor;

void main() {                                           
    highp float size = vData.z;
    highp float endSize = vData.w;

    highp float t = (timestamp - vData.x) / vData.y;

    //Calculate frame location in texture
    highp float frameIndex = mod((((timestamp - vAnimData.w)*1000.)/vAnimData.y),vAnimData.z);
    progress = mod((timestamp - vAnimData.w)*1000., vAnimData.y) / vAnimData.y;

    frameIndex = floor(frameIndex);
    highp vec2 frameTex = vTex;
    if(vTex.x == 0.)
        frameTex.x = (frameIndex/framecount);
    else
        frameTex.x = 1. * ((frameIndex + 1.)/framecount);

    if(vTex.y == 0.)
        frameTex.y = (vAnimData.x/animcount);
    else
        frameTex.y = 1. * ((vAnimData.x + 1.)/animcount);

    fTexA = frameTex;
    //Next frame is also passed, for interpolation
    //### Should the next anim be precalculated to allow for interpolation there?
    if(frameIndex != vAnimData.z - 1.)//Can't do it for the last frame though, this anim may not loop
        frameIndex = mod(frameIndex+1., vAnimData.z);

    if(vTex.x == 0.)
        frameTex.x = (frameIndex/framecount);
    else
        frameTex.x = 1. * ((frameIndex + 1.)/framecount);

    if(vTex.y == 0.)
        frameTex.y = (vAnimData.x/animcount);
    else
        frameTex.y = 1. * ((vAnimData.x + 1.)/animcount);
    fTexB = frameTex;

    //Applying Size here seems to screw with RockingAffector?
    highp float currentSize = mix(size, endSize, t * t);

    if (t < 0. || t > 1.)
        currentSize = 0.;

    //If affector is mananging pos, they don't set speed?
    highp vec2 pos = vPos
                   - currentSize / 2. + currentSize * vTex          // adjust size
                   + vVec.xy * t * vData.y         // apply speed vector..
                   + 0.5 * vVec.zw * pow(t * vData.y, 2.);

    gl_Position = matrix * vec4(pos.x, pos.y, 0, 1);

    // calculate opacity
    highp float fadeIn = min(t * 10., 1.);
    highp float fadeOut = 1. - max(0., min((t - 0.75) * 4., 1.));

    lowp vec4 white = vec4(1.);
    fColor = white * fadeIn * fadeOut * opacity;
}
