#include "pch.h"
#include "gmock/gmock.h"

struct IThingy
{
    virtual ~IThingy() = 0;

    virtual void cheese() = 0;
};

IThingy::~IThingy()
{
}

class ThingyMock : public IThingy
{
public:
    MOCK_METHOD0(cheese, void());
};

class Subject
{
public:
    explicit Subject(std::unique_ptr<IThingy>&& thingy)
        : _thingy(std::move(thingy))
    {
    }

    void test()
    {
        _thingy->cheese();
    }
private:
    std::unique_ptr<IThingy> _thingy;
};

using ::testing::AtLeast;
using ::testing::Exactly;


TEST(TestCaseName, TestName) 
{
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);

  auto mock = std::make_unique<ThingyMock>();
  auto m = mock.get();
  Subject subject(std::move(mock));

  EXPECT_CALL(*m, cheese())
      .Times(Exactly(1));

  subject.test();
}