#include <Windows.h>
#include <iostream>
#include <tchar.h>

using namespace std;

#define THREADS_NUMBER 2
#define ELEMENTS_NUMBER 200
#define BLOCK_SIZE ELEMENTS_NUMBER / THREADS_NUMBER
#define MAX_VALUE 1000

typedef struct _tagARRAYOBJECT
{
	int* iArray;
	int iSize;
	int iThreadID;
} ARRAYOBJECT,*PARRAYOBJECT;

DWORD WINAPI ThreadStart(LPVOID lpParameter);
void PrintArray(int* iArray, int iSize);
void MergeArrays(int* leftArray, int leftArrayLength, int* rightArray, int rightArrayLength, int* mergedArray);

int _tmain(int argc, _TCHAR* argv[])
{
	int iArray1[BLOCK_SIZE];
	int iArray2[BLOCK_SIZE];
	int iArray[ELEMENTS_NUMBER];

	for (int iIndex = 0; iIndex < BLOCK_SIZE; iIndex++)
	{
		iArray1[iIndex] = rand() % MAX_VALUE;
		iArray2[iIndex] = rand() % MAX_VALUE;
	}
	PrintArray(iArray1, BLOCK_SIZE);
	PrintArray(iArray2, BLOCK_SIZE);

	HANDLE hThreads[THREADS_NUMBER];
	ARRAYOBJECT pObject1 = { iArray1, BLOCK_SIZE, 0 };
	hThreads[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadStart, (LPVOID)&pObject1, 0, NULL);

	ARRAYOBJECT pObject2 = { iArray2, BLOCK_SIZE, 0 };
	hThreads[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadStart, (LPVOID)&pObject2, 0, NULL);

	cout << "Waiting execution..." << endl;
	WaitForMultipleObjects(THREADS_NUMBER, hThreads, TRUE, INFINITE);

	MergeArrays(iArray1, BLOCK_SIZE, iArray2, BLOCK_SIZE, iArray);
	PrintArray(iArray,ELEMENTS_NUMBER);

	CloseHandle(hThreads[0]);
	CloseHandle(hThreads[1]);

	cout << "Array sorted..." << endl;
	system("pause");
	return 0;
}

DWORD WINAPI ThreadStart(LPVOID lpParameter)
{
	PARRAYOBJECT pObject = (PARRAYOBJECT)lpParameter;
	int iTmp = 0;
	for (int iIndex = 0; iIndex < pObject->iSize; iIndex++)
	{
		for (int iEndIndex = pObject->iSize - 1; iEndIndex > iIndex; iEndIndex--)
		{
			if (pObject->iArray[iEndIndex] < pObject->iArray[iIndex])
			{
				swap(pObject->iArray[iEndIndex], pObject->iArray[iIndex]);
			}
		}
	}
	return 0;
}

void PrintArray(int* iArray, int iSize)
{
	for (int iIndex = 0; iIndex < iSize; iIndex++)
	{
		cout << iArray[iIndex] << " ";
	}
	cout << endl;
}

void MergeArrays(int* leftArray, int leftArrayLength, int* rightArray, int rightArrayLength, int* mergeArray)
{
	int i = 0;
	int j = 0;
	int k = 0;
	while (i < leftArrayLength && j < rightArrayLength)
	{
		if (leftArray[i] < rightArray[j])
		{
			mergeArray[k] = leftArray[i];
			i++;
		}
		else
		{
			mergeArray[k] = rightArray[j];
			j++;
		}
		k++;
	}

	if (i >= leftArrayLength)
	{
		while (j < rightArrayLength)
		{
			mergeArray[k] = rightArray[j];
			j++;
			k++;
		}
	}
	else if (j >= rightArrayLength)
	{
		while (i < leftArrayLength)
		{
			mergeArray[k] = leftArray[i];
			i++;
			k++;
		}
	}
}