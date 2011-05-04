uniform sampler2D texture;
uniform sampler2D colortable;
uniform sampler2D opacitytable;

varying highp vec2 fTex;
varying lowp vec4 fColor;
varying lowp float tt;

void main() {
    gl_FragColor = (texture2D(texture, fTex).w) * fColor * texture2D(colortable, vec2(tt, 0.5)) *( texture2D(opacitytable, vec2(tt, 0.5)).w);
}
