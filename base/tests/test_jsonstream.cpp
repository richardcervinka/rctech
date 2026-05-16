#include "gtest/gtest.h"
#include "base/jsonstream.h"

using namespace Rc;

TEST(JsonStream, WriteEmptyObject)
{
    std::string json;
    Json::Stream(json)
        << Json::BeginObject
        << Json::EndObject;

    EXPECT_EQ(json, "{}");
}

TEST(JsonStream, WriteEmptyArray)
{
    std::string json;
    Json::Stream(json)
        << Json::BeginArray
        << Json::EndArray;

    EXPECT_EQ(json, "[]");
}

TEST(JsonStream, WriteBool)
{
    std::string json;
    Json::Stream(json) << true;

    EXPECT_EQ(json, "true");
}

TEST(JsonStream, WriteNull)
{
    std::string json;
    Json::Stream(json) << nullptr;

    EXPECT_EQ(json, "null");
}

TEST(JsonStream, WriteInteger)
{
    std::string json;
    Json::Stream(json) << -10;

    EXPECT_EQ(json, "-10");
}

TEST(JsonStream, WriteUnsignedInteger)
{
    std::string json;
    Json::Stream(json) << 10u;

    EXPECT_EQ(json, "10");
}

TEST(JsonStream, WriteEmptyArrayInArray)
{
    std::string json;
    Json::Stream(json)
        << Json::BeginArray
        << Json::BeginArray
        << Json::EndArray
        << Json::EndArray;

    EXPECT_EQ(json, "[[]]");
}

TEST(JsonStream, WriteEmptyObjectInArray)
{
    std::string json;
    Json::Stream(json)
        << Json::BeginArray
        << Json::BeginObject
        << Json::EndObject
        << Json::EndArray;

    EXPECT_EQ(json, "[{}]");
}

TEST(JsonStream, Assert)
{
    std::string json;

    ASSERT_DEATH({
            Json::Stream(json)
                << Json::BeginObject
                << Json::BeginObject
                << Json::EndObject;
        },
        ".+"
    );
}

TEST(Json, Assert2)
{
    std::string json;
    
    ASSERT_DEATH({
            Json::Stream(json)
                << Json::BeginObject
                << Json::EndObject
                << Json::EndObject;
        },
        ".+"
    );
}

TEST(Json, ObjectKeyRequired)
{
    std::string json;
    
    ASSERT_DEATH({
            Json::Stream(json)
                << Json::BeginObject
                << Json::BeginArray
                << Json::EndObject;
        },
        ".+"
    );
}

TEST(Json, ObjectKeyAfterKey)
{
    std::string json;
    
    ASSERT_DEATH({
            Json::Stream(json)
                << Json::BeginObject
                << Json::Key{"key"}
                << Json::Key{"key"}
                << Json::EndObject;
        },
        ".+"
    );
}

// 

TEST(Json, Develop)
{
    std::string json;

    Json::Stream(json)
        << Json::BeginObject
        << Json::Key{"a"} << 10
        << Json::Key{"b"}
        << Json::BeginArray
        << 1
        << Json::BeginArray
        << Json::EndArray
        << 2
        << Json::EndArray
        << Json::EndObject;

    SUCCEED();
}