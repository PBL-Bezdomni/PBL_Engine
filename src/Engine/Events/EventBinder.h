#pragma once

#include <functional>
#include <vector>

class EventBinder
{
public:
	~EventBinder()
	{
		// UnbindAll();
	}

	template<typename EventType, typename Callable>
	void Bind(EventType& event, Callable&& callable)
	{
		auto id = event.AddListener(std::forward<Callable>(callable));

		m_Unbinders.push_back([&event, id]()
		{
			event.RemoveListener(id);
		});
	}

	void UnbindAll()
	{
		for (auto& unbind : m_Unbinders)
		{
			unbind();
		}

		m_Unbinders.clear();
	}

private:
	std::vector<std::function<void()>> m_Unbinders;
};
