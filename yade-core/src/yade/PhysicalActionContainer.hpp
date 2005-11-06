/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __ACTIONCONTAINER_HPP__
#define __ACTIONCONTAINER_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-lib-serialization/Serializable.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhysicalActionContainerIteratorPointer.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class PhysicalAction;

// this container is different: it has ALWAYS data inside, not empty pointers at all. Every field has
// inside the right pointer type so it can be safely (and quickly) static_casted, so that the user himself
// is doing addition, substraction, and whatever he wants.
//
// you should never have to create new PhysicalAction ! (that takes too much time), except for calling prepare, which is done only once
 
class PhysicalActionContainer : public Serializable
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor/Destructor									///
///////////////////////////////////////////////////////////////////////////////////////////////////

	public : PhysicalActionContainer();
	public : virtual ~PhysicalActionContainer();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Methods											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	public    : virtual void clear() 							{throw;};
	// doesn't not delete all, just resets data
	public    : virtual void reset() 							{throw;};
	public    : virtual unsigned int size() 						{throw;};
	// fills container with resetted fields. argument here, should be all PhysicalAction types that are planned to use
	public    : virtual void prepare(std::vector<shared_ptr<PhysicalAction> >& )			{throw;};
	
	// finds and returns action of given polymorphic type, for body of given Id,
	// should be always succesfull. if it is not - you forgot to call prepare()
	public    : virtual shared_ptr<PhysicalAction>& find(
					  unsigned int /*Body->getId() */
					, int /*Force::getClassIndexStatic()*/)		{throw;};

	// looping over Bodies, and their Actions
//	public    : virtual void gotoFirst() 							{throw;};
//	public    : virtual bool notAtEnd() 							{throw;};
//	public    : virtual void gotoNext() 							{throw;};
//	public    : virtual shared_ptr<PhysicalAction>& getCurrent(int& )				{throw;};

	public    : typedef PhysicalActionContainerIteratorPointer iterator;
        public    : virtual PhysicalActionContainer::iterator begin()			{throw;};
        public    : virtual PhysicalActionContainer::iterator end()			{throw;};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization										///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_NAME(PhysicalActionContainer);
	REGISTER_BASE_CLASS_NAME(Serializable);

	// local storage for uniform serialization of all possible container concrete implementations.
	private   : vector<shared_ptr<PhysicalAction> > action;
	public    : virtual void registerAttributes();
	//protected : virtual void preProcessAttributes(bool deserializing);
	//protected : virtual void postProcessAttributes(bool deserializing);

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(PhysicalActionContainer,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __ACTIONCONTAINER_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
