#include <cstdlib>
#include <framework/Pipeline.h>

struct Addition_t
{
	int *op1;
	int *op2;
	int result;
};

static auto freeMessage = [](Addition_t &msg) -> void
{
	free(msg.op1);
	free(msg.op2);

	msg.op1 = nullptr;
	msg.op2 = nullptr;
	msg.result = 0;
};

static auto getOp1 = [](Addition_t &msg) -> bool
{
	msg.op1 = reinterpret_cast<int*>(malloc(sizeof(int)));
	if(msg.op1)
		*msg.op1 = 1;

	return msg.op1 != nullptr;
};

static auto getOp2 = [](Addition_t &msg) -> bool
{
	msg.op2 = reinterpret_cast<int*>(malloc(sizeof(int)));
	if(msg.op2)
		*msg.op2 = 2;

	return msg.op2 != nullptr;
};

static auto addOperands = [](Addition_t &msg) -> bool
{
	msg.result = *msg.op1 + *msg.op2;
	return true;
};

static auto adder = Pipeline<Addition_t>()
	.AddStage([](Addition_t&){ return true; }, freeMessage)
	.AddStage(getOp1)
	.AddStage(getOp2)
	.AddStage(addOperands);

int main(int argc, char **argv)
{
	Addition_t msg = {0};
	if(adder(msg))
	{
		freeMessage(msg);
		return 0;
	}

	return -1;
}