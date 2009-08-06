// Copyright (C) 2004 by Olivier Galizzi <olivier.galizzi@imag.fr>
//  Copyright (C) 2004 by Janek Kozicki <cosurgi@berlios.de>
//
#pragma once
#include<yade/lib-serialization/Serializable.hpp>
#include"InteractionGeometry.hpp"
#include"InteractionPhysics.hpp"



/////////////////////////////////
// FIXME - this is in wrong file!
//#include<boost/strong_typedef.hpp>
//BOOST_STRONG_TYPEDEF(int, body_id_t)
typedef int body_id_t;

class InteractionGeometryEngineUnit;
class InteractionPhysicsEngineUnit;
class ConstitutiveLaw;
class MetaBody;

class Interaction : public Serializable
{
	private	:
		body_id_t id1,id2;
		//! Step number at which the interaction was fully created (interactionGeometry and interactionPhysics).
		//! Should be touched only by InteractionPhysicsMetaEngine and InteractionDispatchers, making them friends therefore
		long iterMadeReal;
		friend class InteractionPhysicsMetaEngine;
		friend class InteractionDispatchers;
	public :
		bool isReal() const {return (bool)interactionGeometry && (bool)interactionPhysics;}
		//! If this interaction was just created in this step (for the constitutive law, to know that it is the first time there)
		bool isFresh(MetaBody* rb);

		//! phase flag to mark (for example, SpatialQuickSortCollider mark by it the stale interactions) 
		bool cycle;      
		//! NOTE : TriangulationCollider needs this (nothing else)
		bool isNeighbor;

		//! relative distance between bodies, given in (MetaBody::cellMax-MetaBody::cellMin) units
		//! Position of id1 must be incremented by that distance so that there is spatial interaction 
		Vector3<int> cellDist;

		shared_ptr<InteractionGeometry> interactionGeometry;
		shared_ptr<InteractionPhysics> interactionPhysics;

		Interaction ();
		Interaction(body_id_t newId1,body_id_t newId2);

		const body_id_t& getId1() const {return id1;};
		const body_id_t& getId2() const {return id2;};

		//! swaps order of bodies within the interaction
		void swapOrder();

		bool operator<(const Interaction& other) const { return getId1()<other.getId1() || (getId1()==other.getId1() && getId2()<other.getId2()); }

		//! cache functors that are called for this interaction. Currently used by InteractionDispatchers.
		struct {
			// Whether geometry dispatcher exists at all; this is different from !geom, since that can mean we haven't populated the cache yet.
			// Therefore, geomExists must be initialized to true first (done in Interaction::reset() called from ctor).
			bool geomExists;
			// shared_ptr's are initialized to NULLs automagically
			shared_ptr<InteractionGeometryEngineUnit> geom;
			shared_ptr<InteractionPhysicsEngineUnit> phys;
			shared_ptr<ConstitutiveLaw> constLaw;
		} functorCache;

		//! Reset interaction to the intial state (keep only body ids)
		void reset();
		//! common initialization called from both constructor and reset()
		void init();
			
	REGISTER_ATTRIBUTES(Serializable,
		(id1)
		(id2)
		(iterMadeReal)
		(interactionGeometry)
		(interactionPhysics)
		(cellDist)
	);
	REGISTER_CLASS_AND_BASE(Interaction,Serializable);
};

REGISTER_SERIALIZABLE(Interaction);
