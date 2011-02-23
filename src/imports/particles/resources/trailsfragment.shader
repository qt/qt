uniform sampler2D texture;

varying highp vec2 fTex;
varying lowp vec4 fColor;

void main() {
    gl_FragColor = (texture2D(texture, fTex).w) * fColor;
}
