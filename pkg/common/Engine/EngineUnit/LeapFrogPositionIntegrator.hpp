/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef LEAPFROG_POSITION_INTEGRATOR_HPP
#define LEAPFROG_POSITION_INTEGRATOR_HPP 

#include "PhysicalParametersEngineUnit.hpp"

class LeapFrogPositionIntegrator : public PhysicalParametersEngineUnit
{
	private :
		vector<Vector3r> prevVelocities;
		vector<bool> firsts;
	
	public :
		virtual void go(	  const shared_ptr<PhysicalParameters>&
					, Body*);
	
	REGISTER_CLASS_NAME(LeapFrogPositionIntegrator);
	REGISTER_BASE_CLASS_NAME(PhysicalParametersEngineUnit);
};

REGISTER_SERIALIZABLE(LeapFrogPositionIntegrator,false);

#endif // LEAPFROG_POSITION_INTEGRATOR_HPP

