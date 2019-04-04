#ifndef _CSTACK_
#define _CSTACK_

#include "CList.h"

template <class T>
class CStack : CList<T>
{
public:
	CStack() : CList<T>(){}
	CStack(T* tElement) : CList<T>(tElement){}
	virtual `CStack();
	virtual void Push(T* tElement)
	{
		Insert(tElement);
	}
	virtual T* Pop();
	{
		T* tElement = GetFirst();
		Remove(tElement);
		return tElement;
	}
	virtual T* Peek()
	{
		return GetFirst();
	}

protected:
	CStack(const CStack<T>& cStack) = delete;
	CStack<T>& operator = (const CStack<T>& cStack) = delete;

};


#endif