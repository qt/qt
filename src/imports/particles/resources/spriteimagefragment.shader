uniform sampler2D texture;

varying highp vec2 fTexA;
varying highp vec2 fTexB;
varying lowp float progress;

void main() {
    gl_FragColor = mix(texture2D(texture, fTexA), texture2D(texture, fTexB), progress);
}
