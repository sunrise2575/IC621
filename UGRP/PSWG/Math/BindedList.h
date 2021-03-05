#pragma once

#include <Prelude.h>

template <typename _Ty> class __BindedList{

private:

	unsigned __maxCount;
	unsigned __count;
	_Ty* __data;

public:

	__BindedList(const unsigned maxCount);
	__BindedList(const __BindedList& copy);
	__BindedList& operator=(const __BindedList& copy);
	~__BindedList();

	_Ty& operator[](unsigned index); // data에 직접 참조

	unsigned inline count() const;
	unsigned maxCount() const;
	unsigned push(const _Ty& Data);
	void push(const _Ty& Data, unsigned index);

	_Ty pop(unsigned index);
	_Ty pop_noswap(unsigned index);
	void print(bool onlyValue = true);

};

template <typename _Ty> __BindedList<_Ty>::__BindedList(unsigned maxCount)
{
	__maxCount = maxCount;
	__count = 0;
	__data = new _Ty[__maxCount];
}

template <typename _Ty> __BindedList<_Ty>::__BindedList(const __BindedList& copy)
{
	__maxCount = copy.__maxCount;
	__count = copy.__count;
	__data = new _Ty[__maxCount];
	for (int i = 0; i < __maxCount; i++) { __data[i] = copy.__data[i]; }
}

template <typename _Ty> __BindedList<_Ty>& __BindedList<_Ty>::operator=(const __BindedList& copy)
{
	__maxCount = copy.__maxCount;
	__count = copy.__count;
	__data = new _Ty[__maxCount];
	for (int i = 0; i < __maxCount; i++) { __data[i] = copy.__data[i]; }
	return *this;
}

template <typename _Ty> __BindedList<_Ty>::~__BindedList()
{
	__maxCount = 0;
	__count = 0;
	delete[] __data;
}

template <typename _Ty> _Ty& __BindedList<_Ty>::operator[](unsigned index)
{
	//if (index >= __count) { throw Particles_OutOfIndex; }
	return __data[index];
}// data에 직접 참조

template <typename _Ty> inline unsigned __BindedList<_Ty>::count() const { return __count; }
template <typename _Ty> unsigned __BindedList<_Ty>::maxCount() const { return __maxCount; }

template <typename _Ty> unsigned __BindedList<_Ty>::push(const _Ty &Data)
{
	if (__maxCount == __count) { throw BindedList_NoMoreSpace; }
	__data[__count] = Data;
	__count++;
	return __count;
}

template <typename _Ty> void __BindedList<_Ty>::push(const _Ty &Data,unsigned index)
{
	if (__maxCount == __count) { throw BindedList_NoMoreSpace; }
	else if (index > __count) { throw BindedList_JumpingInsert; }
	else
	{
		if (index < __count)
		{
			memcpy(__data + index + 1, __data + index, sizeof(_Ty)*(__count - index));
		}
		__data[index] = Data;
		__count++;
	}
}

template <typename _Ty> _Ty __BindedList<_Ty>::pop(unsigned index)
{
	if (index >= __count) { throw BindedList_NoSuchEntity; }
	else if (index == __count - 1) { __count--;  return __data[index]; }
	else
	{
		_Ty results = __data[index];
		__data[index] = __data[__count - 1];
		__count--;
		return results;
	}
}


template <typename _Ty> _Ty __BindedList<_Ty>::pop_noswap(unsigned index)
{
	if (index >= __count) { throw BindedList_NoSuchEntity; }
	else if (index == __count - 1) { __count--;  return __data[index]; }
	else
	{
		_Ty results = __data[index];
		__count--;
		memcpy(__data + index, __data + index + 1, sizeof(_Ty)*(__count - index));
		return results;
	}
}

template <typename _Ty> void __BindedList<_Ty>::print(bool onlyValue)
{
	printf("\n");

	if (!onlyValue)
	{
		printf("+---------+---------+\n");
		printf("|count of maxcount |%8d/%-8d\t|\n", __count, __maxCount);
		printf("+---------+---------+\n");
	}
	if (__count > 0)
	{
		printf("+");  for (int i = 0; i < __count - 1; i++){ printf("---+"); } printf("---+\n");
		printf("|");  for (int i = 0; i < __count - 1; i++){ printf("%6d|", i); } printf("%6d|\n", __count - 1);
		printf("+");  for (int i = 0; i < __count - 1; i++){ printf("---+"); } printf("---+\n");
		printf("|");  for (int i = 0; i < __count - 1; i++){ std::cout << __data[i]; printf("\t|", i); } std::cout << __data[__count-1]; printf("\t|\n", __count - 1);
		printf("+");  for (int i = 0; i < __count - 1; i++){ printf("---+"); } printf("---+\n");
	}
	else
	{
		printf("NO DATA");
	}
}
