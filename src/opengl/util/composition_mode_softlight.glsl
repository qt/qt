// Dca' = 2.Sca < Sa ?
//        Dca.(Sa - (1 - Dca/Da).(2.Sca - Sa)) + Sca.(1 - Da) + Dca.(1 - Sa) :
//          (8.Dca <= Da ?
//           Dca.(Sa - (1 - Dca/Da).(2.Sca - Sa).(3 - 8.Dca/Da)) + Sca.(1 - Da) + Dca.(1 - Sa) :
//           (Dca.Sa + ((Dca/Da)^(0.5).Da - Dca).(2.Sca - Sa)) + Sca.(1 - Da) + Dca.(1 - Sa))
// Da'  = Sa + Da - Sa.Da 
vec4 composite(vec4 src, vec4 dst)
{
    vec4 result;
    float da = max(dst.a, 0.00001);
    result.rgb = mix(dst.rgb * (src.a - (1.0 - dst.rgb / da) * (2.0 * src.rgb - src.a)),
                     mix(dst.rgb * (src.a - (1.0 - dst.rgb / da) * (2.0 * src.rgb - src.a) * (3.0 - 8.0 * dst.rgb / da)),
                         (dst.rgb * src.a + (sqrt(dst.rgb / da) * dst.a - dst.rgb) * (2.0 * src.rgb - src.a)),
                         step(dst.a, 8.0 * dst.rgb)),
                     step(src.a, 2.0 * src.rgb)) + src.rgb * (1.0 - dst.a) + dst.rgb * (1.0 - src.a);
    result.a = src.a + dst.a - src.a * dst.a;
    return result;
}
