/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#include "PropellerVector.hpp"

PropellerVector::PropellerVector(float sv, float pv, float sf, float pf, float sa, float pa)
{
	this->propeller[SV] = sv;
	this->propeller[PV] = pv;
	this->propeller[SF] = sf;
	this->propeller[PF] = pf;
	this->propeller[SA] = sa;
	this->propeller[PA] = pa;
}

PropellerVector::~PropellerVector()
{

}

float PropellerVector::clamp(float value)
{
	return std::max(-1000.0f, std::min(value, 1000.0f));
}

float PropellerVector::getVectorComponent(int Name)
{
	return propeller[Name];
}

PropellerVector PropellerVector::operator+(PropellerVector a)
{
	PropellerVector temp(
		this->propeller[SV] + a.propeller[SV],
		this->propeller[PV] + a.propeller[PV],
		this->propeller[SF] + a.propeller[SF],
		this->propeller[PF] + a.propeller[PF],
		this->propeller[SA] + a.propeller[SA],
		this->propeller[PA] + a.propeller[PA]
	);
	return temp;
}

PropellerVector PropellerVector::operator*(float scaler)
{
	PropellerVector temp(
		this->propeller[SV] * scaler,
		this->propeller[PV] * scaler,
		this->propeller[SF] * scaler,
		this->propeller[PF] * scaler,
		this->propeller[SA] * scaler,
		this->propeller[PA] * scaler
	);
	return temp;
}

void PropellerVector::operator=(PropellerVector a)
{
	this->propeller[SV] = a.propeller[SV];
	this->propeller[PV] = a.propeller[PV];
	this->propeller[SF] = a.propeller[SF];
	this->propeller[PF] = a.propeller[PF];
	this->propeller[SA] = a.propeller[SA];
	this->propeller[PA] = a.propeller[PA];
}

void PropellerVector::operator+=(PropellerVector a)
{
	this->propeller[SV] = this->propeller[SV] + a.propeller[SV];
	this->propeller[PV] = this->propeller[PV] + a.propeller[PV];
	this->propeller[SF] = this->propeller[SF] + a.propeller[SF];
	this->propeller[PF] = this->propeller[PF] + a.propeller[PF];
	this->propeller[SA] = this->propeller[SA] + a.propeller[SA];
	this->propeller[PA] = this->propeller[PA] + a.propeller[PA];
}
