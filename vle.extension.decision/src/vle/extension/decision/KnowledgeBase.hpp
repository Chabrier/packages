/*
 * @file vle/extension/decision/KnowledgeBase.hpp
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


#ifndef VLE_EXT_DECISION_KNOWLEDGEBASE_HPP
#define VLE_EXT_DECISION_KNOWLEDGEBASE_HPP 1

#include <vle/utils/Context.hpp>
#include <vle/extension/decision/Resources.hpp>
#include <vle/extension/decision/Activities.hpp>
#include <vle/extension/decision/Ress.hpp>
#include <vle/extension/decision/Facts.hpp>
#include <vle/extension/decision/Library.hpp>
#include <vle/extension/decision/Rules.hpp>
#include <vle/extension/decision/Table.hpp>
#include <vle/extension/decision/Plan.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>

namespace vle { namespace extension { namespace decision {

typedef Table < ResFct > RessTable;
typedef Table < Fact > FactsTable;
typedef Table < PortFact > PortFactsTable;
typedef Table < PredicateFunction > PredicatesTable;
typedef Table < Activity::AckFct > AcknowledgeFunctions;
typedef Table < Activity::OutFct > OutputFunctions;
typedef Table < Activity::UpdateFct > UpdateFunctions;

inline std::ostream& operator<<(std::ostream& o, const RessTable& kb)
{
    o << "Ress: ";
    for (RessTable::const_iterator it = kb.begin(); it != kb.end(); ++it) {
        o << " (" << it->first << ")";
    }
    return o;
}

inline std::ostream& operator<<(std::ostream& o, const FactsTable& kb)
{
    o << "facts: ";
    for (FactsTable::const_iterator it = kb.begin(); it != kb.end(); ++it) {
        o << " (" << it->first << ")";
    }
    return o;
}

inline std::ostream& operator<<(std::ostream& o, const PortFactsTable& kb)
{
    o << "facts: ";
    for (PortFactsTable::const_iterator it = kb.begin(); it != kb.end(); ++it) {
        o << " (" << it->first << ")";
    }
    return o;
}

template < typename F >
struct r
{
    r(const std::string& name, F func)
        : name(name), func(func)
    {}

    std::string name;
    F func;
};

template < typename F >
struct f
{
    f(const std::string& name, F func)
        : name(name), func(func)
    {}

    std::string name;
    F func;
};

template < typename F >
struct pf
{
    pf(const std::string& name, F func)
        : name(name), func(func)
    {}

    std::string name;
    F func;
};

template < typename F >
struct p
{
    p(const std::string& name, F func)
        : name(name), func(func)
    {}

    std::string name;
    F func;
};

template < typename F >
struct o
{
    o(const std::string& name, F func)
        : name(name), func(func)
    {}

    std::string name;
    F func;
};

template < typename F >
struct a
{
    a(const std::string& name, F func)
        : name(name), func(func)
    {}

    std::string name;
    F func;
};

template < typename F >
struct u
{
    u(const std::string& name, F func)
        : name(name), func(func)
    {}

    std::string name;
    F func;
};

template < typename X >
struct AddRess
{
    AddRess(X kb) : kb(kb) {}

    X kb;
};

template < typename X >
struct AddFacts
{
    AddFacts(X kb) : kb(kb) {}

    X kb;
};

template < typename X >
struct AddPortFacts
{
    AddPortFacts(X kb) : kb(kb) {}

    X kb;
};

template < typename X >
struct AddPredicates
{
    AddPredicates(X kb) : kb(kb) {}

    X kb;
};

template < typename X >
struct AddOutputFunctions
{
    AddOutputFunctions(X kb) : kb(kb) {}

   X kb;
};

template < typename X >
struct AddAcknowledgeFunctions
{
    AddAcknowledgeFunctions(X kb) : kb(kb) {}

    X kb;
};

template < typename X >
struct AddUpdateFunctions
{
    AddUpdateFunctions(X kb) : kb(kb) {}

    X kb;
};

/**
 * @brief KnowledgeBase stores facts, rules, activites and precedence
 * constraints to build a plan activity model.
 * @code
 * // To add facts, predicates, outputs and acknowledge functions, use the
 * // following methods:
 * addFacts(this) +=
 *      F("rain", &KnowledgeBase::rain);
 *
 * addPredicates(this) +=
 *      P("pred 1", &KnowledgeBase::isAlwaysTrue),
 *      P("pred 2", &KnowledgeBase::isAlwaysFalse),
 *      P("pred 3", &KnowledgeBase::randIsTrue,
 *      P("pred 4", &KnowledgeBase::randIsFalse);
 *
 * addOutputFunctions(this) +=
 *      O("output function", &KnowledgeBase::out);
 *
 * addAcknowledgeFunctions(this) +=
 *      A("ack function", &KnowledgeBase::ack);
 *
 * // And use a Parser to fill KnowledgeBase with datas.
 * @endcode
 */
