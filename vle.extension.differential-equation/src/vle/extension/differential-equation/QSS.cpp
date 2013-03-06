/*
 * @file vle/extension/differential-equation/QSS.cpp
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


#include <vle/extension/differential-equation/QSS.hpp>
#include <vle/value/Map.hpp>
#include <cmath>

namespace vle { namespace extension { namespace QSS {

using namespace devs;
using namespace value;

Simple::Simple(const DynamicsInit& model,
	       const InitEventList& events) :
    DifferentialEquation::Base(model, events)
{
    mPrecision = events.getDouble("precision");
    mEpsilon = mPrecision;

    mThreshold = events.getDouble("threshold");
}

double Simple::getEstimatedValue(double e) const
{
    return mValue + e * mGradient;
}

void Simple::updateGradient(bool external, const Time& time)
{
    mLastTime = time;
    mGradient = compute(time);
    if (external) {
	mSigma = Time(0);
	mState = POST3;
    }
    else updateSigma(time);
}

void Simple::updateSigma(const Time& /* time */)
{
    if (std::abs(mGradient) < mThreshold) {
        mSigma = devs::infinity;
    } else {
        if (mGradient > 0) {
            mSigma = Time((d(mIndex + 1)-mValue) / mGradient);
        } else {
            mSigma = Time(((d(mIndex)-mValue)-mEpsilon) / mGradient);
        }
    }
}

void Simple::updateValue(bool external, const Time& time)
{
    if (external) {
	pushValue(time, mValue + (time - mLastTime) * mGradient);
    }
    else {
	if (mGradient >= 0) ++mIndex;
	else --mIndex;
	mPreviousValue = mValue;
	pushValue(time, d(mIndex));
    }
}

void Simple::reset(const Time& time, double value)
{
    mPreviousValue = mValue;
    mValue = value;
    mIndex = (long)(floor(mValue/mPrecision));
    mLastTime = time;
    mGradient = compute(time);
    updateSigma(time);
}

