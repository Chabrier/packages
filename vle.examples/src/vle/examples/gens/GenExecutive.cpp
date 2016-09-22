/*
 * @file vle/examples/gens/GenExecutive.cpp
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


#include <vle/devs/Executive.hpp>
#include <vle/utils/Tools.hpp>
#include <stack>
#include <sstream>

namespace vle { namespace examples { namespace gens {

class GenExecutive : public devs::Executive
{
    enum state { INIT, IDLE, ADDMODEL, DELMODEL };

    std::stack < std::string >  m_stacknames;
    state                       m_state;

public:
    GenExecutive(const devs::ExecutiveInit& mdl,
                 const devs::InitEventList& events) :
        devs::Executive(mdl, events), m_state(INIT)
    {
    }

    virtual ~GenExecutive()
    {
    }

    virtual devs::Time init(devs::Time /* time */) override
    {
        vpz::Dynamic dyn("gensbeep");
        dyn.setPackage("vle.examples");
        dyn.setLibrary("GensBeep");
        dynamics().add(dyn);

        m_state = INIT;

        return 0.0;
    }

    virtual devs::Time timeAdvance() const override
    {
        switch (m_state) {
        case INIT:
            return 0.0;
        case IDLE:
            return 0.0;
        case ADDMODEL:
            return 1.0;
        case DELMODEL:
            return 1.0;
        }
        throw utils::InternalError("GenExecutive ta");
    }

    virtual void internalTransition(devs::Time time) override
    {
        switch (m_state) {
        case INIT:
            m_state = IDLE;
            break;
        case IDLE:
            if (time < 50.0) {
                m_state = ADDMODEL;
            }
            else {
                m_state = DELMODEL;
            }
            break;
        case ADDMODEL:
            add_new_model();
            m_state = IDLE;
            break;
        case DELMODEL:
            del_first_model();
            m_state = IDLE;
            break;
        }
    }

    virtual std::unique_ptr<value::Value> observation(
    		const devs::ObservationEvent& ev) const override
    {
        if (ev.onPort("nbmodel")) {
            return value::Integer::create(get_nb_model());
        } else if (ev.onPort("structure")) {
            std::ostringstream out;
            coupledmodel().writeXML(out);
            return value::String::create(out.str());
        } else if (ev.onPort("adjacency_matrix")) {
            auto ret = vle::value::Set::create();
            auto& set = ret->toSet();
            set.add(value::Integer::create(1));
            if(get_nb_model() > 0 and ev.getTime() < 50.0){
                set.add(value::String::create("add"));
                std::string name = vle::utils::format("beep_%i",
                        (int) m_stacknames.size());
                set.add(value::String::create(name));
                set.add(value::String::create("2"));
                std::string edge =  name + std::string(" counter ");
                set.add(value::String::create(edge));
            }
            else if(get_nb_model() > 0){
                set.add(value::String::create("delete"));
                std::string name = vle::utils::format(
                        "beep_%u", get_nb_model());
                set.add(value::String::create(name));
            }

            return ret;
        } else if (ev.onPort("value"))
            return value::Integer::create(1);

        return devs::Executive::observation(ev);
    }

    ///
    ////
    ///

    void add_new_model()
    {
        std::string name(vle::utils::format("beep_%i",
                (int) m_stacknames.size()));

        std::vector < std::string > outputs;
        outputs.push_back("out");

        createModel(name, std::vector < std::string >(), outputs, "gensbeep");
        addConnection(name, "out", "counter", "in");

        m_stacknames.push(name);
    }

    void del_first_model()
    {
        if (m_stacknames.empty()) {
            throw utils::InternalError(
                    "Cannot delete any model, the executive have no "
                    "element.");
        }

        delModel(m_stacknames.top());
        m_stacknames.pop();
    }

    int get_nb_model() const
    {
        return (coupledmodel().getModelList().size()) - 1;
    }

};

}}} // namespace vle examples gens

DECLARE_EXECUTIVE(vle::examples::gens::GenExecutive)
