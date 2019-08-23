/*
 * ROV_001
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University
 * Department of Computer Science and Engineering
 * Computer Graphics Lab Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */
#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// cw 
// power to force(kgf) function
// y = -0.0000231268185944738 * x * x + 0.0404671849903396 * x
// 
// force to power function
// max fooce = 17.3415532834 at 750(W)
// force = 7.99976983868 at power = 227.180909931
// x = -(sqrt ( 7155891301 ) * sqrt( 138118163120201831546629-7802264870491430032893 * y ) - 31438170461748427 ) / 35933552847057 


// ccw
// power to force(kgf) function
// y = -0.0000148224469843339 * x * x + 0.0217831950231616 * x
//
// force to power function
// max fooce = 7.99976983868 at 750(W)
// x = -( 3800 * sqrt( 595 ) * sqrt( 723060689776852721875-90346532447970149166 * y ) - 2492467218325000 ) / 3392015098293


#define Max_Force_KGF_ClockWise 17.3415532834
#define Max_Force_KGF_CunterClockWise 7.99976983868

#define ClockWise 1
#define CunterClockWise 0

class Propeller
{
public:
	Propeller();
	~Propeller();

	void initialParameter(glm::vec4 force_position,glm::vec4 force_vector);
	float getForce(float scaler);
	float getPower(float scaler);
	float getRPM(float scaler);
	glm::vec3 calculateForceVector(float scaler);

	float force2power(int force, bool state) {
		if (state == ClockWise) {
			return -(sqrt(753895261) * sqrt(30140980566320869 - 1702656250000000 * force) - 4766879735303) / 548500000;
		}
		else {
			return -(sqrt(14837765991) * sqrt(11000993328970040031 - 1374575000000000000 * force) - 404017530168939) / 549830000000;
		}
	}

	float force_to_rpm_clockwise(int force) {
		return -(sqrt(7155891301) * sqrt((13811816312020 - 780226487049) * force * 10000000000) - 31438170461748427) / 35933552847057;
	}

	float force_to_rpm_cunterclockwise(int force) {
		return -(3800 * sqrt(595) * sqrt(72306068977 - 9034653244 * force * 10000000000) - 2492467218325000) / 3392015098293;
	}

	// All in body space
	glm::vec4 force_vector;
	glm::vec4 force_position;
};

