/*
 * @file vle/extension/difference-equation/Generic.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/extension/difference-equation/Generic.hpp>

namespace vle { namespace extension { namespace DifferenceEquation {

using namespace devs;
using namespace value;

Generic::Generic(const DynamicsInit& model,
                 const InitEventList& events) :
    Simple(model, events, false)
{
    mMode = PORT;
}

void Generic::allSync()
{
    mAllSynchro = true;
}

void Generic::beginExt()
{
    mNamesIt = mExternalValues.begin();
}

bool Generic::endExt() const
{
    return mNamesIt == mExternalValues.end();
}

void Generic::nextExt()
{
    ++mNamesIt;
}

std::string Generic::nameExt() const
{
    return mNamesIt->first;
}

double Generic::valueExt(int shift)
{
    std::string name = mNamesIt->first;
    VariableIterators iterators;

    iterators.mValues = &mExternalValues.find(name)->second;
    iterators.mInitValues = &mInitExternalValues.find(name)->second;
    iterators.mReceivedValues = &mReceivedValues.find(name)->second;
    iterators.mSynchro = mAllSynchro;
    return Base::val(name, &mExternalValues.find(name)->second,
                     iterators, shift);
}

void Generic::externalTransition(const ExternalEventList& events,
                                 Time time)
{
    ExternalEventList::const_iterator it = events.begin();

    while (it != events.end()) {
        const vle::value::Map& attrs = it->attributes()->toMap();
        if (it->onPort("add")) {
            std::string name = attrs.getString("name");

            if (mAllSynchro) {
                mSynchros.insert(name);
                ++mWaiting;
                mState = PRE;
            }
            mDepends.insert(name);
            if (mState != INIT) {
                initExternalVariable(name);
            }

            if (mLastComputeTime == time) {
                mReceive = mReceivedValues.size() - 1;
                mLastComputeTime = vle::devs::infinity;
                mSigma = 0;
            }
       } else if (it->onPort("remove")) {
            std::string name = attrs.getString("name");

            if (mAllSynchro) {
                mSynchros.erase(name);
                --mSyncs;
            }
            mDepends.erase(name);
            mExternalValues.erase(name);
            mInitExternalValues.erase(name);
            if (mReceive > 0 and mReceivedValues.find(name)->second) {
                --mReceive;
            }
            mReceivedValues.erase(name);

            if (mLastComputeTime == time) {
                mReceive = mReceivedValues.size();
                mState = PRE;
                mLastComputeTime = vle::devs::infinity;
            }
        }
        ++it;
    }
    Simple::externalTransition(events, time);
}

}}} // namespace vle extension DifferenceEquation
