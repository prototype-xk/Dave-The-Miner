#ifndef MVECTOR_HPP
#define MVECTOR_HPP

namespace mstd {
	template<typename T>
	class Vector {
	private:
		T* m_data = nullptr;
		size_t m_size = 0;
		size_t m_capacity = 0;

		void realloc(size_t newCapacity);
		void push_back(const T& value);

	public:
		Vector();
		~Vector();
	};
}

template<typename T>
mstd::Vector<T>::Vector() {
	m_data = nullptr;
	m_size = 0;
	m_capacity = 0;

}

template<typename T>
mstd::Vector<T>::~Vector() {
	delete[] m_data;
	m_data = nullptr;
}

template<typename T>
void mstd::Vector<T>::realloc(size_t newCapacity) {
	T* newblock = new T[newCapacity];
	for (int x = 0; x < m_size; x++) {
		newblock[x] = m_data[x];
	}
	delete[] m_data;

	m_data = newblock;
	m_capacity = newCapacity;
}

template<typename T>
void mstd::Vector<T>::push_back(const T& value) {
	if (m_size == m_capacity) {
		
	}
}

#endif