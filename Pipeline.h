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

	Pipeline<T> &AddFilter(ApplyFunction stage);
	Pipeline<T> &operator|(ApplyFunction stage);

	Pipeline<T> &AddStage(std::pair<ApplyFunction, RollbackFunction> stage);
	bool operator()(T &message);

private:
	std::vector<std::pair<ApplyFunction, RollbackFunction> > _stages;
};

template<class T>
Pipeline<T> &Pipeline<T>::AddFilter(ApplyFunction stage)
{
	return AddStage(std::make_pair(stage, RollbackFunction()));
}

template<class T>
Pipeline<T> &Pipeline<T>::operator|(ApplyFunction stage)
{
	return AddStage(std::make_pair(stage, RollbackFunction()));
}

template<class T>
Pipeline<T> &Pipeline<T>::AddStage(std::pair<ApplyFunction, RollbackFunction> stage)
{
	_stages.push_back(stage);
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