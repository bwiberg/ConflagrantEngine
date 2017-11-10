#include <gtest/gtest.h>

#include <conflagrant/serialization/Serialize.hh>

#include <conflagrant/serialization/array.hh>
#include <conflagrant/serialization/vector.hh>
#include <conflagrant/serialization/crossguid.hh>
#include <conflagrant/serialization/glm.hh>

class SerializationTest : public ::testing::Test {
public:
    Json::Value json;
};

#define EXPECT_SERIALIZE_RETURNS_TRUE(value) EXPECT_TRUE(cfl::Serialize<cfl::Serializer>(json, value))
#define EXPECT_DESERIALIZE_RETURNS_TRUE(value) EXPECT_TRUE(cfl::Serialize<cfl::Deserializer>(json, value))

///////////////////
/// value types ///
///////////////////

TEST_F(SerializationTest, int_SerializationWorks) {
    int a = -27399;
    decltype(a) b;

    EXPECT_TRUE(cfl::Serializer::Serialize(json, a));
    EXPECT_TRUE(cfl::Deserializer::Serialize(json, b));
    EXPECT_EQ(a, b);
}

TEST_F(SerializationTest, uint_SerializationWorks) {
    uint a = 27398;
    decltype(a) b;

    EXPECT_TRUE(cfl::Serializer::Serialize(json, a));
    EXPECT_TRUE(cfl::Deserializer::Serialize(json, b));
    EXPECT_EQ(a, b);
}

TEST_F(SerializationTest, float_SerializationWorks) {
    float a = -27399.f;
    decltype(a) b;

    EXPECT_TRUE(cfl::Serializer::Serialize(json, a));
    EXPECT_TRUE(cfl::Deserializer::Serialize(json, b));
    EXPECT_EQ(a, b);
}

TEST_F(SerializationTest, double_SerializationWorks) {
    double a = -2739954219.012312312599;
    decltype(a) b;

    EXPECT_TRUE(cfl::Serializer::Serialize(json, a));
    EXPECT_TRUE(cfl::Deserializer::Serialize(json, b));
    EXPECT_EQ(a, b);
}

TEST_F(SerializationTest, bool_SerializationWorks) {
    bool a = true;
    decltype(a) b;

    EXPECT_TRUE(cfl::Serializer::Serialize(json, a));
    EXPECT_TRUE(cfl::Deserializer::Serialize(json, b));
    EXPECT_EQ(a, b);
}

TEST_F(SerializationTest, string_SerializationWorks) {
    cfl::string a = "Hello, world!";
    decltype(a) b;

    EXPECT_TRUE(cfl::Serializer::Serialize(json, a));
    EXPECT_TRUE(cfl::Deserializer::Serialize(json, b));
    EXPECT_EQ(a, b);
}

//////////////////////////
/// array/vector types ///
//////////////////////////

TEST_F(SerializationTest, std_array_SerializationWorks) {
    std::array<std::string, 5> a = {"first", "second", "3rd", "4th", "5th"};
    decltype(a) b;

    EXPECT_SERIALIZE_RETURNS_TRUE(a);
    EXPECT_DESERIALIZE_RETURNS_TRUE(b);
    EXPECT_EQ(a, b);
}

TEST_F(SerializationTest, std_vector_SerializationWorks) {
    std::vector<uint> a;
    a.push_back(54);
    a.push_back(33);
    a.push_back(19);
    a.push_back(1235);
    decltype(a) b;

    EXPECT_SERIALIZE_RETURNS_TRUE(a);
    EXPECT_DESERIALIZE_RETURNS_TRUE(b);
    EXPECT_EQ(a, b);
}

/////////////////////
/// float vectors ///
/////////////////////

