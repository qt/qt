#extension GL_EXT_gpu_shader4 : enable
attribute highp vec2 vPos;
attribute highp vec2 vTex;                              
attribute highp vec3 vData; //  x = time,  y = size,  z = endSize
attribute highp vec4 vVec; // x,y = constant speed,  z,w = acceleration
attribute lowp vec4 vColor;

uniform highp mat4 matrix;                              
uniform highp float timestamp;                          
uniform highp float timelength;
uniform lowp float opacity;
uniform highp float framecount;
uniform highp float frameduration;

varying highp vec2 fTex;                                
varying lowp vec4 fColor;

void main() {                                           
    highp float size = vData.y;
    highp float endSize = vData.z;

    highp float t = (timestamp - vData.x) / timelength; 

    highp int frameIndex = int(uint(floor((timestamp * 1000.0)/float(frameduration)))%uint(framecount));
    highp vec2 frameTex = vTex;
    if(vTex.x == 0.)
        frameTex.x = (float(frameIndex)/float(framecount));
    else
        frameTex.x = 1. * (float(frameIndex + int(1))/float(framecount));

    fTex = frameTex;

    highp float currentSize = mix(size, endSize, t * t);

    if (t < 0. || t > 1.)
        currentSize = 0.;

    highp vec2 pos = vPos
                   - currentSize / 2. + currentSize * vTex          // adjust size
                   + vVec.xy * t * timelength         // apply speed vector..
                   + vVec.zw * pow(t * timelength, 2.);

    gl_Position = matrix * vec4(pos.x, pos.y, 0, 1);

    highp float fadeIn = min(t * 10., 1.);
    highp float fadeOut = 1. - max(0., min((t - 0.75) * 4., 1.));

    fColor = vColor * fadeIn * fadeOut * opacity;
}
