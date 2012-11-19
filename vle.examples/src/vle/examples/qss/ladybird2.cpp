/*
 * @file vle/examples/qss/ladybird2.cpp
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


#include <vle/extension/difference-equation/Simple.hpp>

namespace vle { namespace examples { namespace qss {

class Ladybird2 : public vle::extension::DifferenceEquation::Simple
{
public:
    Ladybird2(const vle::devs::DynamicsInit& model,
	      const vle::devs::InitEventList& events);

    virtual ~Ladybird2();

    virtual double compute(const vle::devs::Time& /* time */);

private:
    Var y;
    Sync x;

    double b;
    double d;
    double e;
};

Ladybird2::Ladybird2(const devs::DynamicsInit& model,
                     const devs::InitEventList& events) :
    extension::DifferenceEquation::Simple(model, events)
{
    b = value::toDouble(events.get("b"));
    d = value::toDouble(events.get("d"));
    e = value::toDouble(events.get("e"));
    y = createVar("y");
    x = createSync("x");
}

Ladybird2::~Ladybird2()
{
}

double Ladybird2::compute(const vle::devs::Time& time)
{
    return y(-1) + timeStep(time) * (b * d * x(0) * y(-1) - e * y(-1));
}

}}} // namespace vle examples qss

DECLARE_DYNAMICS(vle::examples::qss::Ladybird2)
