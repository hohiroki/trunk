#include "Cylinder.hpp"
#include<yade/pkg/common/Sphere.hpp>
#ifdef YADE_OPENGL
	#include<yade/lib/opengl/OpenGLWrapper.hpp>
#endif
#include<yade/pkg/common/Aabb.hpp>
#include<yade/pkg/dem/FrictPhys.hpp>
#include "../../lib/base/Math.hpp"

Cylinder::~Cylinder(){}
ChainedCylinder::~ChainedCylinder(){}
ChainedState::~ChainedState(){}
CylScGeom::~CylScGeom(){}
// Ig2_Sphere_ChainedCylinder_CylScGeom::~Ig2_Sphere_ChainedCylinder_CylScGeom() {}
// Ig2_ChainedCylinder_ChainedCylinder_ScGeom6D::~Ig2_ChainedCylinder_ChainedCylinder_ScGeom6D() {}

YADE_PLUGIN((Cylinder)(ChainedCylinder)(ChainedState)(CylScGeom)(Ig2_Sphere_ChainedCylinder_CylScGeom)(Ig2_ChainedCylinder_ChainedCylinder_ScGeom6D)(Law2_CylScGeom_FrictPhys_CundallStrack)
	#ifdef YADE_OPENGL
		(Gl1_Cylinder)(Gl1_ChainedCylinder)
	#endif
	(Bo1_Cylinder_Aabb)(Bo1_ChainedCylinder_Aabb)
);

vector<vector<int> > ChainedState::chains;
unsigned int ChainedState::currentChain=0;

//!##################	IG FUNCTORS   #####################


