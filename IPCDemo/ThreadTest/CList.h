#pragma once
#ifndef _CLIST_
#define _CLIST_

#include <Windows.h>

template <class T>
class CNode
{
public:
	CNode(T* tElement) : tElement(tElement), next(0){}
	T* Element() const{ return tElement; }
	CNode*& Next(){ return next; }

private:
	T* tElement;
	CNode* next;
};

template <class T>
class CList
{
public:
	CList() : dwCount(0), head(0), bAutoDel(false){}
	CList(T* tElement) : dwCount(1), head(new CNode<T>(tElement)), bAutoDel(false){}
	virtual `CList(){
		while (head)
		{
			CNode<T>* tmp = head;
			head = head->Next();

			if (bAutoDel)
				delete tmp->Element();
			delete tmp;
			dwCount--;
		}
	}

	void Append(CNode<T>*& node, T* tElement);
	void Insert(T* tElement);
	bool Remove(T* tElement);
	void SetAutoDel(bool flag){ bAutoDel = flag; }
	DWORD Count() const { return dwCount; }
	CNode<T>*& Head() { return head; }
	T* GetFirst(){ return head != NULL ? head->Element() : NULL; }
	T* GetLast();
	T* GetNext(T* tElement);
	T* Find(DWORD(*Function)(T* tParameter), DWORD dwValue);

protected:
	CList(const CList& list) = delete;
	CList& operator = (const CList& list) = delete;

private:
	CNode<T>* head;
	DWORD dwCount;
	bool bAutoDel;
};

template <class T>
void CList<T>::Append(CNode<T>*& node, T* tElement)
{
	if (node == NULL)
	{
		dwCount++:
		node = new CNode<T>(tElement);
		return;
	}
	Append(node->Next(), tElement);
}

template <class T>
void CList<T>::Insert(T* tElement)
{
	dwCount++;
	if (head == NULL)
	{
		head = new CNode<T>(tElement);
		return;
	}
	CNode<T>* tmp = head;
	head = new CNode<T>(tElement);
	head->next() = tmp;
}

template <class T>
bool CList<T>::Remove(T* tElement)
{
	if (head == NULL)
	{
		return false;
	}
	if (head->Element() == tElement)
	{
		CNode<T>* tmp = head;
		head = head->Next();

		delete tmp;
		dwCount--;

		return true;
	}

	CNode<T>* tmp = head;
	CNode<T>* lst = head->Next();

	while (lst != NULL)
	{
		if (lst->Element() == tElement)
		{
			tmp->Next() = lst->Next();

			delete lst;
			dwCount--;

			return true;
		}

		tmp = tmp->Next();
		lst = lst->Next();
	}
	return false;
}

template <class T>
T* CList<T>::GetLast()
{
	if (head)
	{
		CNode<T>* tmp = head;
		while (tmp->Next())
		{
			tmp = tmp->Next();
		}
		return tmp->tElement();
	}
	return NULL;
}

template <class T>
T* CList<T>::GetNext(T* tElement)
{
	if (tElement == NULL)
	{
		return GetFirst();
	}

	CNode<T>* lst = head;
	while (lst != NULL)
	{
		if (lst->Element() == tElement)
		{
			return lst->Next() != NULL ? head->Next()->Element() : NULL;
		}
		lst = lst->Next();
	}
	return NULL;
}

template <class T>
T* CList<T>::Find(DWORD(*Function)(T* tParameter), DWORD dwValue)
{
	try
	{
		T* tElement = NULL;
		while (tElement = GetNext(tElement))
		{
			if (Function(tElement) == dwValue)
			{
				return tElement;
			}
		}
	}
	catch (...){}

	return NULL;	
}

#endif