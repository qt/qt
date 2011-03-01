uniform sampler2D texture;

varying highp vec2 fTex;

void main() {
    gl_FragColor = texture2D(texture, fTex);
}