//!Sphere-cylinder or cylinder-cylinder not implemented yet, see Ig2_ChainedCylinder_ChainedCylinder_ScGeom6D and test/chained-cylinder-spring.py
bool Ig2_Sphere_ChainedCylinder_CylScGeom::go(	const shared_ptr<Shape>& cm1,
							const shared_ptr<Shape>& cm2,
							const State& state1, const State& state2, const Vector3r& shift2, const bool& force,
							const shared_ptr<Interaction>& c)
{
	const State* sphereSt=YADE_CAST<const State*>(&state1);
	const ChainedState* cylinderSt=YADE_CAST<const ChainedState*>(&state2);
	ChainedCylinder *cylinder=YADE_CAST<ChainedCylinder*>(cm2.get());
	Sphere *sphere=YADE_CAST<Sphere*>(cm1.get());
	assert(sphereSt && cylinderSt && cylinder && sphere);
	bool isLast = (cylinderSt->chains[cylinderSt->chainNumber].size()==(cylinderSt->rank+1));
	bool isNew = !c->geom;

	shared_ptr<const ChainedState> statePrev;
	if (cylinderSt->rank>0)
		statePrev = YADE_PTR_CAST<const ChainedState> (Body::byId(cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank-1],scene)->state);

	shared_ptr<CylScGeom> scm;
	if (!isNew) {scm=YADE_PTR_CAST<CylScGeom>(c->geom);}

	//FIXME : definition of segment in next line breaks periodicity
	shared_ptr<Body> cylinderNext;
	Vector3r segment, branch, direction;
	Real length, dist;
	branch = sphereSt->pos-cylinderSt->pos-shift2;
	if (!isLast) {
		cylinderNext = Body::byId(cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank+1],scene);
		segment = cylinderNext->state->pos-cylinderSt->pos;
		if (segment.dot(branch)>(segment.dot(segment)/*+interactionDetectionFactor*cylinder->radius*/)) {//position _after_ end of cylinder
			//FIXME : scm->penetrationDepth=-1 is defined to workaround interactions never being erased when scm->isDuplicate=2 on the true interaction.
			if (isNew) return false; else if (scm->isDuplicate) {scm->isDuplicate=2; scm->penetrationDepth=-1; return true;}}
		length = segment.norm();
		direction = segment/length;
		dist = direction.dot(branch);
		if (dist<-interactionDetectionFactor*cylinder->radius &&
			branch.squaredNorm() > pow(interactionDetectionFactor*(sphere->radius+cylinder->radius),2)) {
				if (isNew) return false; else if (scm->isDuplicate) {scm->isDuplicate=2; scm->penetrationDepth=-1; return true;}}
	} else {//handle the last node with length=0
		segment = Vector3r(0,0,0);
		length = 0;
		direction = Vector3r(0,1,0);
		dist = 0;
		if (branch.squaredNorm() > interactionDetectionFactor*(sphere->radius+cylinder->radius)) {
			if (isNew) return false; else if (scm->isDuplicate) {scm->isDuplicate=2; scm->penetrationDepth=-1; return true;}}
	}

	//Check sphere-cylinder distance
	Vector3r projectedP = cylinderSt->pos+shift2 + direction*dist;
	branch = projectedP-sphereSt->pos;
	if (branch.squaredNorm()>(pow(sphere->radius+cylinder->radius,2))) {
		if (isNew) return false; else if (scm->isDuplicate) {scm->isDuplicate=2; scm->penetrationDepth=-1; return true;}}

	if (!isNew) scm->isDuplicate = false;//reset here at each step, and recompute below

	//make sure there is no contact with the previous element in the chain, else consider this one a duplicate and get data from the other contact. two interactions will share the same geometry and physics.
	if (cylinderSt->rank>0 && dist<=0) {
		Vector3r branchP = sphereSt->pos - statePrev->pos;
		Vector3r segmentP = cylinderSt->pos - statePrev->pos;
		if (segmentP.dot(branchP)<segmentP.dot(segmentP)) {
			if (isNew) {
				const shared_ptr<Interaction> intr = scene->interactions->find(c->id1,cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank-1]);
				assert(intr);//we know there is a contact, so there should be at least a virtual interaction created by collider
				if (!intr->geom || !intr->phys) return false;
				else {
					c->geom = intr->geom;
					c->phys = intr->phys;
					scm=YADE_PTR_CAST<CylScGeom>(c->geom);
// 					scm->duplicate = intr;
					scm->isDuplicate = true;
					scm->trueInt = cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank-1];
					isNew = false;
					return true;}
			} else scm->isDuplicate=true;
			scm->trueInt = cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank-1];
		}
	}
	//similarly, make sure there is no contact with the next element in the chain
	else if (!isLast && dist>length) {
		if ( (cylinderNext->state->pos-sphereSt->pos).squaredNorm() <  pow(sphere->radius+cylinder->radius,2)) {
			if (isNew) {
				const shared_ptr<Interaction> intr = scene->interactions->find(c->id1,cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank+1]);
				assert(intr);
				if (!intr->geom || !intr->phys) return false;
				c->geom = intr->geom;
				c->phys = intr->phys;
				scm=YADE_PTR_CAST<CylScGeom>(c->geom);
// 				scm->duplicate = intr;
				scm->trueInt = cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank+1];
				scm->isDuplicate = true;
				isNew = false;
				return true;
			} else scm->isDuplicate=true;
			scm->trueInt = cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank+1];
		}
	}

	//We didn't find any special case, do normal geometry definition
	if (isNew) { scm=shared_ptr<CylScGeom>(new CylScGeom()); c->geom=scm;}

	scm->radius1=sphere->radius;
	scm->radius2=cylinder->radius;
	if (!isLast && !scm->id3) scm->id3=cylinderSt->chains[cylinderSt->chainNumber][cylinderSt->rank+1];
	scm->start=cylinderSt->pos+shift2; scm->end=scm->start+segment;

	//FIXME : there should be other checks without distanceFactor?
	if (dist<=0) {//We have sphere-node contact
		Vector3r normal=(cylinderSt->pos+shift2)-sphereSt->pos;
		Real norm=normal.norm();
		normal /=norm;
		scm->relPos=0;
		scm->onNode=true; scm->relPos=0;
		scm->penetrationDepth=sphere->radius+cylinder->radius-norm;
		scm->contactPoint=sphereSt->pos+(sphere->radius-0.5*scm->penetrationDepth)*normal;
		scm->precompute(state1,state2,scene,c,normal,isNew,shift2,true);//use sphere-sphere precompute (a node is a sphere)
	} else {//we have sphere-cylinder contact
		scm->onNode=false;
		scm->relPos=dist/length;
		Real norm=branch.norm();
		Vector3r normal=branch/norm;
		scm->penetrationDepth= sphere->radius+cylinder->radius-norm;

		// define a virtual sphere at the projected center
		scm->fictiousState.pos = projectedP;
		scm->fictiousState.vel = (1-scm->relPos)*cylinderSt->vel + scm->relPos*cylinderNext->state->vel;
		scm->fictiousState.angVel =
			((1-scm->relPos)*cylinderSt->angVel + scm->relPos*cylinderNext->state->angVel).dot(direction)*direction //twist part : interpolated
			+ segment.cross(cylinderNext->state->vel - cylinderSt->vel);// non-twist part : defined from nodes velocities

		if (dist>length) {
			scm->penetrationDepth=sphere->radius+cylinder->radius-(cylinderSt->pos+segment-sphereSt->pos).norm();
			//FIXME : handle contact jump on next element
		}
		scm->contactPoint = sphereSt->pos+normal*(sphere->radius-0.5*scm->penetrationDepth);
		scm->precompute(state1,scm->fictiousState,scene,c,branch/norm,isNew,shift2,true);//use sphere-sphere precompute (with a virtual sphere)
	}
	return true;
}