class KnowledgeBase
{
public:
    /**
     * @brief Define the return of the processChanges function. A boolean
     * if this function has changed at least one activity and a date to
     * wake up the knowledge base.
     */
    typedef std::pair < bool, devs::Time > Result;

    KnowledgeBase(utils::ContextPtr ctxp)
        : mPlan(ctxp, *this), mLibrary(ctxp, *this), mResourcesCheck(false)
    {}

    /**
     * @brief if checkRessources is set to true, the simulation will
     * stop, when a resource constraint needs more than all the
     * resources defined, elle only a warning will be raised.
     * @param check
     */

    void checkResources(bool check)
    {
        mResourcesCheck = check;
    }

    bool resourcesCheck()
    {
        return mResourcesCheck;
    }

    /**
     * @brief a resource can be defined by a id(string),
     * @param name the name of the resource
     */
    void addResources(const std::string& name)
    {
        if (not resourceTypeExist(name)) {
            mResources[name].insert(name);
            mPlan.activities().setResourceAvailable(name);
        } else {
            throw utils::ModellingError(
                vle::utils::format("Decision: resource `%s' already defined",
                                   name.c_str()));
        }
    }

    /**
     * @brief a resource is defined by a id(string) an class(string),
     * This operator can be used to use many classes for a single
     * resource
     * @param type the class of a resource
     * @param name the name of the resource
     */
    void addResources(const std::string& type, const std::string& name)
    {
        if (not resourceTypeExist(name)) {
            mResources[name].insert(name);
            mPlan.activities().setResourceAvailable(name);
        }

        std::string resource = type + "&" + name;
        if (not resourceTypeExist(resource)) {
            mResources[type].insert(name);
            mPlan.activities().setResourceAvailable(name);
        } else {
            throw utils::ModellingError(
                vle::utils::format("Decision: resource `%s' already defined",
                                   resource.c_str()));
        }
    }

    /**
     * @brief check if a resource exist
     * @param type a combination of classes separated by '&',
     * for example "farmWorker & skilledWorker"
     */
    bool resourceTypeExist(const std::string& type) const
    {
        size_t n = std::count(type.begin(), type.end(), '&');
        if ( n == 0 ) {
            return mResources.find(type) != mResources.end();
        } else {
            std::vector<std::string> strs;
            boost::split(strs, type , boost::is_any_of("&"));

            Resources::const_iterator its = mResources.find(strs[0]);

            if ( its == mResources.end() ) {
                return false;
            }

            std::vector< std::string > result (its->second.begin(),
                                               its->second.end());
            std::vector< std::string > buffer;
            for (unsigned i = 1; i < strs.size(); i++) {
                buffer.clear();
                its = mResources.find(strs[i]);
                std::set_intersection(result.begin(), result.end(),
                                      its->second.begin(), its->second.end(),
                                      std::back_inserter(buffer));
                if (buffer.empty()) {
                    return false;
                }
                swap(result, buffer);
            }
            return true;
        }
    }

    /**
     * @brief get the resource satisfying the combination of classes
     * @param type a combination of classes separated by '&',
     * for example "farmWorker & skilledWorker"
     */
    ResourceSolution getResources (const std::string& type) const
    {
        ResourceSolution resources;

        size_t n = std::count(type.begin(), type.end(), '&');
        if ( n == 0 ) {
            Resources::const_iterator pit;

            pit = mResources.find(type);

            if (pit == mResources.end()) {
                return {};
            }

            resources = std::vector< std::string > (pit->second.begin(),
                                                    pit->second.end());

        } else {
            std::vector<std::string> strs;
            boost::split(strs, type , boost::is_any_of("&"));
            Resources::const_iterator its = mResources.find(strs[0]);
            std::vector< std::string > result(its->second.begin(),
                                              its->second.end());
            std::vector< std::string > buffer;
            for (unsigned i = 1; i < strs.size(); i++) {
                buffer.clear();
                its = mResources.find(strs[i]);
                std::set_intersection(result.begin(), result.end(),
                                      its->second.begin(), its->second.end(),
                                      std::back_inserter(buffer));
                if (buffer.empty()) {
                    return resources;
                }
                swap(result, buffer);
            }
            resources = result;
        }
        return resources;
    }

