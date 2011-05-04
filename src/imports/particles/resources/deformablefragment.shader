uniform sampler2D texture;

varying highp vec2 fTex;
varying lowp float fFade;

void main() {
    gl_FragColor = (texture2D(texture, fTex)) * fFade;
}