bool Ig2_Sphere_ChainedCylinder_CylScGeom::goReverse(	const shared_ptr<Shape>& cm1,
								const shared_ptr<Shape>& cm2,
								const State& state1,
								const State& state2,
								const Vector3r& shift2,
								const bool& force,
								const shared_ptr<Interaction>& c)
{
 	cerr<<"Ig2_Sphere_ChainedCylinder_CylScGeom::goReverse"<<endl;
	c->swapOrder();
	return go(cm2,cm1,state2,state1,-shift2,force,c);
}


bool Ig2_ChainedCylinder_ChainedCylinder_ScGeom6D::go(	const shared_ptr<Shape>& cm1,
							const shared_ptr<Shape>& cm2,
							const State& state1, const State& state2, const Vector3r& shift2, const bool& force,
							const shared_ptr<Interaction>& c)
{
	const ChainedState *pChain1, *pChain2;
	pChain1=YADE_CAST<const ChainedState*>(&state1);
	pChain2=YADE_CAST<const ChainedState*>(&state2);
	if (!pChain1 || !pChain2) {
		cerr <<"cast failed8567"<<endl;
	}
	const bool revert = ((int) pChain2->rank- (int) pChain1->rank == -1);
	const ChainedState& bchain1 = revert? *pChain2 : *YADE_CAST<const ChainedState*>(&state1);
	const ChainedState& bchain2 = revert? *pChain1 : *pChain2;
	if (bchain2.rank-bchain1.rank != 1) {/*cerr<<"Mutual contacts in same chain between not adjacent elements, not handled*/ return false;}
	if (pChain2->chainNumber!=pChain1->chainNumber) {cerr<<"PROBLEM0124"<<endl; return false;}

	ChainedCylinder *bs1=static_cast<ChainedCylinder*>(revert? cm2.get():cm1.get());

	shared_ptr<ScGeom6D> scm;
	bool isNew = !c->geom;
	if(!isNew) scm=YADE_PTR_CAST<ScGeom6D>(c->geom);
	else { scm=shared_ptr<ScGeom6D>(new ScGeom6D()); c->geom=scm; }
	Real length=(bchain2.pos-bchain1.pos).norm();
	Vector3r segt =pChain2->pos-pChain1->pos;
	if(isNew) {/*scm->normal=scm->prevNormal=segt/length;*/bs1->initLength=length;}
	scm->radius1=revert ? 0:bs1->initLength;
	scm->radius2=revert ? bs1->initLength:0;
	scm->penetrationDepth=bs1->initLength-length;
	scm->contactPoint=bchain2.pos;
	//bs1->segment used for fast BBs and projections + display
	bs1->segment= bchain2.pos-bchain1.pos;
#ifdef YADE_OPENGL
	bs1->length=length;
	bs1->chainedOrientation.setFromTwoVectors(Vector3r::UnitZ(),bchain1.ori.conjugate()*segt);
#endif
	scm->precompute(state1,state2,scene,c,segt/length,isNew,shift2,true);
	scm->precomputeRotations(state1,state2,isNew,false);
	//Set values that will be considered in Ip2 functor, geometry (precomputed) is really defined with values above
	scm->radius1 = scm->radius2 = bs1->initLength*0.5;
	return true;
}

bool Ig2_ChainedCylinder_ChainedCylinder_ScGeom6D::goReverse(	const shared_ptr<Shape>& cm1,
								const shared_ptr<Shape>& cm2,
								const State& state1,
								const State& state2,
								const Vector3r& shift2,
								const bool& force,
								const shared_ptr<Interaction>& c)
{
	return go(cm2,cm1,state2,state1,-shift2,force,c);
}

#ifdef YADE_OPENGL
//!##################	RENDERING   #####################

bool Gl1_Cylinder::wire;
bool Gl1_Cylinder::glutNormalize;
int  Gl1_Cylinder::glutSlices;
int  Gl1_Cylinder::glutStacks;
int Gl1_Cylinder::glCylinderList=-1;

