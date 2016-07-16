#pragma once
#include <vector>
#include<assert.h>
#include<iostream>
using namespace std;
// �º���
template <class T>
struct Less
{
	bool operator() (const T& l , const T& r )
	{
		return l < r;
	}
};

template <class T>
struct Greater
{
	bool operator() (const T& l , const T& r )
	{
		return l > r;
	}
};

template<class T, class Compare = Less<T> >// С��
class Heap
{
public:
	Heap()
	{}

	Heap(const T* a, size_t size)
	{
		assert(a);
		_a.reserve(size);
		for (size_t i = 0; i < size; ++i)
		{
			_a.push_back(a[i]);
		}
		
		// ���� N*lgN
		for (int i = (_a.size()-2)/2; i >=0; --i)
		{
			_AdjustDown(i);
		}
	}

	//
	void Push(const T& x)
	{
		_a.push_back(x);
		_AdjustUp(_a.size()-1);
	}

	// 
	void Pop()
	{
		assert(!_a.empty());
		swap(_a[0], _a[_a.size()-1]);
		_a.pop_back();
		_AdjustDown(0);
	}
	
	T& Top()
	{
		assert(!_a.empty());
		return _a[0];
	}

	int Size()
	{
		assert(!_a.empty());
		return _a.size();
	}

	bool Empty()
	{
		return _a.empty();
	}

protected:
	void _AdjustDown(size_t parent)
	{
		// ָ������
		size_t child = parent*2+1;
		Compare com;
		while (child < _a.size())
		{
			// ѡ�����Һ�����������һ��

			//if (child+1 < _a.size() 
			//	&&_a[child+1] > _a[child])
			if(child+1 < _a.size() && com(_a[child+1], _a[child]))
			{
				++child;
			}
			
			// ������ӱȸ��ڵ�Ҫ���򽻻����������µ���
			//if (_a[child] > _a[parent])
			if (com(_a[child],_a[parent]))
			{
				swap(_a[child], _a[parent]);
				parent = child;
				child = parent*2+1;
			}
			else
			{
				break;
			}
		}
	}
	
	//���ϵ�����Ӧ�����������ѻ���С�ѵ�����½��еĲ������
	void _AdjustUp(int child)
	{
		int parent = (child-1)/2;
		Compare com;
		while (child > 0)  //
		//while(parent >= 0)
		{
			//if (_a[child] > _a[parent])
			if (com(_a[child],_a[parent]))
			{
				swap(_a[child], _a[parent]);
				child = parent;
				parent = (child-1)/2;
			}
			else
			{
				break;
			}
		}
	}

protected:
	//T* _a;
	//size_t _size;
	//size_t _capacity;
	vector<T> _a;
};