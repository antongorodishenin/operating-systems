#include <iostream>
#include <time.h>
#include "SyncQueue.cpp"

using namespace std;

HANDLE start;

struct ForProducer
{
	SyncQueue* queue;
	int number;
	int count;
};

struct ForConsumer
{
	SyncQueue* queue;
	int count;
};

DWORD WINAPI producer(LPVOID par)
{
	WaitForSingleObject(start, INFINITE);

	ForProducer* data = (ForProducer*)par;


	for (int i = 0; i < data->count; i++) {
		data->queue->Insert(data->number + i);
		cout << data->number + i << "\n";
		Sleep(70);
	}

	return 0;
}

DWORD WINAPI consumer(LPVOID par)
{
	WaitForSingleObject(start, INFINITE);

	ForConsumer* data = (ForConsumer*)par;

	for (int i = 0; i < data->count; i++) {
		int number = data->queue->Remove();
		cout << -1*number << "\n";
		Sleep(7);
	}

	return 0;
}

int main()
{
	int queueSize;
	cout << "Queue size: ";
	cin >> queueSize;

	start = CreateEvent(NULL, TRUE, FALSE, NULL);

	SyncQueue* queue = new SyncQueue(queueSize);

	int producerNumber;
	cout << "Producer number: ";
	cin >> producerNumber;

	ForProducer* arrayForProducer = new ForProducer[producerNumber];
	for (int i = 0; i < producerNumber; i++) {
		cout << "Number of elements produced by thread " << i + 1 << ": ";
		int count;
		cin >> count;
		arrayForProducer[i].count = count;
		arrayForProducer[i].number = i + 1;
		arrayForProducer[i].queue = *&queue;
	}

	HANDLE* prodThreadHandles = new HANDLE[producerNumber];
	DWORD* prodThreadID = new DWORD[producerNumber];

	for (int i = 0; i < producerNumber; i++) {
		prodThreadHandles[i] = CreateThread(NULL, 0, producer, (void*)&arrayForProducer[i], 0, &prodThreadID[i]);
	}

	int consThreadCount;
	cout << "Consumer count: ";
	cin >> consThreadCount;

	ForConsumer* arrayForConsumer = new ForConsumer[consThreadCount];
	for (int i = 0; i < consThreadCount; i++) {
		cout << "Number of elements consumed by thread " << i + 1 << ": ";
		int count;
		cin >> count;
		arrayForConsumer[i].count = count;
		arrayForConsumer[i].queue = *&queue;
	}

	HANDLE* consThreadHandles = new HANDLE[consThreadCount];
	DWORD* consThreadID = new DWORD[consThreadCount];

	for (int i = 0; i < consThreadCount; i++) {
		consThreadHandles[i] = CreateThread(NULL, 0, consumer, (void*)&arrayForConsumer[i], 0, &consThreadID[i]);
	}

	HANDLE* arrayHandles = new HANDLE[producerNumber + consThreadCount];

	for (int i = 0; i < producerNumber; i++) {
		arrayHandles[i] = prodThreadHandles[i];
	}

	for (int i = 0; i < consThreadCount; i++) {
		arrayHandles[i + producerNumber] = consThreadHandles[i];
	}

	SetEvent(start);

	WaitForMultipleObjects(producerNumber + consThreadCount, arrayHandles, TRUE, INFINITE);

	system("pause");

	return 0;
}