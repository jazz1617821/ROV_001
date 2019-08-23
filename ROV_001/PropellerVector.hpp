/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#pragma once
#include <iostream>
#include <algorithm>

enum PropellerName { SV, PV, SF, PF, SA, PA, NumPropeller };

class PropellerVector
{
public:
	PropellerVector(float sv = 0, float pv = 0, float sf = 0, float pf = 0, float sa = 0, float pa = 0);
	~PropellerVector();
	float clamp(float value);
	float getVectorComponent(int Name);

private:
	float propeller[NumPropeller];

public:
	PropellerVector operator+(PropellerVector a);
	PropellerVector operator*(float scaler);
	void operator=(PropellerVector a);
	void operator+=(PropellerVector a);
};