    /**
     * @brief get the the number of resources
     * satisfying the combination of classes
     * @param type a combination of classes separated by '&',
     * for example "farmWorker & skilledWorker"
     */
    int getResourceQuantity (const std::string& type) const
    {
        return getResources(type).size();
    }

    void addRes(const std::string& name, const ResFct& res)
    { ress().add(name, res); }

    /**
     * @brief Add a fac into the facts tables.
     * @param name
     * @param fact
     */
    void addFact(const std::string& name, const Fact& fact)
    { facts().add(name, fact); }

    void addPortFact(const std::string& name, const PortFact& fact)
    { portfacts().add(name, fact); }

    std::string applyRes(const std::string& funcname,
                         const std::string& activityname,
                         const Activity& activity)
    {
        if  (ress().find(funcname) != ress().end()) {
            return ress()[funcname](activityname, activity);
        } else {
            return "";
        }
    }

    void applyFact(const std::string& name, const value::Value& value)
    {
        if  (facts().find(name) == facts().end()) {
            portfacts()[name](name, value);
        } else {
            facts()[name](value);
        }
    }

    Rule& addRule(const std::string& name)
    { return mPlan.rules().add(name); }

    Rule& addRule(const std::string& name, const Rule& rule)
    { return mPlan.rules().add(name, rule); }

    Rule& addPredicateToRule(const std::string& name, const Predicate& pred)
    { return mPlan.rules().add(name, pred); }

    Activity& addActivity(const std::string& name)
    { return mPlan.activities().add(name); }

    Activity& addActivity(const std::string& name,
                          const devs::Time& start,
                          const devs::Time& finish)
    { return mPlan.activities().add(name, start, finish); }

    Activity& addActivity(const std::string& name,
                          const Activity& act)
    { return mPlan.activities().add(name, act); }

    void removeActivity(const std::string& name)
    { mPlan.activities().remove(name); }


    /**
     * @brief The predecessor activity (i) must start before the successor
     * activity (j) can start.
     * @param acti The (i) activity in SiSj relationship.
     * @param actj The (j) activity in SiSj relationship.
     * @param maxtimelag time lag in SiSj relationship.
     */
    void addStartToStartConstraint(const std::string& acti,
                                   const std::string& actj,
                                   const devs::Time& maxtimelag)
    { mPlan.activities().addStartToStartConstraint(acti, actj, 0, maxtimelag); }

    /**
     * @brief The predecessor activity (i) must start before the successor
     * activity (j) can start.
     * @param acti The (i) activity in SiSj relationship.
     * @param actj The (j) activity in SiSj relationship.
     * @param mintimelag time lag in SiSj relationship.
     * @param maxtimelag time lag in SiSj relationship.
     */
    void addStartToStartConstraint(const std::string& acti,
                                   const std::string& actj,
                                   const devs::Time& mintimelag,
                                   const devs::Time& maxtimelag)
    { mPlan.activities().addStartToStartConstraint(acti, actj, mintimelag,
                                             maxtimelag); }

    /**
     * @brief The predecessor activity (i) must finish before the successor
     * activity (j) can start.
     * @param acti The (i) activity in FiSj relationship.
     * @param actj The (j) activity in FiSj relationship.
     * @param mintimelag minimal time lag in FiSj relationship.
     * @param maxtimelag maximal time lag in FiSj relationship.
     */
    void addFinishToStartConstraint(const std::string& acti,
                                    const std::string& actj,
                                    const devs::Time& mintimelag,
                                    const devs::Time& maxtimelag)
    { mPlan.activities().addFinishToStartConstraint(
            acti, actj, mintimelag, maxtimelag); }

    /**
     * @brief The predecessor activity (i) must finish before the successor
     * activity (j) can finish.
     * @param acti The (i) activity in FiFj relationship.
     * @param actj The (j) activity in FiFj relationship.
     * @param timelag time lag in FiFj relationship.
     */
    void addFinishToFinishConstraint(const std::string& acti,
                                     const std::string& actj,
                                     const devs::Time& maxtimelag)
    { mPlan.activities().addFinishToFinishConstraint(acti, actj, 0, maxtimelag); }

