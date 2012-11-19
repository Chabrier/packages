/*
 * @file vle/examples/dess/I2.cpp
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


#include "I2.hpp"
#include <cmath>

namespace vle { namespace examples { namespace dess {

I2::I2(const devs::DynamicsInit& model,
       const devs::InitEventList& evList):
    extension::DifferentialEquation::DESS(model, evList)
{
    // birth and death rate
    m = value::toDouble(evList.get("m"));
    // rate at which infected individuals (I) recover (R)
    g = value::toDouble(evList.get("g"));
    // rate at which exposed individuals (E) become infected (I)
    a = value::toDouble(evList.get("a"));

    I = createVar("I");
    E = createExt("E");
}

double I2::compute(const vle::devs::Time& /* time */) const
{
    return a * E() - (m + g) * I();
}

DECLARE_DYNAMICS(I2)

}}} // namespace vle examples dess
