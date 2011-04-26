attribute highp vec2 vPos;
attribute highp vec2 vTex;
attribute highp vec4 vData; //  x = time,  y = lifeSpan, z = size,  w = endSize
attribute highp vec4 vVec; // x,y = constant speed,  z,w = acceleration
attribute lowp vec4 vColor;

uniform highp mat4 matrix;                              
uniform highp float timestamp;
uniform sampler2D sizetable;
uniform sampler2D opacitytable;

varying highp vec2 fTex;                                
varying lowp vec4 fColor;
varying lowp float tt;

void main() {                                           
    fTex = vTex;                                        
    highp float size = vData.z;
    highp float endSize = vData.w;

    highp float t = (timestamp - vData.x) / vData.y;

    highp float currentSize = mix(size, endSize, t * t) * texture2D(sizetable, vec2(t,0.5)).w;

    if (t < 0. || t > 1.)
        currentSize = 0.;

    highp vec2 pos = vPos
                   - currentSize / 2. + currentSize * vTex          // adjust size
                   + vVec.xy * t * vData.y         // apply speed vector..
                   + 0.5 * vVec.zw * pow(t * vData.y, 2.);

    gl_Position = matrix * vec4(pos.x, pos.y, 0, 1);

    fColor = vColor;
    tt = t;

}
