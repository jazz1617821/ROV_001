/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#pragma once
#include "MySequentialQueueArray.hpp"

class MyPID
{
// method
public:
	MyPID(float Kp, float Ki, float Kd);
	~MyPID();
	 
	float	processesPID(float current_state_value, float time_interval);

// member
private:
	// PID parameter
	float	Kp;
	float	Ki;
	float	Kd;
	// u(t) parameter
	float	target_value;
	// e(t) error parameter
	MySequentialQueueArray<float> error_queue;
	float	error_sum;

public:


};

