/*
 * @file vle/extension/differential-equation/QSS2.cpp
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


#include <vle/extension/differential-equation/QSS2.hpp>
#include <vle/value/Map.hpp>
#include <cmath>

namespace vle { namespace extension { namespace DifferentialEquation {

using namespace devs;
using namespace value;

mfi::mfi(QSS2* qss,unsigned int i,unsigned int n):
    m_qss(qss),
    index(i),
    m_functionNumber(n)
{
    c = new double[m_functionNumber];
    z = new double[m_functionNumber];
    mz = new double[m_functionNumber];
    xt = new double[m_functionNumber];
    yt = new double[m_functionNumber];
}

mfi::~mfi()
{
    delete[] yt;
    delete[] xt;
    delete[] mz;
    delete[] z;
    delete[] c;
}

Couple mfi::init()
{
    double* sav = new double[m_functionNumber];
    Couple ret;

    for(unsigned int j = 0;j < m_functionNumber;j++)
        z[j] = m_qss->getValue(j);
    for(unsigned int j = 0;j < m_functionNumber;j++)
        mz[j] =  m_qss->compute(j);
    fiz = mz[index];
    for(unsigned int j = 0;j < m_functionNumber;j++)  {
        for(unsigned int k = 0;k < m_functionNumber;k++)
            xt[k] = z[k];
        xt[j] += m_qss->m_precision;

        for(unsigned int k = 0;k < m_functionNumber;k++) {
            sav[k] = m_qss->getValue(k);
            m_qss->setValue(k,xt[k]);
        }
        for(unsigned int k = 0;k < m_functionNumber;k++)
            yt[k] = m_qss->compute(k);
        for(unsigned int k = 0;k < m_functionNumber;k++)
            m_qss->setValue(k,sav[k]);
        c[j] = (yt[index] - fiz)/m_qss->m_precision;
    }
    ret.value = mz[index];
    ret.derivative = 0;
    for(unsigned int j = 0;j < m_functionNumber;j++)
        ret.derivative += c[j]*mz[j];
    m_lastTime = Time(0);
    return ret;
}

Couple mfi::ext(const Time& p_time, unsigned int i,const Couple & input)
{
    Couple ret;
    double e = (p_time - m_lastTime);
    double normal;
    double* zz = new double[m_functionNumber];
    double* sav = new double[m_functionNumber];

    m_lastTime = p_time;
    normal = z[i]+mz[i]*e;
    for(unsigned int j = 0;j < m_functionNumber;j++)
        zz[j] = z[j]+mz[j]*e;
    for(unsigned int j = 0;j < m_functionNumber;j++)
        if (j==i) {
            z[j]=input.value;
            mz[j]=input.derivative;
        }
        else z[j]+=mz[j]*e;
    /*  for(unsigned int k = 0;k < m_functionNumber;k++)
        yt[k] = m_qss->compute(k);
        ret.value = yt[index]; */

    for(unsigned int j = 0;j < m_functionNumber;j++) {
        sav[j] = m_qss->getValue(j);
        m_qss->setValue(j,z[j]);
    }
    ret.value = m_qss->compute(index);
    for(unsigned int j = 0;j < m_functionNumber;j++)
        m_qss->setValue(j,sav[j]);

    if (fabs(normal - z[i]) >= m_qss->m_threshold) {
        for(unsigned int j = 0;j < m_functionNumber;j++) {
            sav[j] = m_qss->getValue(j);
            m_qss->setValue(j,zz[j]);
        }

        double w = m_qss->compute(index);

        for(unsigned int j = 0;j < m_functionNumber;j++)
            m_qss->setValue(j,sav[j]);
        c[i] = (w-ret.value)/(normal-z[i]);
    }

    ret.derivative = 0;
    for(unsigned int j = 0;j < m_functionNumber;j++)
        ret.derivative+=c[j]*mz[j];
    delete[] sav;
    delete[] zz;
    return ret;
}

QSS2::QSS2(const DynamicsInit& p_model,
           const InitEventList& events) :
    Dynamics(p_model, events),
    m_value(0),
    m_derivative(0),
    m_derivative2(0),
    m_index(0),
    m_index2(0),
    m_sigma(0),m_lastTime(0),m_mfi(0)
{
    m_precision = events.getDouble("precision");
    m_epsilon = m_precision;

    m_threshold = events.getDouble("threshold");

    m_active = events.getBoolean("active");

    const Map& variables = events.getMap("variables");
    const MapValue& lst = variables.value();

    m_functionNumber = lst.size();

    m_value = new double[m_functionNumber];
    m_derivative = new double[m_functionNumber];
    m_derivative2 = new double[m_functionNumber];
    m_index = new double[m_functionNumber];
    m_index2 = new double[m_functionNumber];
    m_sigma = new Time[m_functionNumber];
    m_lastTime = new Time[m_functionNumber];
    m_mfi = new pmfi[m_functionNumber];
    for(unsigned int i = 0;i < m_functionNumber;i++)
        m_mfi[i] = new mfi(this,i,m_functionNumber);

    for (MapValue::const_iterator it = lst.begin(); it != lst.end();
         ++it) {
        const Set& tab(toSetValue(*it->second));

        unsigned int index = toInteger(tab.get(0));
        double init = toDouble(tab.get(1));
        m_variableIndex[it->first] = index;
        m_variableName[index] = it->first;
        m_initialValueList.push_back(std::pair < unsigned int, double >(
                index, init));
    }
}

