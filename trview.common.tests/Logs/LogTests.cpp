#include <trview.common/Logs/Log.h>

using namespace trview;

TEST(Log, TopicsCreated)
{
    Log log;
    log.log(Message::Status::Information, "topic", "activity", "text");
    std::vector<std::string> expected_topics{ "topic" };
    ASSERT_EQ(log.topics(), expected_topics);
}

TEST(Log, ActivityCreated)
{
    Log log;
    log.log(Message::Status::Information, "topic", "activity", "text");
    log.log(Message::Status::Information, "topic", "activity 2", "text");
    std::vector<std::string> expected_activities{ "activity", "activity 2" };
    ASSERT_EQ(log.activities("topic"), expected_activities);
}

TEST(Log, TopicAndActivityFilter)
{
    Log log;
    log.log(Message::Status::Information, "topic", "activity", "text");
    log.log(Message::Status::Information, "topic", "activity 2", "text");
    log.log(Message::Status::Warning, "topic", "activity 2", "text 2");
    auto messages = log.messages("topic", "activity 2");
    ASSERT_EQ(messages.size(), 2);
    ASSERT_EQ(messages[0].text, "text");
    ASSERT_EQ(messages[0].status, Message::Status::Information);
    ASSERT_EQ(messages[0].topic, "topic");
    ASSERT_EQ(messages[1].text, "text 2");
    ASSERT_EQ(messages[1].status, Message::Status::Warning);
    ASSERT_EQ(messages[1].topic, "topic");
}

TEST(Log, AllMessages)
{
    Log log;
    log.log(Message::Status::Information, "topic", "activity", "text");
    log.log(Message::Status::Information, "topic", "activity 2", "text");
    log.log(Message::Status::Warning, "topic", "activity 2", "text 2");
    auto messages = log.messages();
    ASSERT_EQ(messages.size(), 3);
}
