/*
 * attention/AttentionModule.cc
 *
 * Copyright (C) 2008 by OpenCog Foundation
 * All Rights Reserved
 *
 * Written by Misgana Bayetta && Roman Treutlein
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "AttentionModule.h"
#include "AttentionParamQuery.h"

#include <opencog/attentionbank/bank/AttentionBank.h>
#include <opencog/attentionbank/types/atom_types.h>
#include <opencog/cogserver/server/CogServer.h>
#include <opencog/cogserver/modules/agents/AgentsModule.h>
#include <opencog/util/Config.h>

using namespace opencog;
using namespace std::placeholders;

concurrent_queue<Handle> AttentionModule::newAtomsInAV;

DECLARE_MODULE(AttentionModule)

AttentionModule::AttentionModule(CogServer& cs) :
    Module(cs)
{
    init();
    do_start_ecan_register();
    do_stop_ecan_register();
    do_list_ecan_param_register();
    do_set_ecan_param_register();
}

AttentionModule::~AttentionModule()
{
    logger().debug("[AttentionModule] enter destructor");

    _scheduler->unregisterAgent(AFImportanceDiffusionAgent::info().id);
    _scheduler->unregisterAgent(WAImportanceDiffusionAgent::info().id);

    _scheduler->unregisterAgent(ForgettingAgent::info().id);
    _scheduler->unregisterAgent(HebbianUpdatingAgent::info().id);
    _scheduler->unregisterAgent(HebbianCreationAgent::info().id);

    do_start_ecan_unregister();
    do_stop_ecan_unregister();
    do_list_ecan_param_unregister();
    do_set_ecan_param_unregister();

    attentionbank(&_cogserver.getAtomSpace()).AddAFSignal().disconnect(addAFConnection);

    logger().debug("[AttentionModule] exit destructor");
}

void AttentionModule::init()
{
    std::string save = config().get("MODULES");
    config().set("MODULES", "libagents.so");
    _cogserver.loadModules();
    config().set("MODULES", save);
    Module* amod = _cogserver.getModule("opencog::AgentsModule");
    AgentsModule* agmod = (AgentsModule*) amod;
    _scheduler = &agmod->get_scheduler();

    AttentionParamQuery _atq(&_cogserver.getAtomSpace());

    // Set params
    int af_size = std::stoi(_atq.get_param_value(AttentionParamQuery::af_max_size));
    attentionbank(&_cogserver.getAtomSpace()).set_af_size(af_size);
    
    // New Thread based ECAN agents.
    _scheduler->registerAgent(AFImportanceDiffusionAgent::info().id, &afImportanceFactory);
    _scheduler->registerAgent(WAImportanceDiffusionAgent::info().id, &waImportanceFactory);

    _scheduler->registerAgent(AFRentCollectionAgent::info().id, &afRentFactory);
    _scheduler->registerAgent(WARentCollectionAgent::info().id, &waRentFactory);

    _scheduler->registerAgent(ForgettingAgent::info().id,          &forgettingFactory);
    _scheduler->registerAgent(HebbianCreationAgent::info().id,&hebbianCreationFactory);
    _scheduler->registerAgent(HebbianUpdatingAgent::info().id,&hebbianUpdatingFactory);

    _forgetting_agentptr =
        _scheduler->createAgent(ForgettingAgent::info().id, false);
    _hebbiancreation_agentptr =
        _scheduler->createAgent(HebbianCreationAgent::info().id,false);
    _hebbianupdating_agentptr =
        _scheduler->createAgent(HebbianUpdatingAgent::info().id,false);

    _afImportanceAgentPtr = _scheduler->createAgent(AFImportanceDiffusionAgent::info().id,false);
    _waImportanceAgentPtr = _scheduler->createAgent(WAImportanceDiffusionAgent::info().id,false);

    _afRentAgentPtr = _scheduler->createAgent(AFRentCollectionAgent::info().id, false);
    _waRentAgentPtr = _scheduler->createAgent(WARentCollectionAgent::info().id, false);


    addAFConnection = attentionbank(&_cogserver.getAtomSpace()).AddAFSignal().connect(
            std::bind(&AttentionModule::addAFSignalHandler,
                this, _1, _2, _3));
}

std::string AttentionModule::do_start_ecan(Request *req, std::list<std::string> args)
{
    std::string afImportance = AFImportanceDiffusionAgent::info().id;
    std::string waImportance = WAImportanceDiffusionAgent::info().id;

    std::string afRent = AFRentCollectionAgent::info().id;
    std::string waRent = WARentCollectionAgent::info().id;

    _scheduler->startAgent(_afImportanceAgentPtr, true, afImportance);
    _scheduler->startAgent(_waImportanceAgentPtr, true, waImportance);

    _scheduler->startAgent(_afRentAgentPtr, true, afRent);
    _scheduler->startAgent(_waRentAgentPtr, true, waRent);

   // _scheduler->startAgent(_forgetting_agentptr,true,"attention");

   // _scheduler->startAgent(_hebbiancreation_agentptr,true,"hca");
   // _scheduler->startAgent(_hebbianupdating_agentptr,true,"hua");

    return ("Started the following agents:\n" + afImportance + "\n" + waImportance +
           "\n" + afRent + "\n" + waRent + "\n");
}

std::string AttentionModule::do_stop_ecan(Request *req, std::list<std::string> args)
{
    _scheduler->stopAgent(_afImportanceAgentPtr);
    _scheduler->stopAgent(_waImportanceAgentPtr);

    _scheduler->stopAgent(_afRentAgentPtr);
    _scheduler->stopAgent(_waRentAgentPtr);

    _scheduler->stopAgent(_forgetting_agentptr);

    _scheduler->stopAgent(_hebbiancreation_agentptr);
    _scheduler->stopAgent(_hebbianupdating_agentptr);

    return "Stopped ECAN agents.\n";
}

std::string AttentionModule::do_list_ecan_param(Request *req, std::list<std::string> args)
{
    std::string response = "";
    AttentionParamQuery _atq(&_cogserver.getAtomSpace());
    HandleSeq hseq = _atq.get_params();
    for(const Handle& h : hseq){
        std::string param = h->get_name();
        response += param + "= " + _atq.get_param_value(param) + "\n"; 
    }
    return response;
}

std::string AttentionModule::do_set_ecan_param(Request *req, std::list<std::string> args)
{
    AttentionParamQuery _atq(&_cogserver.getAtomSpace());
    auto it = args.begin();
    std::string param = *it;
    std::advance(it,1);
    _atq.set_param(param, *it);

   return param+"= "+_atq.get_param_value(param)+"\n";
}


/*
 * When an atom enters the AttentionalFocus, it is added to a concurrent_queue
 * so that the HebbianCreationAgent know to check it and create HebbianLinks if
 * neccesary
 */
void AttentionModule::addAFSignalHandler(const Handle& source,
                                        const AttentionValuePtr& av_old,
                                        const AttentionValuePtr& av_new)
{
    newAtomsInAV.push(source);
}
