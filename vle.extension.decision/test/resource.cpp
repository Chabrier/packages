/*
 * @file vle/extension/decision/test/ss.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
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

#include <vle/utils/unit-test.hpp>
#include <iostream>
#include <iterator>
#include <boost/lexical_cast.hpp>
#include <vle/vle.hpp>
#include <vle/value/Double.hpp>
#include <vle/utils/Context.hpp>
#include <vle/extension/decision/KnowledgeBase.hpp>

namespace vmd = vle::extension::decision;

namespace vle { namespace extension { namespace decision { namespace ex {
    class Resourcetest_0: public vmd::KnowledgeBase
    {
    public:
        Resourcetest_0(vle::utils::ContextPtr ctxp)
            : vmd::KnowledgeBase(ctxp)
        {
            vmd::Activity& A = addActivity("A", 1.0, 10.0);
            vmd::Activity& B = addActivity("B", 1.0, 10.0);

            addResources("Farmer", "Bob");

            A.getParams().addString("resources", "Farmer");
            B.getParams().addString("resources", "Farmer");

            A.freeRessources();
            B.freeRessources();

        }

        virtual ~Resourcetest_0() {}
    };

    class Resourcetest_1: public vmd::KnowledgeBase
    {
    public:
        Resourcetest_1(vle::utils::ContextPtr ctxp)
            : vmd::KnowledgeBase(ctxp)
        {
            vmd::Activity& A = addActivity("A", 1.0, 10.0);
            vmd::Activity& B = addActivity("B", 1.0, 10.0);

            addResources("Farmer", "Bob");

            A.getParams().addString("resources", "Farmer");
            B.getParams().addString("resources", "Farmer");

            A.freeRessources();
            B.freeRessources();

            B.setPriority(1.);
        }

        virtual ~Resourcetest_1() {}
    };

    class Resourcetest_2: public vmd::KnowledgeBase
    {
    public:
        Resourcetest_2(vle::utils::ContextPtr ctxp)
            : vmd::KnowledgeBase(ctxp)
        {
            vmd::Activity& A = addActivity("A", 1.0, 10.0);
            vmd::Activity& B = addActivity("B", 1.0, 10.0);

            addResources("Farmer", "Bob");

            A.getParams().addString("resources", "Farmer");
            B.getParams().addString("resources", "Farmer");

            A.freeRessources();
            B.freeRessources();
        }

        virtual ~Resourcetest_2() {}
    };

    class Resourcetest_3: public vmd::KnowledgeBase
    {
    public:
        Resourcetest_3(vle::utils::ContextPtr ctxp)
            : vmd::KnowledgeBase(ctxp)
        {
            vmd::Activity& A = addActivity("A", 1.0, 10.0);
            vmd::Activity& B = addActivity("B", 1.0, 10.0);
            vmd::Activity& C = addActivity("C", 1.0, 10.0);

            addResources("Farmer", "Bob");
            addResources("Farmer", "Bill");
            addResources("Worker", "Tim");

            //checkResources(true);

            A.getParams().addString("resources", "Bob+Bob|Bob|Bill");
            B.getParams().addString("resources", "Bob|Bill");
            C.getParams().addString("resources", "Tim|Bob|Bill");

            A.freeRessources();
            B.freeRessources();
            C.freeRessources();
        }

        virtual ~Resourcetest_3() {}
    };

    class Resourcetest_4: public vmd::KnowledgeBase
    {
    public:
        Resourcetest_4(vle::utils::ContextPtr ctxp)
            : vmd::KnowledgeBase(ctxp)
        {
            addRes("resFunc_1",  boost::bind(&Resourcetest_4::resFunc_1, this, _1, _2));
            addRes("resFunc_2", boost::bind(&Resourcetest_4::resFunc_2, this, _1, _2));

            vmd::Activity& A = addActivity("A", 1.0, 10.0);
            vmd::Activity& B = addActivity("B", 1.0, 10.0);
            vmd::Activity& C = addActivity("C", 1.0, 10.0);

            addResources("Farmer", "Bob");
            addResources("Farmer", "Bill");
            addResources("Worker", "Tim");

            A.getParams().addString("resources", applyRes("resFunc_1","A",A));
            B.getParams().addString("resources", applyRes("resFunc_1","B",B));
            C.getParams().addString("resources", applyRes("resFunc_2","C",C));

            A.freeRessources();
            B.freeRessources();
            C.freeRessources();
        }

        virtual ~Resourcetest_4() {}

        std::string resFunc_1(const std::string& /*name*/,
                              const Activity& /*activity*/) const
        {
            return "Bob|Bill";
        }


        std::string resFunc_2(const std::string& /*name*/,
                              const Activity& /*activity*/) const
        {
            return "Tim|Bob|Bill";
        }

    };

    class Resourcetest_5: public vmd::KnowledgeBase
    {
    public:
        Resourcetest_5(vle::utils::ContextPtr ctxp)
            : vmd::KnowledgeBase(ctxp)
        {
            vmd::Activity& A = addActivity("A", 1.0, 10.0);
            vmd::Activity& B = addActivity("B", 1.0, 10.0);
            vmd::Activity& C = addActivity("C", 1.0, 10.0);
            vmd::Activity& D = addActivity("D", 1.0, 10.0);

            for (int i = 1; i<= 13; i++){
                std::string female = "ff1"+ boost::lexical_cast<std::string>(i) ;
                addResources("farmA",female);
                addResources("female",female);
            }

            for (int i = 1; i<= 13; i++){
                std::string male = "mf1"+boost::lexical_cast<std::string>(i) ;
                addResources("farmA",male);
                addResources("male",male);
            }

            for (int i = 1; i<= 25; i++){
                std::string female = "ff2"+boost::lexical_cast<std::string>(i) ;
                addResources("farmB",female);
                addResources("female",female);
            }

            for (int i = 1; i<= 13; i++){
                std::string male = "mf2"+boost::lexical_cast<std::string>(i) ;
                addResources("farmB",male);
                addResources("male",male);
            }

            addResources("bullock","bf11");
            addResources("tractor","tf11");
            addResources("farmA","bf11");
            addResources("farmA","tf11");

            addResources("bullock","bf21");
            addResources("tractor","tf21");
            addResources("farmB","bf21");
            addResources("farmB","tf21");

            for (int i = 1; i<= 300; i++){
                std::string female = "fv"+boost::lexical_cast<std::string>(i) ;
                addResources("farmA",female);
                addResources("farmB",female);
                addResources("female",female);
            }

            for (int i = 1; i<= 300; i++){
                std::string male = "mv"+boost::lexical_cast<std::string>(i) ;
                addResources("farmA",male);
                addResources("farmB",male);
                addResources("male",male);
            }

            addResources("tractor","tv1");
            addResources("tractor","tv2");
            //addResources("tractor","tv3");
            addResources("farmA","tv1");
            addResources("farmA","tv2");
            addResources("farmA","tv3");
            addResources("farmB","tv1");
            addResources("farmB","tv2");
            addResources("farmB","tv3");


            //addResources("tractor","tv4");
            //addResources("tractor","tv5");
            //addResources("tractor","tv6");
             addResources("farmA","tv4");
             addResources("farmA","tv5");
             addResources("farmA","tv6");
             addResources("farmB","tv4");
             addResources("farmB","tv5");
             addResources("farmB","tv6");


             addResources("bullock","bv1");
             addResources("bullock","bv2");
             addResources("bullock","bv3");
             addResources("bullock","bv4");
             addResources("bullock","bv5");
             addResources("bullock","bv6");
             addResources("bullock","bv7");
             addResources("bullock","bv8");
             addResources("bullock","bv9");
             addResources("bullock","bv10");
             addResources("bullock","bv11");
             addResources("bullock","bv12");
             addResources("bullock","bv13");
             addResources("bullock","bv14");
             addResources("bullock","bv15");

             addResources("farmA","bv1");
             addResources("farmA","bv2");
             addResources("farmA","bv3");
             addResources("farmA","bv4");
             addResources("farmA","bv5");
             addResources("farmA","bv6");
             addResources("farmA","bv7");
             addResources("farmA","bv8");
             addResources("farmA","bv9");
             addResources("farmA","bv10");
             addResources("farmA","bv11");
             addResources("farmA","bv12");
             addResources("farmA","bv13");
             addResources("farmA","bv14");
             addResources("farmA","bv15");

             addResources("farmB","bv1");
             addResources("farmB","bv2");
             addResources("farmB","bv3");
             addResources("farmB","bv4");
             addResources("farmB","bv5");
             addResources("farmB","bv6");
             addResources("farmB","bv7");
             addResources("farmB","bv8");
             addResources("farmB","bv9");
             addResources("farmB","bv10");
             addResources("farmB","bv11");
             addResources("farmB","bv12");
             addResources("farmB","bv13");
             addResources("farmB","bv14");
             addResources("farmB","bv15");

            A.getParams().addString("resources", "male&farmA.8 + tractor&farmA");
            B.getParams().addString("resources", "male&farmA.12 + tractor&farmA");
            C.getParams().addString("resources", "male&farmB.15 + tractor&farmB");
            D.getParams().addString("resources", "male&farmB.19 + tractor&farmB.2");

            A.freeRessources();
            B.freeRessources();
            C.freeRessources();
            D.freeRessources();
        }

        virtual ~Resourcetest_5() {}
    };


}}}} // namespace vle extension decision ex