double QSS2::getDerivative(unsigned int i) const
{
    return m_derivative[i];
}

double QSS2::getDerivative2(unsigned int i) const
{
    return m_derivative2[i];
}

double QSS2::getIndex(unsigned int i) const
{
    return m_index[i];
}

double QSS2::getIndex2(unsigned int i) const
{
    return m_index2[i];
}

const Time & QSS2::getLastTime(unsigned int i) const
{
    return m_lastTime[i];
}

const Time & QSS2::getSigma(unsigned int i) const
{
    return m_sigma[i];
}

void QSS2::setIndex(unsigned int i,double p_index)
{
    m_index[i] = p_index;
}

void QSS2::setIndex2(unsigned int i,double p_index)
{
    m_index2[i] = p_index;
}

void QSS2::setDerivative(unsigned int i,double p_derivative)
{
    m_derivative[i] = p_derivative;
}

void QSS2::setDerivative2(unsigned int i,double p_derivative)
{
    m_derivative2[i] = p_derivative;
}

void QSS2::setLastTime(unsigned int i,const Time & p_time)
{
    m_lastTime[i] = p_time;
}

void QSS2::setSigma(unsigned int i,const Time & p_time)
{
    m_sigma[i] = p_time;
}

void QSS2::setValue(unsigned int i,double p_value)
{
    m_value[i] = p_value;
}

void QSS2::updateSigma(unsigned int) //i)
{
    // Calcul du sigma de la i�me fonction
    //   if (std::abs(getGradient(i)) < m_threshold)
    //     setSigma(i,Time::infinity);
    //   else
    //     if (getGradient(i) > 0)
    //       setSigma(i,Time((d(getIndex(i)+1)-getValue(i))/getGradient(i)));
    //     else
    //       setSigma(i,Time(((d(getIndex(i))-getValue(i))-m_epsilon)
    // 			    /getGradient(i)));
}

void QSS2::minSigma()
{
    // Recherche de la prochaine fonction � calculer
    unsigned int j = 1;
    unsigned int j_min = 0;
    double v_min = getSigma(0);

    while (j < m_functionNumber)
    {
        if (v_min > getSigma(j)) {
            v_min = getSigma(j);
            j_min = j;
        }
        ++j;
    }
    m_currentModel = j_min;
}

bool QSS2::delta(double a,double b,double c,double& s)
{
    if (a!=0) {
        double d = b*b-4*a*c;
        double s1 ,s2;

        if (d<0) return false;
        s1 = (-b+sqrt(d))/(2*a);
        s2 = (-b-sqrt(d))/(2*a);
        if (s2<s1) {
            std::swap(s1, s2);
        }
        if (s1>0) {
            s = s1;
            return true;
        } else {
            if (s2>0) {
                s = s2;
                return true;
            }
        }
    } else {
        if (b == 0) return false;
        s = -c/b;
        if (s>0) return true;
    }
    return false;
}

bool QSS2::intermediaire(double a,double b,double c,double cp,double& s)
{
    double s1,s2;

    if (delta(a,b,c,s1))
        if (delta(a,b,cp,s2))
            if (s1<s2)
            {
                s = s1;
                return true;
            }
            else
            {
                s = s2;
                return true;
            }
        else
        {
            s = s1;
            return true;
        }
    else
    {
        if (delta(a,b,cp,s2))
        {
            s = s2;
            return true;
        }
    }
    return false;
}

// DEVS Methods

void QSS2::finish()
{
    delete[] m_value;
    delete[] m_derivative;
    delete[] m_derivative2;
    delete[] m_index;
    delete[] m_index2;
    delete[] m_sigma;
    delete[] m_lastTime;
    for(unsigned int i = 0;i < m_functionNumber;i++)
        delete m_mfi[i];
    delete[] m_mfi;
}

void QSS2::init2()
{
    for(unsigned int i = 0;i < m_functionNumber;i++)
    {
        m_index[i] = getValue(i);

        Couple res = m_mfi[i]->init();

        m_derivative[i] = res.value;
        m_derivative2[i] = res.derivative;

        m_index2[i] = m_derivative[i];
        if (fabs(m_derivative2[i]) < m_threshold)
            setSigma(i, devs::infinity);
        else
            setSigma(i,Time(sqrt(2*m_precision/fabs(m_derivative2[i]))));
        setLastTime(i,Time(0));
    }
    minSigma();
}

