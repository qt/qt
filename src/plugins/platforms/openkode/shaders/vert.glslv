uniform vec2 window; // window size

// Per-vertex attributes]
attribute vec2 pos_attr;
attribute vec2 texcoord_attr;

// Output vertex color
varying vec2 texcoord_var;

void main()
{
    gl_Position = vec4( (2.0 * pos_attr / window -1.0) * vec2(1.0, -1.0), 0.0, 1.0);
    texcoord_var = texcoord_attr;
}