    /**
     * @brief The predecessor activity (i) must finish before the successor
     * activity (j) can finish.
     * @param acti The (i) activity in FiFj relationship.
     * @param actj The (j) activity in FiFj relationship.
     * @param mintimelag time lag in FiFj relationship.
     * @param maxtimelag time lag in FiFj relationship.
     */
    void addFinishToFinishConstraint(const std::string& acti,
                                     const std::string& actj,
                                     const devs::Time& mintimelag,
                                     const devs::Time& maxtimelag)
    { mPlan.activities().addFinishToFinishConstraint(
            acti, actj, mintimelag, maxtimelag); }

    const Rules& rules() const { return mPlan.rules(); }
    const Activities& activities() const { return mPlan.activities(); }

    /**
     * @brief Compute the next date when change in activity status.
     * @param time The current time.
     * @return A date in range ]devs::Time::negativeInfinity,
     * devs::Time::infinity[.
     */
    devs::Time nextDate(const devs::Time& time)
    { return mPlan.activities().nextDate(time); }

    devs::Time duration(const devs::Time& time);

    /**
     * @brief This function is call to change the states of the activities.
     * @return The next date on which the knowledge base must be change
     * (temporal constraints, temporal precedence constraints)
     * ]-devs::negativeInfinity, devs::infinity[ and a boolean if a change
     * has come.
     */
    Result processChanges(const devs::Time& time);

    /**
     * @brief Change the stage of the activity from STARTED to DONE.
     * @param name Name of the activity to done.
     * @param date Date when activity failed.
     * @throw utils::ArgError if activity is not in START state.
     */
    void setActivityDone(const std::string& name,
                         const devs::Time& date);
    /**
     * @brief Change the stage of the activity from * to FAILED. This function
     * does nothing if activity is already in FAILED state.
     * @param name Name of the activity to failed.
     * @param date Date when activity failed.
     * @throw utils::ArgError if activity is in FAILED state.
     */
    void setActivityFailed(const std::string& name,
                           const devs::Time& date);

    const Activities::result_t& waitedActivities() const
    { return mPlan.activities().waitedAct(); }

    const Activities::result_t& startedActivities() const
    { return mPlan.activities().startedAct(); }

    const Activities::result_t& failedActivities() const
    { return mPlan.activities().failedAct(); }

    const Activities::result_t& doneActivities() const
    { return mPlan.activities().doneAct(); }

    const Activities::result_t& endedActivities() const
    { return mPlan.activities().endedAct(); }

    const Activities::result_t& latestWaitedActivities() const
    { return mPlan.activities().latestWaitedAct(); }

    const Activities::result_t& latestStartedActivities() const
    { return mPlan.activities().latestStartedAct(); }

    const Activities::result_t& latestFailedActivities() const
    { return mPlan.activities().latestFailedAct(); }

    const Activities::result_t& latestDoneActivities() const
    { return mPlan.activities().latestDoneAct(); }

    const Activities::result_t& latestEndedActivities() const
    { return mPlan.activities().latestEndedAct(); }

    /**
     * @brief Return true if at least on list of activities (waited, started,
     * failed, done or ended lists) is not empty.
     * @return true or false.
     */
    bool haveActivityInLatestActivitiesLists() const
    {
        return not (mPlan.activities().latestWaitedAct().empty() and
                    mPlan.activities().latestStartedAct().empty() and
                    mPlan.activities().latestFailedAct().empty() and
                    mPlan.activities().latestDoneAct().empty() and
                    mPlan.activities().latestEndedAct().empty());
    }

    /**
     * @brief Clear the lists of activities (waited, started, failed, done or
     * ended lists).
     */
    void clearLatestActivitiesLists()
    { mPlan.activities().clearLatestActivitiesLists(); }

    const RessTable& ress() const
    { return mRessTable; }

    /**
     * @brief Get the table of available facts.
     * @return Table of available facts.
     */
    const FactsTable& facts() const
    { return mFactsTable; }

    const PortFactsTable& portfacts() const
    { return mPortFactsTable; }

    /**
     * @brief Get the table of available predicates.
     * @return Table of available predicates.
     */
    const PredicatesTable& predicates() const
    { return mPredicatesTable; }

    /**
     * @brief Get the table of available acknowledge functions.
     * @return Table of available acknowledge functions.
     */
    const AcknowledgeFunctions& acknowledgeFunctions() const
    { return mAckFunctions; }