/**
 * This test is here to exibit resources managment.
 *
 * Expected behavior :
 * A receive an ack "done" at 2
 * A : 0--WAIT--1--STARTED--2--DONE----------10
 * B : 0--WAIT-------------------6--STARTED--10
 *
 */
void resource_0()
{
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::Resourcetest_0 base(ctxp);
    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    {
        const vmd::Activity& A =  base.activities().get("A")->second;
        const vmd::Activity& B =  base.activities().get("B")->second;
        EnsuresEqual(A.isInWaitState(), true);
        EnsuresEqual(B.isInWaitState(), true);
    }
    base.processChanges(1.0);
    {
        const vmd::Activity& A =  base.activities().get("A")->second;
        const vmd::Activity& B =  base.activities().get("B")->second;
        EnsuresEqual(A.isInStartedState(), true);
        vmd::ActivitiesResourcesConstIteratorPair pit;
        pit = base.activities().resources("A");
        EnsuresEqual(std::distance(pit.first, pit.second), 1);
        EnsuresEqual((*(pit.first)).second, "Bob");
        EnsuresEqual(B.isInWaitState(), true);
    }
    base.setActivityDone("A",2.0);
    base.processChanges(2.0);
    {
        const vmd::Activity& A =  base.activities().get("A")->second;
        const vmd::Activity& B =  base.activities().get("B")->second;
        EnsuresEqual(A.isInDoneState(), true);
        EnsuresEqual(B.isInStartedState(), true);
    }
}

