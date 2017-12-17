void AlphaBlend_FrontToBack(inout vec3 colorDst, inout float alphaDst, in vec3 colorSrc, in float alphaSrc) {
    const float k = (1 - alphaDst) * alphaSrc;
    colorDst += k * colorSrc;
    alphaSrc += k;
}
