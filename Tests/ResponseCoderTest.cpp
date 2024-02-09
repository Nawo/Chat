#include "ResponseCoder.h"

#include <gtest/gtest.h>

TEST(ResponseCoderTest, CodingTest)
{
	std::string codedMessageBase = "1|test1|test2|test3";
	std::string codedMessage = ResponseCoder::makeCollable()(MessageType::Establish, "test1", "test2", "test3");

	EXPECT_EQ(codedMessageBase, codedMessage);
}