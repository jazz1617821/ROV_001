/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Lab 503 Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#include "MyPID.hpp"

MyPID::MyPID(float Kp, float Ki, float Kd) : error_queue(100)
{
	this->Kp = Kp;
	this->Ki = Ki;
	this->Kd = Kd;
	error_sum = 0;
}


MyPID::~MyPID()
{

}

float MyPID::processesPID(float current_error_value, float time_interval)
{
	/*if (error_queue.getSize() == error_queue.getCapacity()) {
		error_sum -= error_queue.getFront();
	}
	error_sum += error_queue.getBack();*/
	this->error_queue.push(current_error_value);
	error_sum = 0;
	for (int i = error_queue.getSize(), j = 0; i > 0, j < 5; i--, j++) {
		error_sum += error_queue[i - 1];
	}


	float u;
	float P = Kp * error_queue.getBack();
	float I = Ki * error_sum;
	float D = Kd * time_interval * (error_queue.getBack() - error_queue[error_queue.getSize() - 2]);


	//if (I < 1.0) {
		u = P + I + D;
	//}
	//else {
	//	u = P + D;
	//}


	return u;
}
