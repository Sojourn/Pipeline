#ifndef PIPELINE_H
#define PIPELINE_H

#include <functional>
#include <vector>

template<class T>
class Pipeline
{
public:
	typedef typename std::function<bool (T &)> ApplyFunction;
	typedef typename std::function<void (T &)> RollbackFunction;

	Pipeline<T> &AddStage(
		ApplyFunction applyFunc,
		RollbackFunction rollbackFunc = RollbackFunction());

	bool operator()(T &message);

private:
	std::vector<std::pair<ApplyFunction, RollbackFunction> > _stages;
};

template<class T>
Pipeline<T> &Pipeline<T>::AddStage(ApplyFunction applyFunc, RollbackFunction rollbackFunc)
{
	_stages.push_back(std::make_pair(applyFunc, rollbackFunc));
	return *this;
}

template<class T>
bool Pipeline<T>::operator()(T &message)
{
	const int32_t count = _stages.size();
	int32_t i;

	for(i = 0; i < count; i++)
	{
		auto &stage = _stages[i].first;
		if(!stage(message))
			break;
	}

	if(i != count)
	{
		for(i--; i >= 0; i--)
		{
			auto &rollback = _stages[i].second;
			if(rollback)
				rollback(message);
		}
	}

	return i == count;
}

#endif // PIPELINE_H