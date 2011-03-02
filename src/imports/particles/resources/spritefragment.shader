uniform sampler2D texture;

varying highp vec2 fTexA;
varying highp vec2 fTexB;
varying lowp float progress;
varying lowp vec4 fColor;

void main() {
    gl_FragColor = mix(texture2D(texture, fTexA), texture2D(texture, fTexB), progress) * fColor.w;
}
