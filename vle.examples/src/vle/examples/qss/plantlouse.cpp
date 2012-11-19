/*
 * @file vle/examples/qss/plantlouse.cpp
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
#include <cmath>

namespace vle { namespace examples { namespace qss {

class Plantlouse : public extension::QSS::Simple
{
public:
    Plantlouse(const devs::DynamicsInit& model,
	       const devs::InitEventList& events);

    virtual ~Plantlouse();

    virtual double compute(const devs::Time& time) const;

private:
    Var x;
    Ext y;

    double a;
    double b;
};

Plantlouse::Plantlouse(const devs::DynamicsInit& model,
                       const devs::InitEventList& events) :
    extension::QSS::Simple(model, events)
{
    a = value::toDouble(events.get("a"));
    b = value::toDouble(events.get("b"));

    x = createVar("x");
    y = createExt("y");
}

Plantlouse::~Plantlouse()
{
}

double Plantlouse::compute(const vle::devs::Time& /* time */) const
{
    return a * x() - b * y() * x();
}

}}} // namespace vle examples qss

DECLARE_DYNAMICS(vle::examples::qss::Plantlouse)
