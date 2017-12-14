const vec3 Ones = vec3(1, 1, 1);

vec3 GetUnitCubeCoordinates(const vec3 worldPosition, const vec3 voxelCenter, const vec3 voxelHalfDimensions) {
    return (worldPosition - voxelCenter) / voxelHalfDimensions;
}

vec3 GetNormalizedCoordinatesFromUnitCubeCoordinates(const vec3 unitCubeCoordinates) {
    return 0.5 * unitCubeCoordinates + 0.5;
}

bool IsWithinVoxelColume(const vec3 unitCubeCoordinates) {
    return all(lessThan(abs(unitCubeCoordinates), Ones));
}

ivec3 GetIntegerCoordinatesFromNormalizedTextureCoordinates(layout(RGBA8) image3D image, vec3 normalizedCoordinates) {
    return ivec3(imageSize(image) * normalizedCoordinates);
}
