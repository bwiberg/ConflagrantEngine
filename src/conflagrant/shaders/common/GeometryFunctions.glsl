vec3 MakeOrthogonal(vec3 u) {
    const vec3 v = vec3(0.610446, 0.772075, 0.176788);
    return abs(dot(u, v)) > 0.999f ? cross(u, vec3(0, 1, 0)) : cross(u, v);
}

void MakeOrthonormalBasisTBN(vec3 N, out vec3 T, out vec3 B) {
    T = MakeOrthogonal(N);
    B = normalize(cross(T, N));
}