template<typename vector_t>
bool Equals(vector_t const &a, vector_t const &b) {
    int const N = a.length();
    for (int i = 0; i < N; ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

TEST_F(SerializationTest, glm_vec2Works) {
    glm::vec2 a(1.25f, 1.26f);
    decltype(a) b;
    EXPECT_SERIALIZE_RETURNS_TRUE(a);
    EXPECT_DESERIALIZE_RETURNS_TRUE(b);
    EXPECT_TRUE(Equals(a, b));
}

TEST_F(SerializationTest, glm_vec3Works) {
    glm::vec3 a(1.25f, 1.26f, 1337.0f);
    decltype(a) b;

    EXPECT_SERIALIZE_RETURNS_TRUE(a);
    EXPECT_DESERIALIZE_RETURNS_TRUE(b);
    EXPECT_TRUE(Equals(a, b));
}

TEST_F(SerializationTest, glm_vec4Works) {
    glm::vec4 a(1.25f, 1.26f, -12454785689.0f, 15.0f);
    decltype(a) b;

    EXPECT_SERIALIZE_RETURNS_TRUE(a);
    EXPECT_DESERIALIZE_RETURNS_TRUE(b);
    EXPECT_TRUE(Equals(a, b));
}

//////////////////////
/// double vectors ///
//////////////////////

TEST_F(SerializationTest, glm_dvec2Works) {
    glm::dvec2 a(1.25, 1.26);
    decltype(a) b;
    EXPECT_SERIALIZE_RETURNS_TRUE(a);
    EXPECT_DESERIALIZE_RETURNS_TRUE(b);
    EXPECT_TRUE(Equals(a, b));
}

TEST_F(SerializationTest, glm_dvec3Works) {
    glm::dvec3 a(1.2, 1.2, 1337.0);
    decltype(a) b;

    EXPECT_SERIALIZE_RETURNS_TRUE(a);
    EXPECT_DESERIALIZE_RETURNS_TRUE(b);
    EXPECT_TRUE(Equals(a, b));
}

TEST_F(SerializationTest, glm_dvec4Works) {
    glm::dvec4 a(1.25, 1.26, -1245153915453112352758431657816324785689.0, 15.0);
    decltype(a) b;

    EXPECT_SERIALIZE_RETURNS_TRUE(a);
    EXPECT_DESERIALIZE_RETURNS_TRUE(b);
    EXPECT_TRUE(Equals(a, b));
}

///////////////////////
/// integer vectors ///
///////////////////////

TEST_F(SerializationTest, glm_ivec2Works) {
    glm::ivec2 a(1, 1);
    decltype(a) b;
    EXPECT_SERIALIZE_RETURNS_TRUE(a);
    EXPECT_DESERIALIZE_RETURNS_TRUE(b);
    EXPECT_TRUE(Equals(a, b));
}

TEST_F(SerializationTest, glm_ivec3Works) {
    glm::ivec3 a(-125412, 1, 1337);
    decltype(a) b;

    EXPECT_SERIALIZE_RETURNS_TRUE(a);
    EXPECT_DESERIALIZE_RETURNS_TRUE(b);
    EXPECT_TRUE(Equals(a, b));
}

TEST_F(SerializationTest, glm_ivec4Works) {
    glm::ivec4 a(1, 1, -12451539, 15);
    decltype(a) b;

    EXPECT_SERIALIZE_RETURNS_TRUE(a);
    EXPECT_DESERIALIZE_RETURNS_TRUE(b);
    EXPECT_TRUE(Equals(a, b));
}

////////////////////////////////
/// unsigned integer vectors ///
////////////////////////////////

TEST_F(SerializationTest, glm_uvec2Works) {
    glm::uvec2 a(1, 1);
    decltype(a) b;
    EXPECT_SERIALIZE_RETURNS_TRUE(a);
    EXPECT_DESERIALIZE_RETURNS_TRUE(b);
    EXPECT_TRUE(Equals(a, b));
}

TEST_F(SerializationTest, glm_uvec3Works) {
    glm::uvec3 a(125412, 1, 1337);
    decltype(a) b;

    EXPECT_SERIALIZE_RETURNS_TRUE(a);
    EXPECT_DESERIALIZE_RETURNS_TRUE(b);
    EXPECT_TRUE(Equals(a, b));
}

TEST_F(SerializationTest, glm_uvec4Works) {
    glm::uvec4 a(1, 1, 5135718, 15);
    decltype(a) b;

    EXPECT_SERIALIZE_RETURNS_TRUE(a);
    EXPECT_DESERIALIZE_RETURNS_TRUE(b);
    EXPECT_TRUE(Equals(a, b));
}

///////////////////////
/// boolean vectors ///
///////////////////////

TEST_F(SerializationTest, glm_bvec2Works) {
    glm::bvec2 a(true, false);
    decltype(a) b;
    EXPECT_SERIALIZE_RETURNS_TRUE(a);
    EXPECT_DESERIALIZE_RETURNS_TRUE(b);
    EXPECT_TRUE(Equals(a, b));
}

TEST_F(SerializationTest, glm_bvec3Works) {
    glm::bvec3 a(true, false, true);
    decltype(a) b;

    EXPECT_SERIALIZE_RETURNS_TRUE(a);
    EXPECT_DESERIALIZE_RETURNS_TRUE(b);
    EXPECT_TRUE(Equals(a, b));
}

TEST_F(SerializationTest, glm_bvec4Works) {
    glm::bvec4 a(true, false, false, false);
    decltype(a) b;

    EXPECT_SERIALIZE_RETURNS_TRUE(a);
    EXPECT_DESERIALIZE_RETURNS_TRUE(b);
    EXPECT_TRUE(Equals(a, b));
}

////////////////
/// matrices ///
////////////////

template<typename matrix_t>
bool MatrixEquals(matrix_t const &a, matrix_t const &b) {
    int const N = a.length();
    for (int i = 0; i < N; ++i) {
        if (!Equals(a[i], b[i])) {
            return false;
        }
    }
    return true;
}

TEST_F(SerializationTest, glm_mat2Works) {
    glm::mat2 a(0, 1, 2, 3.1240f);
    decltype(a) b;
    EXPECT_SERIALIZE_RETURNS_TRUE(a);
    EXPECT_DESERIALIZE_RETURNS_TRUE(b);
    EXPECT_TRUE(MatrixEquals(a, b));
}

TEST_F(SerializationTest, glm_mat3Works) {
    glm::mat3 a(0, 1, 2, 3.1230001123f, 4, 5, 6, 7, 1337);
    decltype(a) b;

    EXPECT_SERIALIZE_RETURNS_TRUE(a);
    EXPECT_DESERIALIZE_RETURNS_TRUE(b);
    EXPECT_TRUE(MatrixEquals(a, b));
}

TEST_F(SerializationTest, glm_mat4Works) {
    glm::mat4 a(0, 2, 1, 4, 3, 6, 5.1241241f, 8, 7, 123, 124, 54, 123, 432, 12412, 1249);
    decltype(a) b;

    EXPECT_SERIALIZE_RETURNS_TRUE(a);
    EXPECT_DESERIALIZE_RETURNS_TRUE(b);
    EXPECT_TRUE(MatrixEquals(a, b));
}
