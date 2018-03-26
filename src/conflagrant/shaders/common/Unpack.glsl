void Unpack_vec3_float(vec4 value, out vec3 _vec3, out float _float) {
    _vec3 = value.xyz;
    _float = value.w;
}
