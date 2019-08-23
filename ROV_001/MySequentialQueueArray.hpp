/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#pragma once
template <class T>
class MySequentialQueueArray
{
public:
	MySequentialQueueArray(unsigned int max_capacity) 
	{
		this->max_capacity = max_capacity;
		this->Array = new T[max_capacity];
		this->size = this->front_index = this->back_index = 0;
	}

	~MySequentialQueueArray() 
	{
		delete this->Array;
	}

	void clear() {
		this->size = this->front_index = this->back_index = 0;
	}

	void push(T value) {
		if (this->size == 0) 
		{
			this->Array[this->back_index] = value;
			this->size++;
		}
		else if (this->size < this->max_capacity) 
		{
			this->back_index++;
			this->Array[this->back_index] = value;
			this->size++;
		}
		else if(this->size == this->max_capacity)
		{
			this->front_index = (this->front_index + 1) % this->max_capacity;
			this->back_index = (this->back_index + 1) % this->max_capacity;
			this->Array[this->back_index] = value;
		}
	}

	T pop(void) {
		if (this->size > 0)
		{
			unsigned int pop_index = this->front_index;
			this->front_index = (this->front_index + 1) % this->max_capacity;
			this->size--;
			return this->Array[pop_index];
		}
	}

	T getFront(void) {
		return this->Array[this->front_index];
	}

	T getBack(void) {
		return this->Array[this->back_index];
	}

	int getSize(void) {
		return this->size;
	}

	int getCapacity(void) {
		return this->max_capacity;
	}

	T operator[](unsigned int i) {
		if (i < this->size)
			return this->Array[(this->front_index + i) % this->size];
	}

// member
private:
	T* Array;
	unsigned int max_capacity;
	int front_index;
	int back_index;
	int size;
};
