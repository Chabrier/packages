/*
 * @file vle/extension/difference-equation/Base.cpp
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


#include <vle/extension/difference-equation/Base.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/Set.hpp>
#include <cmath>

namespace vle { namespace extension { namespace DifferenceEquation {

using namespace devs;
using namespace value;

const unsigned int Base::DEFAULT_SIZE = 2;

Base::Base(const DynamicsInit& model,
	   const InitEventList& events,
	   bool control) :
    Dynamics(model, events),
    mTimeStep(0),
    mTimeStepUnit(vle::utils::DATE_TIME_UNIT_NONE),
    mMode(NAME),
    mNosyncDependance(false),
    mControl(control),
    mSynchro(false),
    mAllSynchro(false),
    mNosyncReceiveTime(vle::devs::infinity),
    mLastComputeTime(vle::devs::infinity),
    mLastClearTime(vle::devs::infinity),
    mWaiting(0)
    {
        if (events.exist("time-step")) {
            if (events.get("time-step")->isDouble()) {
                mTimeStep = toDouble(events.get("time-step"));
            } else if (events.get("time-step")->isMap()) {
                const value::Map& timeStep = events.getMap("time-step");

                mTimeStep = toDouble(timeStep.get("value"));
                mTimeStepUnit = vle::utils::DateTime::convertUnit(
                    toString(timeStep.get("unit")));
            } else {
                throw utils::InternalError((boost::format(
                            "[%1%] DifferenceEquation - bad type "\
                            "for value of time-step port")
                    % getModelName()).str());
            }

        } else {
            if (not events.exist("time-step")) {
                throw utils::InternalError((boost::format(
                            "[%1%] DifferenceEquation - "\
                            "time-step port not exists")
                    % getModelName()).str());
            }
        }

        if (events.exist("mode")) {
            std::string str = toString(events.get("mode"));

            if (str == "name" ) mMode = NAME;
            else if (str == "port" ) mMode = PORT;
            else if (str == "mapping" ) {
                mMode = MAPPING;

                if (not events.exist("mapping")) {
                    throw utils::InternalError((boost::format(
                                "[%1%] DifferenceEquation - "\
                                "mapping port not exists")
                        % getModelName()).str());
                }

                const value::Map& mapping = events.getMap("mapping");
                const value::MapValue& lst = mapping.value();
                for (value::MapValue::const_iterator it = lst.begin();
                     it != lst.end(); ++it) {
                    std::string port = it->first;
                    std::string name = toString(it->second);

                    mMapping[port] = name;
                }

            }
        }
    }

void Base::addExternalValue(double value,
                            const std::string& name,
                            bool init)
{
    if (mExternalValues.find(name) == mExternalValues.end()) {
        throw utils::ModellingError(
            (boost::format("[%1%] DifferenceEquation::add - "\
                  "invalid variable name: %2%") % getModelName() % name).str());
    }

    mExternalValues[name].push_front(value);
    if (mSize[name] != -1 and
        (int)mExternalValues[name].size() > mSize[name]) {
        mExternalValues[name].pop_back();
    }
    if (not init) {
        mReceivedValues[name] = true;
    }
}

void Base::applyPerturbations(const vle::devs::Time& time, bool update)
{
    Perturbations::const_iterator it = mPerturbations.begin();

    while (it != mPerturbations.end()) {
        double value = it->value;

        if (mLastComputeTime == time) {
            if (it->type == ADD) {
                value += getValue(it->name);
            } else if (it->type == SCALE) {
                value *= getValue(it->name);
            }
            removeValue(it->name);
        } else {
            if (it->type == ADD or it->type == SCALE) {
                updateValues(time);
                if (it->type == ADD) {
                    value += getValue(it->name);
                } else if (it->type == SCALE) {
                    value *= getValue(it->name);
                }
            }
        }
        addValue(value, it->name);
        ++it;
    }

    LockedVariables::const_iterator it2 = mLockedVariables.begin();

    while (it2 != mLockedVariables.end()) {
        lockValue(*it2);
        ++it2;
    }

    if (update) {
        if (mPerturbations.size() < getVariableNumber()) {
            if (mLastComputeTime == time) {
                removeValues();
            }
            updateValues(time);
            mLastComputeTime = time;
        }
    }
    mPerturbations.clear();
}

void Base::clearReceivedValues()
{
    ReceivedMap::iterator it = mReceivedValues.begin();

    while (it != mReceivedValues.end()) {
        it->second = false;
        ++it;
    }
}

void Base::createExternalVariable(const std::string& name,
                                  VariableIterators& iterators)
{
    mExternalValues.insert(std::make_pair(name, Values()));
    iterators.mValues = &mExternalValues.find(name)->second;
    mInitExternalValues.insert(std::make_pair(name, Values()));
    iterators.mInitValues = &mInitExternalValues.find(name)->second;
    mReceivedValues.insert(std::make_pair(name, false));
    iterators.mReceivedValues = &mReceivedValues.find(name)->second;
    mNosyncReceivedValues.insert(std::make_pair(name, devs::Time(0)));
    if (mSize.find(name) == mSize.end()) {
        mSize[name] = DEFAULT_SIZE;
    }
}

void Base::depends(const std::string& name,
                   bool synchronized)
{
    if (synchronized) {
        mSynchro = true;
        mSynchros.insert(name);
        ++mWaiting;
    }
    mDepends.insert(name);
}

void Base::initExternalVariable(const std::string& name)
{
    if (not ((mControl and mDepends.find(name) != mDepends.end()) or
             not mControl)) {
        throw utils::ModellingError((boost::format(
                    "[%1%] DifferenceEquation::init - invalid variable name: "\
                    "%2%") % getModelName() % name).str());
    }

    if (mSynchros.find(name) == mSynchros.end()) {
        if (mAllSynchro) {
            mSynchros.insert(name);
            ++mSyncs;
            --mWaiting;
        }
    } else {
        ++mSyncs;
        --mWaiting;
    }

    if (mExternalValues.find(name) == mExternalValues.end()) {
        mReceivedValues[name] = false;
        mExternalValues[name] = Values();
        mSize[name] = DEFAULT_SIZE;
    }

    if (mInitExternalValues.find(name) != mInitExternalValues.end()) {
        ValuesIterator it = mInitExternalValues[name].begin();

        while (it != mInitExternalValues[name].end()) {
            double value = *it;

            addExternalValue(value, name, true);
            ++it;
        }
        mInitExternalValues[name].clear();
    }
}

double Base::val(const std::string& name,
                 const Values* it,
                 const VariableIterators& iterators,
                 int shift) const
{
    if (shift > 0) {
        throw utils::ModellingError(
            (boost::format("[%1%] DifferenceEquation::getValue - " \
                  "positive shift on %2%") % getModelName() % name).str());
    }

    if (mState == INIT) {
        if (not iterators.mSynchro and
            not *iterators.mReceivedValues) {
            ++shift;
        }

        if (shift == 0) {
            if (it->empty()) {
                throw utils::ModellingError((boost::format(
                            "[%1%] - %2%[0] - shift too large") %
                    getModelName() % name).str());
            }

            return it->front();
        } else {
            if ((int)(it->size() - 1) < -shift) {
                throw utils::ModellingError((boost::format(
                            "[%1%] - %2%[%3%] - shift too large") %
                    getModelName() % name % shift).str());
            }

            return (*it)[-shift];
        }
    } else {
        if (not iterators.mSynchro and
            not *iterators.mReceivedValues) {
            ++shift;
        }

        if (shift > 0) {
            throw utils::InternalError(
                (boost::format("[%1%] DifferenceEquation::getValue - " \
                      "wrong shift on %2%") %
                getModelName() % name).str());
        }

        if (shift == 0) {
            if (it->empty()) {
                throw utils::ModellingError((boost::format(
                            "[%1%] - %2%[0] - shift too large") %
                    getModelName() % name).str());
            }

            return it->front();
        } else {
            if ((int)(it->size() - 1) < -shift) {
                throw utils::InternalError((boost::format(
                            "[%1%] - %2%[%3%] - shift too large") %
                    getModelName() % name % shift).str());
            }
            return (*it)[-shift];
        }
    }
}

void Base::processUpdate(const std::string& name,
                         const devs::ExternalEvent& event,
                         bool begin, bool end,
                         const vle::devs::Time& time)
{
    double value = 0.0;
    bool ok = true;
    bool sync = mSynchros.find(name) != mSynchros.end();

    if (mState == SEND_INIT or mState == POST_SEND_INIT) {

        if (mControl and not (mControl and
                              mDepends.find(name) != mDepends.end())) {
            throw utils::ModellingError(
                (boost::format("[%1%] DifferenceEquation::init "                 \
                      "- invalid variable name: %2%") % getModelName() %
                name).str());
        }

        if (event.attributes()->toMap().exist("init")) {
            const value::Set& init =
                event.attributes()->toMap().getSet("init");
            unsigned int i;

            for (i = 0; i < init.size(); ++i) {
                mInitExternalValues[name].push_front(
                    toDouble(init.get(i)));
            }
        } else if (event.attributes()->toMap().exist("value")) {
            value = event.attributes()->toMap().getDouble("value");
            mNoEDValues[name].push_front(value);
        }
        ok = false;
    } else {
        value = event.attributes()->toMap().getDouble("value");
    }

    if (mState == INIT or
        (mState == PRE and
         mInitExternalValues.find(name) != mInitExternalValues.end() and
         not mInitExternalValues[name].empty())) {
        initExternalVariable(name);
        sync = mSynchros.find(name) != mSynchros.end();
    }

    if (ok) {
        if ((not begin) and (not end) and sync) {
            mExternalValues[name].pop_front();
            addExternalValue(value, name);
            if (mSigma == 0) {
                mState = RUN;
                mSigma = mTimeStep;
            }
        } else {
            if (mLastComputeTime == time) {
                if (sync) {
                    mExternalValues[name].pop_front();
                    mState = RUN;
                    mSigma = 0;
                } else {
                    if (mNosyncReceivedValues[name] == time) {
                        mExternalValues[name].pop_front();
                    }
                }
            } else {
                if (mReceivedValues[name]) {
                    mExternalValues[name].pop_front();
                    mReceivedValues[name] = false;
                    if (mState == INIT or (mState == PRE and end)) {
                        if (sync) {
                            --mReceive;
                        }
                    }
                }
            }
            addExternalValue(value, name);
            if (not sync) {
                mNosyncReceivedValues[name] = time;
                mNosyncReceiveTime = time;
                ++mNosyncReceive;
            } else {
                if (mState == INIT or (mState == PRE and end)) {
                    ++mReceive;
                }
            }
        }
    }
}

void Base::pushNoEDValues()
{
    if (not mNoEDValues.empty()) {
        ValuesMapIterator itl = mNoEDValues.begin();

        while (itl != mNoEDValues.end()) {
            std::deque < double >::const_iterator itv = itl->second.begin();

            initExternalVariable(itl->first);
            if (mSynchros.find(itl->first) != mSynchros.end()) {
                ++mReceive;
            }
            while (itv != itl->second.end()) {
                addExternalValue(*itv, itl->first);
                ++itv;
            }
            ++itl;
        }
        mNoEDValues.clear();
    }
}

void Base::size(const std::string& name,
                int s)
{
    if (mSize.find(name) != mSize.end()) {
        throw utils::InternalError((boost::format(
                    "[%1%] DifferenceEquation::size - %2% already exists") %
            getModelName() % name).str());
    }

    mSize[name] = s;
}

Time Base::init(devs::Time time)
{
    if (mMode == NAME) {
        mDependance = getModel().existInputPort("update");
    } else {
        mDependance = (getModel().getInputPortNumber() -
                       (getModel().existInputPort("phase")?1:0) -
                       (getModel().existInputPort("perturb")?1:0) -
                       (getModel().existInputPort("request")?1:0) -
                       (getModel().existInputPort("add")?1:0) -
                       (getModel().existInputPort("remove")?1:0)) > 0;
    }

    mActive = getModel().existOutputPort("update");
    mSyncs = 0;
    if (mControl) {
        unsigned int n;

        if (mMode == NAME) {
            if (getModel().existInputPort("update")) {
                n = std::distance(getModel().getInPort("update").begin(),
                                  getModel().getInPort("update").end());
            } else {
                n = 0;
            }
        } else {
            n = getModel().getInputPortNumber() -
                (getModel().existInputPort("phase")?1:0) -
                (getModel().existInputPort("perturb")?1:0) -
                (getModel().existInputPort("request")?1:0) -
                (getModel().existInputPort("add")?1:0) -
                (getModel().existInputPort("remove")?1:0);
        }

        if (mDepends.size() != n) {
            throw utils::InternalError((boost::format(
                        "[%1%] DifferenceEquation::size - " \
                        "%2% connection(s) on update port missing") %
                getModelName() % (mDepends.size() - n)).str());
        }

    } else {
        if (mMode == NAME) {
            if (getModel().existInputPort("update")) {
                mWaiting = std::distance(getModel().getInPort("update").begin(),
                                         getModel().getInPort("update").end());
            }
        } else {
            mWaiting = getModel().getInputPortNumber() -
                (getModel().existInputPort("phase")?1:0) -
                (getModel().existInputPort("perturb")?1:0) -
                (getModel().existInputPort("request")?1:0) -
                (getModel().existInputPort("add")?1:0) -
                (getModel().existInputPort("remove")?1:0);
        }
    }

    mReceive = 0;
    mNosyncReceive = 0;
    mLastTime = time;
    mState = SEND_INIT;
    return 0.0;
}

Time Base::timeAdvance() const
{
    return mSigma;
}

void Base::confluentTransitions(
    Time time,
    const ExternalEventList& extEventlist)
{
    externalTransition(extEventlist, time);
    internalTransition(time);
}

void Base::internalTransition(Time time)
{
    switch (mState) {
    case SEND_INIT:
        mSigma = 0.0;
        mState = POST_SEND_INIT;
        break;
    case POST_SEND_INIT:
        pushNoEDValues();

        if (mControl) {
            if (check()) {
                initValues(time);
            } else {
                if (!mDependance or !mSynchro or mWaiting <= 0) {
                    mState = INIT;
                    initValues(time);
                }
            }

            if (mReceive == mSyncs and mWaiting <= 0) {
                mReceive = 0;
                mNosyncReceive = 0;
            }

            if (mActive and check()) {
                mState = PRE_INIT;
                mSigma = 0;
            } else {
                if (mDependance and mWaiting > 0) {
                    mState = INIT;
                    mSigma = devs::infinity;
                } else {
                    clearReceivedValues();
                    mState = RUN;
                    mSigma = timeStep(time);
                }
            }
        } else {
            if (mWaiting <= 0) {
                initValues(time);
                if (mActive) {
                    mState = POST;
                    mSigma = 0;
                } else {
                    mState = RUN;
                    timeStep(time);
                }
            } else {
                mState = INIT;
                mSigma = devs::infinity;
            }
        }
        break;
    case PRE_INIT:
        if (mDependance and mWaiting > 0) {
            mState = INIT;
            mSigma = devs::infinity;
        } else {
            clearReceivedValues();
            if (mSyncs != 0 and (mSynchro or mAllSynchro)) {
                mState = PRE;
            } else {
                mState = RUN;
            }
            mSigma = timeStep(time);
        }
        break;
    case PRE_INIT2:
        if (mSyncs != 0 and (mSynchro or mAllSynchro)) {
            mState = PRE;
        } else {
            mState = RUN;
        }
        mSigma = timeStep(time);
        break;
    case INIT:
        break;
    case PRE:
        if (mSyncs != 0 and (mSynchro or mAllSynchro)) break;
    case RUN:
        if (mSyncs == 0) {
            clearReceivedValues();
        }
        mLastTime = time;

        if (devs::isInfinity(mLastClearTime) or mLastClearTime < time) {
            mLockedVariables.clear();
            invalidValues();
            mLastClearTime = time;
        }

        if (mPerturbations.empty()) {
            if (mLastComputeTime == time) {
                removeValues();
            }
            updateValues(time);
            mLastComputeTime = time;
        } else {
            applyPerturbations(time, true);
        }
        if (mActive) {
            mState = POST;
            mSigma = 0;
        } else {
            if (mSyncs != 0 and (mSynchro or mAllSynchro)) {
                mState = PRE;
            } else {
                mState = RUN;
            }
            mSigma = timeStep(time);
        }
        break;
    case POST:
        mReceive = 0;
        mNosyncReceive = 0;
        if (mSynchro or mAllSynchro) {
            mState = PRE;
        } else {
            mState = RUN;
        }
        mSigma = timeStep(time);
        break;
    case POST2:
        mState = RUN;
        mSigma = mSigma2;
        break;
    case POST3:
        mState = PRE;
        mSigma = timeStep(time);
        break;
    }
}

void Base::externalTransition(const ExternalEventList& event,
                              Time time)
{
    Time e = time - mLastTime;
    // FIXME: bool end = (e == mSigma);
    bool end = std::abs(e - mSigma) < 1e-10;
    bool begin = (e == 0);
    ExternalEventList::const_iterator it = event.begin();
    bool reset = false;
    bool ignore = true;

    while (it != event.end()) {
        const vle::value::Map& attrs = it->attributes()->toMap();
        if (it->onPort("phase")) {
            mPhase = attrs.getInt("phase");
        } else if (mMode == NAME and it->onPort("update")) {
            if (attrs.exist("value") or
                (mState == SEND_INIT or mState == POST_SEND_INIT)) {
                std::string name = attrs.getString("name");

                if ((mNosyncReceiveTime < time or
                     isInfinity(mNosyncReceiveTime)) and
                    (mLastComputeTime < time or
                     isInfinity(mLastComputeTime))) {
                    mNosyncReceive = 0;
                }

                if (mNosyncReceive == 0 and mReceive == 0 and mSyncs > 0 and
                    (mLastComputeTime < time or
                     devs::isInfinity(mLastComputeTime))) {
                    clearReceivedValues();
                }
                processUpdate(name, *it, begin, end, time);
                ignore = false;
            }
        } else if (it->onPort("perturb")) {
            std::string name = attrs.getString("name");
            double value = attrs.getDouble("value");
            Perturbation_t type = SET;

            if (attrs.exist("type")) {
                type = (Perturbation_t)(
                    attrs.getInt("type"));
            }

            if (devs::isInfinity(mLastClearTime) or mLastClearTime < time) {
                mLockedVariables.clear();
                invalidValues();
                mLastClearTime = time;
            }
            mLockedVariables.push_back(name);

            mPerturbations.push_back(Perturbation(name, value, type));

            ignore = false;

        } else if (not it->onPort("add") and
                   not it->onPort("remove")) {

            if (mMode != PORT and mMode != MAPPING) {
                throw utils::InternalError((boost::format(
                            "[%1%] - DifferenceEquation: invalid mode") %
                    getModelName()).str());
            }

            std::string portName = it->getPortName();
            std::string name = (mMode == PORT)?portName:mMapping[portName];

            if (attrs.exist("value") or
                (mState == SEND_INIT or mState == POST_SEND_INIT)) {

                if ((mNosyncReceiveTime < time or
                     devs::isInfinity(mNosyncReceiveTime)) and
                    (mLastComputeTime < time or
                     devs::isInfinity(mLastComputeTime))) {
                    mNosyncReceive = 0;
                }

                if (mNosyncReceive == 0 and mReceive == 0 and mSyncs > 0 and
                    (mLastComputeTime < time or
                     devs::isInfinity(mLastComputeTime))) {
                    clearReceivedValues();
}
                processUpdate(name, *it, begin, end, time);
                ignore = false;
            }
        }
        ++it;
    }

    if (not ignore) {
        if (not mPerturbations.empty()) {
            if (mReceive == mSyncs or mLastComputeTime == time) {
                if (not end and e > 0) {
                    applyPerturbations(time, false);
                    reset = true;
                } else {
                    mState = RUN;
                    mSigma = 0;
                }
            }
        }

        if (mState == INIT) {
            pushNoEDValues();

            bool ok = true;

            if (mNosyncDependance) {
                for (ValuesMapIterator ite = mExternalValues.begin();
                     ite != mExternalValues.end() and ok; ++ite) {
                    ok = mSynchros.find(ite->first) != mSynchros.end() or
                        not mInitExternalValues[ite->first].empty();
                }
            }

            if (mWaiting <= 0 and ok) {
                if (not check()) {
                    initValues(time);
                    mSigma = 0;
                    mState = PRE_INIT2;
                    clearReceivedValues();
                } else {
                    clearReceivedValues();
                    mSigma = timeStep(time);
                    mState = PRE;
                }
                if (mReceive == mSyncs) {
                    mReceive = 0;
                    mNosyncReceive = 0;
                }
            }
        } else {
            if (mState == PRE and end and (mReceive == mSyncs or reset)) {
                mState = RUN;
                mSigma = 0;
                if (mReceive == mSyncs and mWaiting <= 0) {
                    mReceive = 0;
                    mNosyncReceive = 0;
                }
            } else {
                if ((mState == RUN or mState == PRE)
                    and (not end) and (not begin) and reset) {
                    mLastTime = time;
                    mState = POST2;
                    mSigma2 = mSigma - e;
                    mSigma = 0;
                } else {
                    if (mState == POST) {
                        mState = POST3;
                        mSigma = 0;
                    } else {
                        mLastTime = time;
                        mSigma -= e;
                        if (mSigma < 0.0) {
                            mSigma = 0.0;
                        }
                    }
                }
            }
        }
    }
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

double operator<<(double& value, const Var& var)
{
    value = var.value;
    return value;
}

vle::devs::ExternalEventList& operator<<(vle::devs::ExternalEventList& output,
					 const Var& var)
{
    output.emplace_back(var.name);
    vle::value::Map& attrs = output.back().addMap();
    attrs.addString("name",var.name);
    attrs.addDouble("value",var.value);
    return output;
}

}}} // namespace vle extension DifferenceEquation
