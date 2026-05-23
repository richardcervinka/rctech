#include "gtest/gtest.h"
#include "base/jsonstream.h"

using namespace Rc;

TEST(JsonStream, WriteEmptyObject)
{
    std::string json;
    Json::Stream(json)
        << Json::BeginJson
        << Json::BeginObject
        << Json::EndObject
        << Json::EndJson;

    EXPECT_EQ(json, "{}");
}

TEST(JsonStream, WriteEmptyArray)
{
    std::string json;
    Json::Stream(json)
        << Json::BeginJson
        << Json::BeginArray
        << Json::EndArray
        << Json::EndJson;

    EXPECT_EQ(json, "[]");
}

TEST(JsonStream, WriteBool)
{
    std::string json;
    Json::Stream(json)
        << Json::BeginJson
        << Json::True
        << Json::EndJson;

    EXPECT_EQ(json, "true");
}

TEST(JsonStream, WriteNull)
{
    std::string json;
    Json::Stream(json)
        << Json::BeginJson
        << Json::Null
        << Json::EndJson;

    EXPECT_EQ(json, "null");
}

TEST(JsonStream, WriteInteger)
{
    std::string json;
    Json::Stream(json)
        << Json::BeginJson
        << Json::Number{-10}
        << Json::EndJson;

    EXPECT_EQ(json, "-10");
}

TEST(JsonStream, WriteUnsignedInteger)
{
    std::string json;
    Json::Stream(json)
        << Json::BeginJson
        << Json::Number{10u}
        << Json::EndJson;

    EXPECT_EQ(json, "10");
}

TEST(JsonStream, WriteEmptyArrayInArray)
{
    std::string json;
    Json::Stream(json)
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
    Json::Stream(json)
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
    Json::Stream(json)
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

TEST(JsonStream, EscapedAllSpecialChars)
{
    std::string json;
    Json::Stream(json)
        << Json::BeginJson
        << Json::Chars{"\"\b\f\n\r\t\\"}
        << Json::EndJson;

    EXPECT_EQ(json, R"("\"\b\f\n\r\t\\")");
}

TEST(JsonStream, RawString)
{
    std::string json;
    Json::Stream(json)
        << Json::BeginJson
        << Json::String{"\r\n\t"}
        << Json::EndJson;

    EXPECT_EQ(json, "\"\r\n\t\"");
}

TEST(JsonStream, ArrayCommaPlacement)
{
    std::string json;
    Json::Stream(json)
        << Json::BeginJson
        << Json::BeginArray
        << Json::Number{1}
        << Json::Number{2}
        << Json::Number{3}
        << Json::EndArray
        << Json::EndJson;

    EXPECT_EQ(json, "[1,2,3]");
}

TEST(JsonStream, ObjectCommaPlacement)
{
    std::string json;
    Json::Stream(json)
        << Json::BeginJson
        << Json::BeginObject
        << Json::Key{"a"} << Json::Number{1}
        << Json::Key{"b"} << Json::Number{2}
        << Json::Key{"c"} << Json::Number{3}
        << Json::EndObject
        << Json::EndJson;

    EXPECT_EQ(json, R"({"a":1,"b":2,"c":3})");
}

TEST(JsonStream, EscapedStringInObject)
{
    std::string json;
    Json::Stream(json)
        << Json::BeginJson
        << Json::BeginObject
        << Json::Key{"msg"}
        << Json::Chars{"Hello \"Richard\""}
        << Json::EndObject
        << Json::EndJson;

    EXPECT_EQ(json, R"({"msg":"Hello \"Richard\""})");
}

TEST(JsonStream, DeepNesting)
{
    std::string json;
    Json::Stream(json)
        << Json::BeginJson
        << Json::BeginObject
        << Json::Key{"outer"}
        << Json::BeginArray
        << Json::BeginObject
        << Json::Key{"x"} << Json::Number{1}
        << Json::EndObject
        << Json::EndArray
        << Json::EndObject
        << Json::EndJson;

    EXPECT_EQ(json, R"({"outer":[{"x":1}]})");
}

TEST(JsonStream, Assert)
{
    std::string json;

    ASSERT_DEATH({
            Json::Stream(json)
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
            Json::Stream(json)
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
            Json::Stream(json)
                << Json::BeginJson
                << Json::BeginObject
                << Json::BeginArray
                << Json::EndObject
                << Json::EndJson;
        },
        ".+"
    );
}

TEST(Json, ObjectKeyAsValue)
{
    std::string json;
    
    ASSERT_DEATH({
            Json::Stream(json)
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

TEST(Json, MissingObjectValue)
{
    std::string json;
    
    ASSERT_DEATH({
            Json::Stream(json)
                << Json::BeginJson
                << Json::BeginObject
                << Json::Key{"key"}
                << Json::EndObject
                << Json::EndJson;
        },
        ".+"
    );
}

TEST(JsonStream, ValueWithoutKeyInObject)
{
    std::string json;

    ASSERT_DEATH({
        Json::Stream(json)
            << Json::BeginJson
            << Json::BeginObject
            << Json::Number{123}
            << Json::EndObject
            << Json::EndJson;
    }, ".+");
}

TEST(JsonStream, KeyOutsideObject)
{
    std::string json;

    ASSERT_DEATH({
        Json::Stream(json)
            << Json::BeginJson
            << Json::Key{"x"}
            << Json::EndJson;
    }, ".+");
}

TEST(JsonStream, EndArrayWithoutBegin)
{
    std::string json;

    ASSERT_DEATH({
        Json::Stream(json)
            << Json::BeginJson
            << Json::EndArray
            << Json::EndJson;
    }, ".+");
}

TEST(JsonStream, EndJsonBeforeClosing)
{
    std::string json;

    ASSERT_DEATH({
        Json::Stream(json)
            << Json::BeginJson
            << Json::BeginObject
            << Json::EndJson;
    }, ".+");
}