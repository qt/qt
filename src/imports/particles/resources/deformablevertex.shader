attribute highp vec2 vPos;
attribute highp vec2 vTex;
attribute highp vec4 vData; //  x = time,  y = lifeSpan, z = size,  w = endSize
attribute highp vec4 vVec; // x,y = constant speed,  z,w = acceleration
attribute highp vec4 vDeformVec; //x,y x unit vector; z,w = y unit vector
attribute highp vec2 vRotation; //x = radians of rotation, y= bool autoRotate

uniform highp mat4 matrix;
uniform highp float timestamp;
uniform lowp float opacity;

varying highp vec2 fTex;
varying lowp float fFade;

void main() {                                           
    fTex = vTex;
    highp float size = vData.z;
    highp float endSize = vData.w;

    highp float t = (timestamp - vData.x) / vData.y;

    highp float currentSize = mix(size, endSize, t * t);

    highp vec2 pos;
    if (t < 0. || t > 1.){
        currentSize = 0.;
        pos = vPos;
    }else{
        highp float rotation = vRotation.x;
        if(vRotation.y == 1.0){
            highp vec2 curVel = vVec.zw * t * vData.y + vVec.xy;
            rotation += atan(curVel.y, curVel.x);
        }
        highp vec2 trigCalcs = vec2(cos(rotation), sin(rotation));
        highp vec2 xDeform = vDeformVec.xy * currentSize * (vTex.x-0.5);
        highp vec2 yDeform = vDeformVec.zw * currentSize * (vTex.y-0.5);
        highp vec2 xRotatedDeform;
        xRotatedDeform.x = trigCalcs.x*xDeform.x - trigCalcs.y*xDeform.y;
        xRotatedDeform.y = trigCalcs.y*xDeform.x + trigCalcs.x*xDeform.y;
        highp vec2 yRotatedDeform;
        yRotatedDeform.x = trigCalcs.x*yDeform.x - trigCalcs.y*yDeform.y;
        yRotatedDeform.y = trigCalcs.y*yDeform.x + trigCalcs.x*yDeform.y;
        pos = vPos
              + xRotatedDeform
              + yRotatedDeform
              //- vec2(1,1) * currentSize * 0.5 // 'center'
              + vVec.xy * t * vData.y         // apply speed
              + 0.5 * vVec.zw * pow(t * vData.y, 2.); // apply acceleration
    }

    gl_Position = matrix * vec4(pos.x, pos.y, 0, 1);

    highp float fadeIn = min(t * 10., 1.);
    highp float fadeOut = 1. - max(0., min((t - 0.75) * 4., 1.));

    fFade = fadeIn * fadeOut * opacity;
}
