#include <gtest/gtest.h>
#include <cstring>
#include <conops/util/cmd_queue.h>

TEST(CmdQueueTest, EnqueueOneElement)
{
	struct conops_cmd_queue queue;
	cmd_queue_init(&queue, nullptr, nullptr, nullptr);

	struct conops_cmd cmd;
	cmd.timestamp = 123;
	std::memset(cmd.payload, 0xAA, PAYLOAD_SIZE);

	EXPECT_EQ(cmd_queue_enqueue(&queue, &cmd), 0);
	EXPECT_FALSE(cmd_queue_is_empty(&queue));

	struct conops_cmd out{};
	EXPECT_EQ(cmd_queue_dequeue(&queue, 200, &out), 1);
	EXPECT_EQ(out.timestamp, 123U);
	EXPECT_EQ(std::memcmp(out.payload, cmd.payload, PAYLOAD_SIZE), 0);
}

TEST(CmdQueueTest, EnqueueFullQueue)
{
	struct conops_cmd_queue queue;
	cmd_queue_init(&queue, nullptr, nullptr, nullptr);

	struct conops_cmd cmd;
	for (uint16_t i = 0; i < CMD_QUEUE_CAPACITY; ++i) {
		ASSERT_EQ(cmd_queue_enqueue(&queue, &cmd), 0);
	}

	EXPECT_EQ(cmd_queue_enqueue(&queue, &cmd), -1)
		<< "A full queue should not allow another enqueue!";
}

TEST(CmdQueueTest, DequeueEmptyQueue)
{
	struct conops_cmd_queue queue;
	cmd_queue_init(&queue, nullptr, nullptr, nullptr);

	struct conops_cmd out{};
	EXPECT_EQ(cmd_queue_dequeue(&queue, 0, &out), 0);
}

TEST(CmdQueueTest, DequeueInOrder)
{
	struct conops_cmd_queue queue;
	cmd_queue_init(&queue, nullptr, nullptr, nullptr);

	struct conops_cmd cmd1;
	struct conops_cmd cmd2;
	struct conops_cmd cmd3;

	cmd1.timestamp = 100U;
	cmd2.timestamp = 200U;
	cmd3.timestamp = 300U;

	ASSERT_EQ(cmd_queue_enqueue(&queue, &cmd3), 0);
	ASSERT_EQ(cmd_queue_enqueue(&queue, &cmd2), 0);
	ASSERT_EQ(cmd_queue_enqueue(&queue, &cmd1), 0);

	conops_cmd out;
	ASSERT_EQ(cmd_queue_dequeue(&queue, 300, &out), 1);
	EXPECT_EQ(out.timestamp, 100U);

	ASSERT_EQ(cmd_queue_dequeue(&queue, 300, &out), 1);
	EXPECT_EQ(out.timestamp, 200U);

	ASSERT_EQ(cmd_queue_dequeue(&queue, 300, &out), 1);
	EXPECT_EQ(out.timestamp, 300U);

	EXPECT_TRUE(cmd_queue_is_empty(&queue));
}

TEST(CmdQueueTest, RandomOrderEnqueueDequeue)
{
	struct conops_cmd_queue queue;
	cmd_queue_init(&queue, nullptr, nullptr, nullptr);

	uint32_t timestamps[] = { 50, 20, 100, 80, 10 };
	uint32_t expected[] = { 10, 20, 50, 80, 100 };

	for (auto ts : timestamps) {
		struct conops_cmd cmd;
		cmd.timestamp = ts;
		ASSERT_EQ(cmd_queue_enqueue(&queue, &cmd), 0);
	}

	struct conops_cmd out;
	for (auto expected_ts : expected) {
		ASSERT_EQ(cmd_queue_dequeue(&queue, 200, &out), 1);
		EXPECT_EQ(out.timestamp, expected_ts);
	}

	EXPECT_TRUE(cmd_queue_is_empty(&queue));
}

TEST(CmdQueueTest, DequeueFutureTimestamp)
{
	struct conops_cmd_queue queue;
	cmd_queue_init(&queue, nullptr, nullptr, nullptr);

	struct conops_cmd cmd;
	cmd.timestamp = 500;

	ASSERT_EQ(cmd_queue_enqueue(&queue, &cmd), 0);

	struct conops_cmd out;
	EXPECT_EQ(cmd_queue_dequeue(&queue, 400, &out), 0)
		<< "Dequeues are only allowed if current time is past the command timestamp";
	EXPECT_EQ(cmd_queue_dequeue(&queue, 500, &out), 1);
	EXPECT_EQ(out.timestamp, 500U);
}

TEST(CmdQueueTest, DequeueInverseEnqueueOrder)
{
	struct conops_cmd_queue queue;
	cmd_queue_init(&queue, nullptr, nullptr, nullptr);

	struct conops_cmd cmd1;
	cmd1.timestamp = 500;

	struct conops_cmd cmd2;
	cmd2.timestamp = 200;

	struct conops_cmd cmd3;
	cmd3.timestamp = 125;

	ASSERT_EQ(cmd_queue_enqueue(&queue, &cmd2), 0);
	ASSERT_EQ(cmd_queue_enqueue(&queue, &cmd1), 0);

	struct conops_cmd out;

	EXPECT_EQ(cmd_queue_dequeue(&queue, 520, &out), 1);
	EXPECT_EQ(out.timestamp, 200U);

	ASSERT_EQ(cmd_queue_enqueue(&queue, &cmd3), 0);

	EXPECT_EQ(cmd_queue_dequeue(&queue, 550, &out), 1);
	EXPECT_EQ(out.timestamp, 125U);

	EXPECT_FALSE(cmd_queue_is_empty(&queue));
}

TEST(CmdQueueTest, QueueReset)
{
	struct conops_cmd_queue queue;
	cmd_queue_init(&queue, nullptr, nullptr, nullptr);

	struct conops_cmd cmd;
	cmd.timestamp = 1;

	for (uint16_t i = 0; i < CMD_QUEUE_CAPACITY; ++i) {
		ASSERT_EQ(cmd_queue_enqueue(&queue, &cmd), 0);
	}

	cmd_queue_reset(&queue);

	struct conops_cmd out;

	ASSERT_EQ(cmd_queue_dequeue(&queue, 1000, &out), 0)
		<< "After a queue reset and before another enqueue, no dequeue should be allow be performed!";
	ASSERT_TRUE(cmd_queue_is_empty(&queue)) << "After a queue reset the queue should be empty!";

	ASSERT_EQ(cmd_queue_enqueue(&queue, &cmd), 0)
		<< "Resetting the queue should prevent future enqueues!";

	ASSERT_EQ(cmd_queue_is_empty(&queue), false);
}

int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
