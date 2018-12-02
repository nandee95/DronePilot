#pragma once

template <typename T>
class ScopedPtr
{
public:
	T* ptr;
	ScopedPtr(T* ptr) : ptr(ptr)
	{

	}

	ScopedPtr() : ptr(nullptr)
	{

	}

	void operator = (T* ptr)
	{
		if (this->ptr != nullptr) delete this->ptr;
		this->ptr = ptr;
	}
	T& operator *()
	{
		return *ptr;
	}

	T* operator ->()
	{
		return ptr;
	}

	~ScopedPtr()
	{
		delete ptr;
	}
};