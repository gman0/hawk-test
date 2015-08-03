#ifndef _id_store_h_
#define _id_store_h_

#include <unordered_map>
#include <utility>
#include <tuple>

template <typename T>
class Id_store
{
public:
	using Container_type = std::unordered_map<unsigned, T>;

private:
	unsigned m_last_id;
	Container_type m_store;

public:
	Id_store() : m_last_id{1} {}

	template <typename... Args>
	unsigned insert(Args&&... args)
	{
		m_store.emplace(m_last_id, std::forward<Args>(args)...);
		return m_last_id++;
	}

	unsigned last_id() const { return m_last_id - 1; }

	T& operator[](unsigned id) { return m_store.at(id); }
	bool erase(unsigned id) { return m_store.erase(id) != 0; }
	Container_type& get_container() { return m_store; }
};

#endif // _id_store_h_
