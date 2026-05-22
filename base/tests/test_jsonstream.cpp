#include "gtest/gtest.h"
#include "base/jsonstream.h"

using namespace Rc;

TEST(JsonStream, WriteEmptyObject)
{
    std::string json;
    json
        << Json::BeginJson
        << Json::BeginObject
        << Json::EndObject
        << Json::EndJson;

    EXPECT_EQ(json, "{}");
}

TEST(JsonStream, WriteEmptyArray)
{
    std::string json;
    json
        << Json::BeginJson
        << Json::BeginArray
        << Json::EndArray
        << Json::EndJson;

    EXPECT_EQ(json, "[]");
}

TEST(JsonStream, WriteBool)
{
    std::string json;
    json
        << Json::BeginJson
        << Json::True
        << Json::EndJson;

    EXPECT_EQ(json, "true");
}

TEST(JsonStream, WriteNull)
{
    std::string json;
    json
        << Json::BeginJson
        << Json::Null
        << Json::EndJson;

    EXPECT_EQ(json, "null");
}

TEST(JsonStream, WriteInteger)
{
    std::string json;
    json
        << Json::BeginJson
        << Json::Number{-10}
        << Json::EndJson;

    EXPECT_EQ(json, "-10");
}

TEST(JsonStream, WriteUnsignedInteger)
{
    std::string json;
    json
        << Json::BeginJson
        << Json::Number{10u}
        << Json::EndJson;

    EXPECT_EQ(json, "10");
}

TEST(JsonStream, WriteEmptyArrayInArray)
{
    std::string json;
    json
        << Json::BeginJson
        << Json::BeginArray
        << Json::BeginArray
        << Json::EndArray
        << Json::EndArray
        << Json::EndJson;

    EXPECT_EQ(json, "[[]]");
}

TEST(JsonStream, WriteEmptyObjectInArray)
{
    std::string json;
    json
        << Json::BeginJson
        << Json::BeginArray
        << Json::BeginObject
        << Json::EndObject
        << Json::EndArray
        << Json::EndJson;

    EXPECT_EQ(json, "[{}]");
}

TEST(JsonStream, SimpleObject)
{
    std::string json;
    json
        << Json::BeginJson
        << Json::BeginObject
        << Json::Key{"float"}
        << Json::Number{1.2f}
        << Json::Key{"array"}
        << Json::BeginArray
        << Json::EndArray
        << Json::EndObject
        << Json::EndJson;

    EXPECT_EQ(json, R"({"float":1.2,"array":[]})");
}

TEST(JsonStream, StringWithEscapedChars)
{
    std::string json;
    json
        << Json::BeginJson
        << Json::Chars{"\r\n\t"}
        << Json::EndJson;

    EXPECT_EQ(json, R"("\r\n\t")");
}

TEST(JsonStream, RawString)
{
    std::string json;
    json
        << Json::BeginJson
        << Json::Chars{R"(\r\n\t)"}
        << Json::EndJson;

    EXPECT_EQ(json, R"("\r\n\t")");
}

TEST(JsonStream, Assert)
{
    std::string json;

    ASSERT_DEATH({
            json
                << Json::BeginJson
                << Json::BeginObject
                << Json::BeginObject
                << Json::EndObject
                << Json::EndJson;
        },
        ".+"
    );
}

TEST(Json, Assert2)
{
    std::string json;
    
    ASSERT_DEATH({
            json
                << Json::BeginJson
                << Json::BeginObject
                << Json::EndObject
                << Json::EndObject
                << Json::EndJson;
        },
        ".+"
    );
}

TEST(Json, ObjectKeyRequired)
{
    std::string json;
    
    ASSERT_DEATH({
            json
                << Json::BeginJson
                << Json::BeginObject
                << Json::BeginArray
                << Json::EndObject
                << Json::EndJson;
        },
        ".+"
    );
}

TEST(Json, ObjectKeyAfterKey)
{
    std::string json;
    
    ASSERT_DEATH({
            json
                << Json::BeginJson
                << Json::BeginObject
                << Json::Key{"key"}
                << Json::Key{"key"}
                << Json::EndObject
                << Json::EndJson;
        },
        ".+"
    );
}

// 

TEST(Json, Dev)
{
    std::string json;

    json
        << Json::BeginJson
        << Json::BeginObject
        << Json::Key{"a"}
        << Json::Number{10}
        << Json::Key{"b"}
        << Json::BeginArray
        << Json::BeginArray
        << Json::EndArray
        << Json::EndArray
        << Json::EndObject
        << Json::EndJson;

    SUCCEED();
}