void Gl1_Cylinder::out( Quaternionr q )
{
	AngleAxisr aa(q);
	std::cout << " axis: " <<  aa.axis()[0] << " " << aa.axis()[1] << " " << aa.axis()[2] << ", angle: " << aa.angle() << " | ";
}

void Gl1_Cylinder::go(const shared_ptr<Shape>& cm, const shared_ptr<State>& ,bool wire2, const GLViewInfo&)
{
	Real r=(static_cast<Cylinder*>(cm.get()))->radius;
	Real length=(static_cast<Cylinder*>(cm.get()))->length;
	//glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Vector3f(cm->color[0],cm->color[1],cm->color[2]));
	glColor3v(cm->color);
	if(glutNormalize) glPushAttrib(GL_NORMALIZE);
// 	glPushMatrix();
	Quaternionr shift = (static_cast<ChainedCylinder*>(cm.get()))->chainedOrientation;
	if (wire || wire2) drawCylinder(true, r,length,shift);
	else drawCylinder(false, r,length,shift);
	if(glutNormalize) glPopAttrib();
// 	glPopMatrix();
	return;
}

void Gl1_ChainedCylinder::go(const shared_ptr<Shape>& cm, const shared_ptr<State>& state,bool wire2, const GLViewInfo&)
{
	Real r=(static_cast<ChainedCylinder*>(cm.get()))->radius;
	Real length=(static_cast<ChainedCylinder*>(cm.get()))->length;
	Quaternionr shift;// = (static_cast<ChainedCylinder*>(cm.get()))->chainedOrientation;
	shift.setFromTwoVectors(Vector3r::UnitZ(),state->ori.conjugate()*(static_cast<ChainedCylinder*>(cm.get()))->segment);
	glColor3v(cm->color);
	if(glutNormalize) glPushAttrib(GL_NORMALIZE);
	if (wire || wire2) drawCylinder(true, r,length,shift);
	else drawCylinder(false, r,length,shift);
	if(glutNormalize) glPopAttrib();
	return;
}

void Gl1_Cylinder::drawCylinder(bool wire, Real radius, Real length, const Quaternionr& shift) const
{
   glPushMatrix();
   GLUquadricObj *quadObj = gluNewQuadric();
   gluQuadricDrawStyle(quadObj, (GLenum) (wire ? GLU_SILHOUETTE : GLU_FILL));
   gluQuadricNormals(quadObj, (GLenum) GLU_SMOOTH);
   gluQuadricOrientation(quadObj, (GLenum) GLU_OUTSIDE);
   AngleAxisr aa(shift);
   glRotatef(aa.angle()*180.0/Mathr::PI,aa.axis()[0],aa.axis()[1],aa.axis()[2]);
   gluCylinder(quadObj, radius, radius, length, glutSlices,glutStacks);
   gluQuadricOrientation(quadObj, (GLenum) GLU_INSIDE);
   glutSolidSphere(radius,glutSlices,glutStacks);
   glTranslatef(0.0,0.0,length);

   glutSolidSphere(radius,glutSlices,glutStacks);
//    gluDisk(quadObj,0.0,radius,glutSlices,_loops);
   gluDeleteQuadric(quadObj);
   glPopMatrix();
}


//!##################	BOUNDS FUNCTOR   #####################

#endif

void Bo1_Cylinder_Aabb::go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r& se3, const Body* b){
	Cylinder* cylinder = static_cast<Cylinder*>(cm.get());
	if(!bv){ bv=shared_ptr<Bound>(new Aabb); }
	Aabb* aabb=static_cast<Aabb*>(bv.get());
	if(!scene->isPeriodic){
		const Vector3r& O = se3.position;
		Vector3r O2 = se3.position+se3.orientation*cylinder->segment;
		aabb->min=aabb->max=O;
		for (int k=0;k<3;k++){
			aabb->min[k]=min(aabb->min[k],min(O[k],O2[k])-cylinder->radius);
			aabb->max[k]=max(aabb->max[k],max(O[k],O2[k])+cylinder->radius);
		}
		return;
	}
}