    /**
     * @brief Get the table of available update functions.
     * @return Table of available update functions.
     */
    const UpdateFunctions& updateFunctions() const
    { return mUpdateFunctions; }

    /**
     * @brief Get the table of available output functions;
     * @return Table of available output functions.
     */
    const OutputFunctions& outputFunctions() const
    { return mOutFunctions; }

    RessTable& ress()
    { return mRessTable; }

    /**
     * @brief Get the table of available facts.
     * @return Table of available facts.
     */
    FactsTable& facts()
    { return mFactsTable; }

    PortFactsTable& portfacts()
    { return mPortFactsTable; }

    /**
     * @brief Get the table of available predicates.
     * @return Table of available predicates.
     */
    PredicatesTable& predicates()
    { return mPredicatesTable; }

    /**
     * @brief Get the table of available acknowledge functions.
     * @return Table of available acknowledge functions.
     */
    AcknowledgeFunctions& acknowledgeFunctions()
    { return mAckFunctions; }

    /**
     * @brief Get the table of available output functions;
     * @return Table of available output functions.
     */
    OutputFunctions& outputFunctions()
    { return mOutFunctions; }

    /**
     * @brief Get the table of available update functions.
     * @return Table of available update functions.
     */
    UpdateFunctions& updateFunctions()
    { return mUpdateFunctions; }

    template < typename X >
        AddRess < X > addRess(X obj)
        {
            return AddRess < X >(obj);
        }

    template < typename X >
        r < X > R(const std::string& name, X func)
        {
            return r < X >(name, func);
        }

    template < typename X >
        AddFacts < X > addFacts(X obj)
        {
            return AddFacts < X >(obj);
        }

    template < typename X >
        AddPortFacts < X > addPortFacts(X obj)
        {
            return AddPortFacts < X >(obj);
        }

    template < typename X >
        f < X > F(const std::string& name, X func)
        {
            return f < X >(name, func);
        }

    template < typename X >
        pf < X > PF(const std::string& name, X func)
        {
            return pf < X >(name, func);
        }

    template < typename X >
        AddPredicates < X > addPredicates(X obj)
        {
            return AddPredicates < X >(obj);
        }

    template < typename X >
        p < X > P(const std::string& name, X func)
        {
            return p < X >(name, func);
        }

    template < typename X >
        AddOutputFunctions < X > addOutputFunctions(X obj)
        {
            return AddOutputFunctions < X >(obj);
        }

    template < typename X >
        o < X > O(const std::string& name, X func)
        {
            return o < X >(name, func);
        }

    template < typename X >
        AddAcknowledgeFunctions < X > addAcknowledgeFunctions(X obj)
        {
            return AddAcknowledgeFunctions < X >(obj);
        }

    template < typename X >
        a < X > A(const std::string& name, X func)
        {
            return a < X >(name, func);
        }

    template < typename X >
        AddUpdateFunctions < X > addUpdateFunctions(X obj)
        {
            return AddUpdateFunctions < X >(obj);
        }

    template < typename X >
        u < X > U(const std::string& name, X func)
        {
            return u < X >(name, func);
        }

    //
    // Manage the plan library.
    //

    /**
     * @brief Get a constant reference to the plan Library.
     * @return A constant reference.
     */
    const Library& library() const { return mLibrary; }

    /**
     * @brief Get a reference to the plan Library.
     * @return A reference.
     */
    Library& library() { return mLibrary; }

    /**
     * @brief Get a constant reference to the current plan.
     * @return A constant reference.
     */
    const Plan& plan() const { return mPlan; }

    /**
     * @brief Get a reference to the current plan.
     * @return A reference.
     */
    Plan& plan() { return mPlan; }

    /**
     * @brief Merge the specified Plan `name' from the Library with the current
     * KnowledgeBase.
     *
     * @param name The name of the Plan to merge.
     */
    //void instantiatePlan(const std::string& name); FIXME

private:
    Plan mPlan; /**< The current plan to interpret */

    Library mLibrary; /**< The plan library. It stocks all plans available for
                        this decision knowledge base. */
    bool mResourcesCheck;

    Resources mResources;
    RessTable mRessTable;
    FactsTable mFactsTable;
    PortFactsTable mPortFactsTable;
    PredicatesTable mPredicatesTable;
    AcknowledgeFunctions mAckFunctions;
    OutputFunctions mOutFunctions;
    UpdateFunctions mUpdateFunctions;