void resource_1()
{
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::Resourcetest_1 base(ctxp);
    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    {
        const vmd::Activity& A =  base.activities().get("A")->second;
        const vmd::Activity& B =  base.activities().get("B")->second;
        EnsuresEqual(A.isInWaitState(), true);
        EnsuresEqual(B.isInWaitState(), true);
    }
    base.processChanges(1.0);
    {
        const vmd::Activity& A =  base.activities().get("A")->second;
        const vmd::Activity& B =  base.activities().get("B")->second;
        EnsuresEqual(B.isInStartedState(), true);
        EnsuresEqual(A.isInWaitState(), true);
    }
    base.setActivityDone("B",2.0);
    base.processChanges(2.0);
    {
        const vmd::Activity& A =  base.activities().get("A")->second;
        const vmd::Activity& B =  base.activities().get("B")->second;
        EnsuresEqual(B.isInDoneState(), true);
        EnsuresEqual(A.isInStartedState(), true);
    }
}

void resource_2()
{
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::Resourcetest_2 base(ctxp);
    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    {
        const vmd::Activity& A =  base.activities().get("A")->second;
        const vmd::Activity& B =  base.activities().get("B")->second;
        EnsuresEqual(A.isInWaitState(), true);
        EnsuresEqual(B.isInWaitState(), true);
    }
    base.processChanges(1.0);
    {
        const vmd::Activity& A =  base.activities().get("A")->second;
        const vmd::Activity& B =  base.activities().get("B")->second;
        EnsuresEqual(A.isInStartedState(), true);
        vmd::ActivitiesResourcesConstIteratorPair pit;
        pit = base.activities().resources("A");
        EnsuresEqual(std::distance(pit.first, pit.second), 1);
        EnsuresEqual((*(pit.first)).second, "Bob");
        EnsuresEqual(B.isInWaitState(), true);
    }
    base.setActivityDone("A",2.0);
    base.processChanges(2.0);
    {
        const vmd::Activity& A =  base.activities().get("A")->second;
        const vmd::Activity& B =  base.activities().get("B")->second;
        EnsuresEqual(A.isInDoneState(), true);
        EnsuresEqual(B.isInStartedState(), true);
    }
}