Time QSS2::init(const devs::Time& /* time */)
{
    std::vector < std::pair < unsigned int , double > >::const_iterator it =
        m_initialValueList.begin();

    while (it != m_initialValueList.end()) {
        setValue(it->first,it->second);
        ++it;
    }
    init2();
    return getSigma(m_currentModel);
}

void QSS2::output(const Time& /* time */,
                  ExternalEventList& output) const
{
    if (m_active) {
        ExternalEvent* ee = new ExternalEvent("out");

        for (unsigned int i = 0; i < m_functionNumber ; i++) {
            std::map < unsigned int , std::string >::const_iterator it;
            it = m_variableName.find(i);

            if (it == m_variableName.end()) {
                throw utils::ModellingError(fmt(_(
                        "QSS2: unknow variable %1%")) % it->second);
            }
            ee << attribute(it->second, getValue(i));
        }
        output.push_back(ee);
    }
}

Time QSS2::timeAdvance() const
{
    return getSigma(m_currentModel);
}

void QSS2::confluentTransitions(
    const Time& time,
    const ExternalEventList& extEventlist)
{
    externalTransition(extEventlist, time);
    internalTransition(time);
}

void QSS2::internalTransition(const Time& time)
{
    unsigned int i = m_currentModel;
    Couple input;

    //  std::cout << "[" << i << "] int at "
    //	    << time << std::endl;

    input.value = m_value[i]+m_derivative[i]*m_sigma[i]
        +m_derivative2[i]/2*m_sigma[i] * m_sigma[i];
    input.derivative = m_derivative[i]+m_derivative2[i]*m_sigma[i];

    //   std::cout << time
    // 	    << " : " << input.value << " ; "
    // 	    << input.derivative << std::endl;


    // Mise � jour des sigma
    //   for (unsigned int j = 0;j < m_functionNumber;j++)
    //     m_sigma[j]-=(event->getTime() - m_lastTime[j]).getValue();

    // Mise � jour de l'index
    m_value[i]+=m_derivative[i] * m_sigma[i]
        +(m_derivative2[i]/2)*m_sigma[i] * m_sigma[i];
    m_index[i] = m_value[i];
    m_index2[i] = m_derivative[i]+ m_derivative2[i]*m_sigma[i];
    m_derivative[i]+=m_derivative2[i]*m_sigma[i];

    if (fabs(m_derivative2[i]) < m_threshold)
        m_sigma[i] = devs::infinity;
    else m_sigma[i] = Time(sqrt(2*m_precision/fabs(m_derivative2[i])));
    setLastTime(i, time);

    // Mise � jour des autres �quations
    for (unsigned int j = 0;j < m_functionNumber;j++) {
        double e = (time - getLastTime(j));

        if (e==0) m_mfi[j]->ext(time, i, input);
        else {
            setLastTime(j, time);

            Couple ret = m_mfi[j]->ext(time ,i,input);

            m_value[j]+=m_derivative[j]*e+(m_derivative2[j]/2)*e*e;
            m_index[j]+=m_index2[j]*e;

            m_derivative[j] = ret.value;
            m_derivative2[j] = ret.derivative;

            double a = m_derivative2[j]/2;
            double b = m_derivative[j] - m_index2[j];
            double c = m_value[j] - m_index[j] + m_precision;
            double cp = m_value[j] - m_index[j] - m_precision;
            double s;

            if (intermediaire(a,b,c,cp,s)) m_sigma[j] = Time(s);
            else m_sigma[j] = devs::infinity;
        }

        //      std::cout << " -> [" << j << "] = " << m_sigma[j].getValue() << std::endl;

    }
    minSigma();
}

void QSS2::externalTransition(const ExternalEventList& event,
                              const Time& /* time */)
{
    ExternalEventList::const_iterator it = event.begin();

    while (it != event.end()) {
        if ((*it)->onPort("parameter")) {
            std::string v_name = (*it)->getStringAttributeValue("name");
            double v_value = (*it)->getDoubleAttributeValue("value");

            processPerturbation(v_name,v_value);

            init2();
        }
    }
}

Value* QSS2::observation(const ObservationEvent& event) const
{
    //  unsigned int i = to_int(event->getPortName());
    unsigned int i = m_variableIndex.find(event.getPortName())->second;
    double e = (event.getTime()-m_lastTime[i]);
    double x = m_value[i]+m_derivative[i]*e+m_derivative2[i]/2*e*e;

    return Double::create(x);
}

}}} // namespace vle extension DifferentialEquation