    static void unionLists(Activities::result_t& last,
                           Activities::result_t& recent);
};

template < typename X, typename F >
AddRess < X > operator+=(AddRess < X > add, r < F > pred)
{
    add.kb->ress().add(pred.name, boost::bind(pred.func, add.kb, _1, _2));
    return add;
}

template < typename X, typename F >
AddFacts < X > operator+=(AddFacts < X > add, f < F > pred)
{
    add.kb->facts().add(pred.name, std::bind(pred.func, add.kb,
            std::placeholders::_1));
    return add;
}

template < typename X, typename PF >
AddPortFacts < X > operator+=(AddPortFacts < X > add, f < PF > pred)
{
    add.kb->portfacts().add(pred.name, std::bind(pred.func, add.kb,
                        std::placeholders::_1, std::placeholders::_2));
    return add;
}

template < typename X, typename F >
AddRess < X > operator,(AddRess < X > add, f < F > pred)
{
    add.kb->ress().add(pred.name, boost::bind(pred.func, add.kb, _1, _2));
    return add;
}

template < typename X, typename F >
AddFacts < X > operator,(AddFacts < X > add, f < F > pred)
{
    add.kb->facts().add(pred.name, std::bind(pred.func, add.kb,
            std::placeholders::_1));
    return add;
}

template < typename X, typename PF >
AddPortFacts < X > operator,(AddPortFacts < X > add, f < PF > pred)
{
    add.kb->portfacts().add(pred.name, std::bind(pred.func, add.kb,
                        std::placeholders::_1, std::placeholders::_2));
    return add;
}

template < typename X, typename F >
AddPredicates < X > operator+=(AddPredicates < X > add, p < F > pred)
{
    add.kb->predicates().add(pred.name,
            std::bind(pred.func, add.kb, std::placeholders::_1,
                      std::placeholders::_2, std::placeholders::_3));
    return add;
}

template < typename X, typename F >
AddPredicates < X > operator,(AddPredicates < X > add, p < F > pred)
{
    add.kb->predicates().add(pred.name,
            std::bind(pred.func, add.kb,std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3));
    return add;
}

template < typename X, typename F >
AddAcknowledgeFunctions < X > operator+=(AddAcknowledgeFunctions < X > add,
                                         a < F > pred)
{
    add.kb->acknowledgeFunctions().add(pred.name,
            std::bind(pred.func, add.kb,
                    std::placeholders::_1, std::placeholders::_2));
    return add;
}

template < typename X, typename F >
AddAcknowledgeFunctions < X > operator,(AddAcknowledgeFunctions < X > add,
                                        a < F > pred)
{
    add.kb->acknowledgeFunctions().add(pred.name,
            std::bind(pred.func, add.kb,
                    std::placeholders::_1, std::placeholders::_2));
    return add;
}

template < typename X, typename F >
AddOutputFunctions < X > operator+=(AddOutputFunctions < X > add,
                                    o < F > pred)
{
    add.kb->outputFunctions().add(pred.name,
            std::bind(pred.func, add.kb, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3));
    return add;
}

template < typename X, typename F >
AddOutputFunctions < X > operator,(AddOutputFunctions < X > add,
                                   o < F > pred)
{
    add.kb->outputFunctions().add(pred.name,
            std::bind(pred.func, add.kb, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3));
    return add;
}

template < typename X, typename F >
AddUpdateFunctions < X > operator+=(AddUpdateFunctions < X > add,
                                    u < F > pred)
{
    add.kb->updateFunctions().add(pred.name,
            std::bind(pred.func, add.kb,
                    std::placeholders::_1, std::placeholders::_2));
    return add;
}

template < typename X, typename F >
AddUpdateFunctions < X > operator,(AddUpdateFunctions < X > add,
                                   u < F > pred)
{
    add.kb->updateFunctions().add(pred.name,
            std::bind(pred.func, add.kb,
                    std::placeholders::_1, std::placeholders::_2));
    return add;
}

inline std::ostream& operator<<(std::ostream& o, const KnowledgeBase& kb)
{
    return o << kb.facts() << kb.portfacts() << "\n" << kb.rules() << "\n"
        << kb.activities() << "\n";
}

struct CompareActivities
{
    inline bool operator()(Activities::iterator x, Activities::iterator y) const
    {
        return &x->second < &y->second;
    }
};

}}} // namespace vle model decision

#endif