void Bo1_ChainedCylinder_Aabb::go(const shared_ptr<Shape>& cm, shared_ptr<Bound>& bv, const Se3r& se3, const Body* b){
	Cylinder* cylinder = static_cast<Cylinder*>(cm.get());
	if(!bv){ bv=shared_ptr<Bound>(new Aabb); }
	Aabb* aabb=static_cast<Aabb*>(bv.get());
	if(!scene->isPeriodic){
		const Vector3r& O = se3.position;
		Vector3r O2 = se3.position+cylinder->segment;
		aabb->min=aabb->max=O;
		for (int k=0;k<3;k++){
			aabb->min[k]=min(aabb->min[k],min(O[k],O2[k])-cylinder->radius);
			aabb->max[k]=max(aabb->max[k],max(O[k],O2[k])+cylinder->radius);
		}
		return;
	}
}

void Law2_CylScGeom_FrictPhys_CundallStrack::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* contact){
	int id1 = contact->getId1(), id2 = contact->getId2();

	CylScGeom* geom= static_cast<CylScGeom*>(ig.get());
	FrictPhys* phys = static_cast<FrictPhys*>(ip.get());
	if(geom->penetrationDepth <0){
		if (neverErase) {
			phys->shearForce = Vector3r::Zero();
			phys->normalForce = Vector3r::Zero();}
		else 	scene->interactions->requestErase(contact);
		return;}
	if (geom->isDuplicate) {
		if (id2!=geom->trueInt) {
// 			cerr<<"skip duplicate "<<id1<<" "<<id2<<endl;
			if (geom->isDuplicate==2) scene->interactions->requestErase(contact);
// 			cerr<<"erase duplicate "<<id1<<" "<<id2<<endl;
			return;}
	}
	Real& un=geom->penetrationDepth;
	phys->normalForce=phys->kn*std::max(un,(Real) 0)*geom->normal;

	Vector3r& shearForce = geom->rotate(phys->shearForce);
	const Vector3r& shearDisp = geom->shearIncrement();
	shearForce -= phys->ks*shearDisp;
	Real maxFs = phys->normalForce.squaredNorm()*std::pow(phys->tangensOfFrictionAngle,2);

	if (likely(!scene->trackEnergy)){//Update force but don't compute energy terms (see below))
		// PFC3d SlipModel, is using friction angle. CoulombCriterion
		if( shearForce.squaredNorm() > maxFs ){
			Real ratio = sqrt(maxFs) / shearForce.norm();
			shearForce *= ratio;}
	} else {
		//almost the same with additional Vector3r instanciated for energy tracing, duplicated block to make sure there is no cost for the instanciation of the vector when traceEnergy==false
		if(shearForce.squaredNorm() > maxFs){
			Real ratio = sqrt(maxFs) / shearForce.norm();
			Vector3r trialForce=shearForce;//store prev force for definition of plastic slip
			//define the plastic work input and increment the total plastic energy dissipated
			shearForce *= ratio;
			Real dissip=((1/phys->ks)*(trialForce-shearForce))/*plastic disp*/ .dot(shearForce)/*active force*/;
			if(dissip>0) scene->energy->add(dissip,"plastDissip",plastDissipIx,/*reset*/false);
		}
		// compute elastic energy as well
		scene->energy->add(0.5*(phys->normalForce.squaredNorm()/phys->kn+phys->shearForce.squaredNorm()/phys->ks),"elastPotential",elastPotentialIx,/*reset at every timestep*/true);
	}
	if (!scene->isPeriodic) {
		Vector3r force = -phys->normalForce-shearForce;
		scene->forces.addForce(id1,force);
		scene->forces.addTorque(id1,(geom->radius1-0.5*geom->penetrationDepth)* geom->normal.cross(force));
		//FIXME : include moment due to axis-contact distance in forces on node
		Vector3r twist = (geom->radius2-0.5*geom->penetrationDepth)* geom->normal.cross(force);
		scene->forces.addForce(id2,(geom->relPos-1)*force);
		scene->forces.addTorque(id2,(1-geom->relPos)*twist);
		if (geom->relPos) { //else we are on node (or on last node - and id3 is junk)
			scene->forces.addForce(geom->id3,(-geom->relPos)*force);
			scene->forces.addTorque(geom->id3,geom->relPos*twist);}
	}
// 		applyForceAtContactPoint(-phys->normalForce-shearForce, geom->contactPoint, id1, de1->se3.position, id2, de2->se3.position);
	else {//FIXME : periodicity not implemented here :
		Vector3r force = -phys->normalForce-shearForce;
		scene->forces.addForce(id1,force);
		scene->forces.addForce(id2,-force);
		scene->forces.addTorque(id1,(geom->radius1-0.5*geom->penetrationDepth)* geom->normal.cross(force));
		scene->forces.addTorque(id2,(geom->radius2-0.5*geom->penetrationDepth)* geom->normal.cross(force));
	}
}