void resource_3()
{
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::Resourcetest_3 base(ctxp);
    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    {
        const vmd::Activity& A =  base.activities().get("A")->second;
        const vmd::Activity& B =  base.activities().get("B")->second;
        const vmd::Activity& C =  base.activities().get("C")->second;
        EnsuresEqual(A.isInWaitState(), true);
        EnsuresEqual(B.isInWaitState(), true);
        EnsuresEqual(C.isInWaitState(), true);
    }

    base.processChanges(1.0);
    {
        const vmd::Activity& A =  base.activities().get("A")->second;
        const vmd::Activity& B =  base.activities().get("B")->second;
        const vmd::Activity& C =  base.activities().get("C")->second;
        EnsuresEqual(A.isInStartedState(), true);
        {
            vmd::ActivitiesResourcesConstIteratorPair pit;
            pit = base.activities().resources("A");
            EnsuresEqual(std::distance(pit.first, pit.second), 1);
            EnsuresEqual((*(pit.first)).second, "Bob");
        }
        EnsuresEqual(B.isInStartedState(), true);
        {
            vmd::ActivitiesResourcesConstIteratorPair pit;
            pit = base.activities().resources("B");
            EnsuresEqual(std::distance(pit.first, pit.second), 1);
            EnsuresEqual((*(pit.first)).second, "Bill");
        }
        EnsuresEqual(C.isInStartedState(), true);
        {
            vmd::ActivitiesResourcesConstIteratorPair pit;
            pit = base.activities().resources("C");
            EnsuresEqual(std::distance(pit.first, pit.second), 1);
            EnsuresEqual((*(pit.first)).second, "Tim");
        }
    }
}

void resource_4()
{
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::Resourcetest_4 base(ctxp);
    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    {
        const vmd::Activity& A =  base.activities().get("A")->second;
        const vmd::Activity& B =  base.activities().get("B")->second;
        const vmd::Activity& C =  base.activities().get("C")->second;
        EnsuresEqual(A.isInWaitState(), true);
        EnsuresEqual(B.isInWaitState(), true);
        EnsuresEqual(C.isInWaitState(), true);
    }

    base.processChanges(1.0);
    {
        const vmd::Activity& A =  base.activities().get("A")->second;
        const vmd::Activity& B =  base.activities().get("B")->second;
        const vmd::Activity& C =  base.activities().get("C")->second;
        EnsuresEqual(A.isInStartedState(), true);
        {
            vmd::ActivitiesResourcesConstIteratorPair pit;
            pit = base.activities().resources("A");
            EnsuresEqual(std::distance(pit.first, pit.second), 1);
            EnsuresEqual((*(pit.first)).second, "Bob");
        }
        EnsuresEqual(B.isInStartedState(), true);
        {
            vmd::ActivitiesResourcesConstIteratorPair pit;
            pit = base.activities().resources("B");
            EnsuresEqual(std::distance(pit.first, pit.second), 1);
            EnsuresEqual((*(pit.first)).second, "Bill");
        }
        EnsuresEqual(C.isInStartedState(), true);
        {
            vmd::ActivitiesResourcesConstIteratorPair pit;
            pit = base.activities().resources("C");
            EnsuresEqual(std::distance(pit.first, pit.second), 1);
            EnsuresEqual((*(pit.first)).second, "Tim");
        }
    }
}

void resource_5()
{
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::Resourcetest_5 base(ctxp);
    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    {
        const vmd::Activity& A =  base.activities().get("A")->second;
        const vmd::Activity& B =  base.activities().get("B")->second;
        const vmd::Activity& C =  base.activities().get("C")->second;
        const vmd::Activity& D =  base.activities().get("D")->second;
        EnsuresEqual(A.isInWaitState(), true);
        EnsuresEqual(B.isInWaitState(), true);
        EnsuresEqual(C.isInWaitState(), true);
        EnsuresEqual(D.isInWaitState(), true);
    }

    base.processChanges(1.0);
    {
        const vmd::Activity& A =  base.activities().get("A")->second;
        const vmd::Activity& B =  base.activities().get("B")->second;
        const vmd::Activity& C =  base.activities().get("C")->second;
        const vmd::Activity& D =  base.activities().get("D")->second;
        EnsuresEqual(A.isInWaitState(), true);
        EnsuresEqual(B.isInStartedState(), true);
        EnsuresEqual(C.isInStartedState(), true);
        EnsuresEqual(D.isInStartedState(), true);
    }
}

int main()
{
    resource_0();
    resource_1();
    resource_2();
    resource_3();
    resource_4();
    resource_5();

    return unit_test::report_errors();
}
