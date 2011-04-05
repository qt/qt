attribute highp vec2 vTex;
attribute highp vec4 vAnimData;// idx, duration, frameCount (this anim), timestamp (this anim)

uniform highp mat4 matrix;
uniform highp float timestamp;                          
uniform lowp float opacity;
uniform highp float framecount; //maximum of all anims
uniform highp float animcount;
uniform highp float width;
uniform highp float height;

varying highp vec2 fTexA;
varying highp vec2 fTexB;
varying lowp float progress;


void main() {                                           
    //Calculate frame location in texture
    highp float frameIndex = mod((((timestamp - vAnimData.w)*1000.)/vAnimData.y),vAnimData.z);
    progress = mod((timestamp - vAnimData.w)*1000., vAnimData.y) / vAnimData.y;

    frameIndex = floor(frameIndex);
    highp vec2 frameTex;
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


    gl_Position = matrix * vec4(width * vTex.x, height * vTex.y, 0, 1);
}
