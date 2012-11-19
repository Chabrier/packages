/*
 * @file vle/examples/counter/UniformGenerator.hpp
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


#ifndef EXAMPLES_RANDOM_UNIFORM_GENERATOR_HPP
#define EXAMPLES_RANDOM_UNIFORM_GENERATOR_HPP

#include <vle/examples/counter/RandomGenerator.hpp>

namespace vle { namespace examples { namespace generator {

    class UniformGenerator : public RandomGenerator
    {
    public:
        UniformGenerator(vle::utils::Rand& rnd, double min,double max) :
            RandomGenerator(rnd),
            m_min(min),
            m_max(max)
        { }

        virtual ~UniformGenerator() { }

        virtual double generate();

    private:
        double m_min;
        double m_max;
    };

}}} // namesapce vle examples generator

#endif
