attribute highp vec2 vTex;
attribute highp vec4 vAnimData;// idx, duration, frameCount (this anim), timestamp (this anim)

uniform highp mat4 matrix;
uniform highp float timestamp;                          
uniform lowp float opacity;
uniform highp float framecount; //maximum of all anims
uniform highp float animcount;
uniform highp float width;
uniform highp float height;

varying highp vec2 fTex;                                

void main() {                                           
    //Calculate frame location in texture
    highp float frameIndex = fract((((timestamp - vAnimData.w)*1000.)/vAnimData.y)/vAnimData.z) * vAnimData.z;
    //fract(x/z)*z used to avoid uints and % (GLSL chokes on them?)

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

    fTex = frameTex;


    gl_Position = matrix * vec4(width * vTex.x, height * vTex.y, 0, 1);
}
