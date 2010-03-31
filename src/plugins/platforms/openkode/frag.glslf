uniform sampler2D tex_samp;

varying vec2 texcoord_var;

void main()
{
    gl_FragColor = texture2D(tex_samp, texcoord_var);
}