// DEVS Methods
Time Simple::init(const Time& time)
{
    Time r = DifferentialEquation::Base::init(time);

    mIndex = (long)(floor(mValue/mPrecision));
    return r;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

Multiple::Multiple(const DynamicsInit& model,
		   const InitEventList& events) :
    Dynamics(model, events),
    mActive(true),
    mDependance(true),
    mValue(0),
    mExternalVariableNumber(0),
    mExternalValues(false),
    mIndex(0),
    mGradient(0),
    mSigma(0),
    mLastTime(0),
    mState(0)
{
    mThreshold = events.getDouble("threshold");

    if (events.exist("active")) {
        mActive = events.getBoolean("active");
    }

    if (events.exist("dependance")) {
        mDependance = events.getBoolean("dependance");
    }

    const value::Set& variables = events.getSet("variables");
    unsigned int index;

    for (index = 0; index < variables.size(); ++index) {
        const value::Set& tab(variables.getSet(index));
	std::string name = value::toString(tab.get(0));
	double init = value::toDouble(tab.get(1));
	double precision = value::toDouble(tab.get(2));
        mVariableIndex[name] = index;
        mVariableName[index] = name;
        mVariablePrecision[index] = precision;
        mVariableEpsilon[index] = precision;
        mInitialValueList.push_back(std::pair < unsigned int, double >(
                index, init));
    }
    mVariableNumber = index;
    mValue = new double[index];
    mIndex = new long[index];
    mGradient = new double[index];
    mSigma = new Time[index];
    mLastTime = new Time[index];
    mState = new state[index];
}

Multiple::~Multiple()
{
    delete[] mGradient;
    delete[] mValue;
    delete[] mIndex;
    delete[] mSigma;
    delete[] mLastTime;
    delete[] mState;
}

void Multiple::updateSigma(unsigned int i)
{
    if (std::abs(getGradient(i)) < mThreshold) {
        setSigma(i,devs::infinity);
    } else {
        if (getGradient(i) > 0) {
            setSigma(i, Time((d(i, getIndex(i) + 1)-getValue(i)) /
                                   getGradient(i)));
        } else {
            setSigma(i, Time(((d(i, getIndex(i)) - getValue(i)) -
                                    mVariableEpsilon[i]) / getGradient(i)));
        }
    }
}

void Multiple::minSigma()
{
    // Recherche de la prochaine fonction � calculer
    unsigned int j = 1;
    unsigned int j_min = 0;
    double min = getSigma(0);

    while (j < mVariableNumber) {
        if (min > getSigma(j)) {
            min = getSigma(j);
            j_min = j;
        }
        ++j;
    }
    mCurrentModel = j_min;
}

void Multiple::reset(const Time& time, unsigned int i, double value)
{
    setValue(i, value);
    setIndex(i, (long)(floor(getValue(i)/mVariablePrecision[i])));
    setLastTime(i, time);
    setGradient(i, compute(i, time));
    updateSigma(i);
}

// DEVS Methods

Time Multiple::init(const Time& /* time */)
{
    // Initialisation des variables internes
    std::vector < std::pair < unsigned int , double > >::const_iterator it =
        mInitialValueList.begin();

    while (it != mInitialValueList.end()) {
        mValue[it->first] = it->second;
        ++it;
    }

    for(unsigned int i = 0;i < mVariableNumber;i++) {
        mGradient[i] = 0.0;
        mIndex[i] = (long)(floor(getValue(i)/mVariablePrecision[i]));
        setSigma(i, Time(0));
        setLastTime(i, Time(0));
        setState(i, INIT);
    }
    mCurrentModel = 0;
    return Time(0);
}


void Multiple::output(const Time& time, ExternalEventList& output) const
{
    if (getState(mCurrentModel) == INIT or
        ((getState(mCurrentModel) == POST3 and mExternalValues) or
         (getState(mCurrentModel) == RUN and mActive))) {
        std::map < unsigned int , std::string >::const_iterator it;
        it = mVariableName.find(mCurrentModel);

        if (it == mVariableName.end()) {
            throw utils::ModellingError(fmt(_(
                    "QSS::Multiple: unknown variable: '%1%'")) % it->second);
        }

        ExternalEvent* ee = new ExternalEvent(it->second);
        double e = (time - getLastTime(mCurrentModel));

        ee << attribute("name", it->second);
        ee << attribute("value", getValue(mCurrentModel) + e
                              * getGradient(mCurrentModel));
        ee << attribute("gradient", getGradient(mCurrentModel));
        output.push_back(ee);
    }
}

Time Multiple::timeAdvance() const
{
    return getSigma(mCurrentModel);
}

void Multiple::confluentTransitions(
    const Time& time,
    const ExternalEventList& extEventlist)
{
    externalTransition(extEventlist, time);
    internalTransition(time);
}

void Multiple::internalTransition(const Time& time)
{
    switch (getState(mCurrentModel)) {
    case INIT: // init du gradient
        if (mDependance) {
            for (unsigned int i = 0; i < mVariableNumber; i++) {
                setState(i, POST_INIT);
                setSigma(i, devs::infinity);
            }
        }
        else {
            for (unsigned int i = 0; i < mVariableNumber; i++) {
                setState(i, RUN);
                setGradient(i, compute(i, time));
                updateSigma(i);
            }
            minSigma();
        }
        break;
    case POST2:
        // mise � jour du gradient apr�s reception des valeurs de
        // mes variables externes
        for (unsigned int i = 0; i < mVariableNumber; i++) {
            setState(i, RUN);
            setLastTime(i, time);
            setGradient(i, compute(i, time));
            updateSigma(i);
        }
        minSigma();
        break;
    case POST3:
        // mise � jour du gradient apr�s reception d'une nouvelle
        // valeur pour l'une de mes variables externes
        // et envoie de ma nouvelle valeur aux �quations qui
        // d�pendent de moi si elles existent
        setState(mCurrentModel, RUN);
        updateSigma(mCurrentModel);
        minSigma();
        break;
    case RUN:
        if (getGradient(mCurrentModel) >= 0) incIndex(mCurrentModel);
        else decIndex(mCurrentModel);
        for (unsigned int i = 0; i < mVariableNumber; i++)
            if (i != mCurrentModel)
                setValue(i, getValue(i) + (time -
                                           getLastTime(i)) *
                         getGradient(i));
            else
                setValue(mCurrentModel, d(mCurrentModel,
                                         getIndex(mCurrentModel)));

        // Propagation ou non de la nouvelle valeur
        if (mActive and mExternalValues) {
            // si oui alors on va attendre les valeurs
            // actualis�es de toutes mes variables externes
            setState(mCurrentModel, POST);
            setSigma(mCurrentModel, devs::infinity);
        } else {
            // sinon on passe au pas suivant

            for (unsigned int i = 0; i < mVariableNumber; i++) {
                setState(i, RUN);
                setLastTime(i, time);
                setGradient(i, compute(i, time));
                updateSigma(i);
            }
        }
        minSigma();
        break;
    case POST_INIT:
    case POST:
        break;
    }
}

void Multiple::externalTransition(const ExternalEventList& event,
				  const Time& time)
{
    if (getState(0) == POST_INIT) { // r�ception de la d�finition des variables
                                    // externes
        ExternalEventList::const_iterator it = event.begin();
        unsigned int index = 0;
        unsigned int linear = 0;

        while (it != event.end()) {
            std::string name = (*it)->getStringAttributeValue("name");
            double value = (*it)->getDoubleAttributeValue("value");

            mExternalVariableIndex[name] = index;
            mExternalVariableValue[index] = value;
            if ((mIsGradient[index] = (*it)->existAttributeValue("gradient"))) {
                mExternalVariableGradient[index] =
                    (*it)->getDoubleAttributeValue("gradient");
                ++linear;
            }
            ++index;
            ++it;
        }
        mExternalValues = (linear < mExternalVariableIndex.size());
        mExternalVariableNumber = linear;
        for (unsigned int i = 0; i < mVariableNumber; i++) {
            setState(i, RUN);
            setGradient(i, compute(i, time));
            updateSigma(i);
        }
        minSigma();
    }
    else {
        ExternalEventList::const_iterator it = event.begin();
        bool _reset = false;

        while (it != event.end()) {
            std::string name = (*it)->getStringAttributeValue("name");
            double value = (*it)->getDoubleAttributeValue("value");

            // c'est une variable externe num�rique
            if ((*it)->onPort("update")) {

                if (mVariableIndex.find(name) == mVariableIndex.end()) {
                   throw utils::ModellingError(
                       fmt(_("QSS::Multiple update, invalid variable name:" \
                             " '%1%'")) % name);
                }

                setExternalValue(name, value);
                if (mIsGradient[mExternalVariableIndex[name]])
                    setExternalGradient(
                        name,
                        (*it)->getDoubleAttributeValue("gradient"));
            }
            // c'est une perturbation sur une variable interne
            if ((*it)->onPort("perturb")) {

                if (mVariableIndex.find(name) == mVariableIndex.end()) {
                    throw utils::InternalError(fmt(_(
                            "QSS::Multiple update, invalid variable name: %1%"))
                        % name);
                }

                reset(time, mVariableIndex[name], value);
                _reset = true;
            }
            ++it;
        }
        // Mise � jour des valeurs de variables externes
        if (mExternalVariableNumber > 1) {
            for (unsigned int j = 0 ; j < mExternalVariableNumber ; j++)
                mExternalVariableValue[j] +=
                    (time -
                     getLastTime(0)) * mExternalVariableGradient[j];
        }
        for (unsigned int i = 0; i < mVariableNumber; i++) {
            if (getState(i) == POST) {
                setState(i, POST2);
                setSigma(i, 0);
            }
            else if (getState(i) == RUN) {
                if (_reset) setSigma(i, 0);
                else {
                    // Mise � jour de la valeur
                    setValue(i, getValue(i) +
                             (time - mLastTime[i]) * getGradient(i));
                    setLastTime(i, time);
                    setGradient(i, compute(i, time));

                    setSigma(i, 0);
                    setState(i, POST3);
                }
            }
        }
        minSigma();
    }
}

Value* Multiple::observation(const ObservationEvent& event) const
{
    unsigned int i = mVariableIndex.find(event.getPortName())->second;
    double e = (event.getTime() - getLastTime(i));

    return Double::create(getValue(i)+e*getGradient(i));
}

//void Multiple::request(const RequestEvent& event,
                          //const Time& time,
                          //ExternalEventList& output) const
//{
    //unsigned int i = mVariableIndex.find(
	//event.getStringAttributeValue("name"))->second;
    //double e = (time - getLastTime(i)).getValue();
    //ExternalEvent* ee = new ExternalEvent("response");

    //ee << attribute("name", event.getStringAttributeValue("name"));
    //ee << attribute("value", getValue(i)+e*getGradient(i));
    //output.push_back(ee);
//}

int Multiple::getVariable(const std::string& name) const
{
    std::map < std::string, unsigned int >::const_iterator it;

    it = mVariableIndex.find(name);
    if (it != mVariableIndex.end()) return it->second;
    else return -1;
}

const std::string& Multiple::getVariable(unsigned int index) const
{
    std::map < unsigned int, std::string >::const_iterator it;
    it = mVariableName.find(index);

    if (it == mVariableName.end()) {
        throw utils::InternalError(fmt(_(
                "QSS::Multiple model, unknow variable index '%1%'")) % index);
    }

    return it->second;
}

}}} // namespace vle extension QSS
