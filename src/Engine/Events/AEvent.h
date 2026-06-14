#pragma once

#include <functional>

template<typename... Args>
class AEvent
{
public:
	using Handler = std::function<void(Args...)>;
	using ListenerID = uint64_t;

	ListenerID AddListener(Handler handler)
	{
		ListenerID id = ++m_NextID;
		m_Listeners[id] = std::move(handler);
		return id;
	}

	void RemoveListener(ListenerID id)
	{
		m_Listeners.erase(id);
	}

	void Invoke(Args... args)
	{
		for (auto& [id, handler] : m_Listeners)
		{
			handler(args...);
		}
	}

private:
	std::unordered_map<ListenerID, Handler> m_Listeners;
	ListenerID m_NextID;
};
