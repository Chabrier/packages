/*
 * @file vle/examples/equation/E1.hpp
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


#ifndef EXAMPLES_EQUATION_E1_HPP
#define EXAMPLES_EQUATION_E1_HPP

#include <vle/extension/difference-equation/Multiple.hpp>

namespace vle { namespace examples { namespace equation {

    class E1 : public extension::DifferenceEquation::Multiple
    {
        Var a;
        Var b;
        Var c;

    public:
        E1(const devs::DynamicsInit& model,
           const devs::InitEventList& events) :
            extension::DifferenceEquation::Multiple(model, events)
        {
            a = createVar("a");
            b = createVar("b");
            c = createVar("c");
        }
        virtual ~E1() { }

        virtual void compute(const devs::Time& time);
        virtual void initValue(const devs::Time& time);
    };

}}} // namespace vle examples equation

DECLARE_DYNAMICS(vle::examples::equation::E1)

#endif
