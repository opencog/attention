
#include <opencog/atoms/base/Link.h>
#include <opencog/atoms/core/StateLink.h>
#include <opencog/guile/SchemeEval.h>

#include "AttentionParamQuery.h"

using namespace opencog;

// Attentional Focus Params
const std::string AttentionParamQuery::af_size = "AF_SIZE";
const std::string AttentionParamQuery::af_decay = "AFB_DECAY";
const std::string AttentionParamQuery::af_bottom = "AFB_BOTTOM";
const std::string AttentionParamQuery::af_min_size = "MIN_AF_SIZE";
const std::string AttentionParamQuery::af_max_size = "MAX_AF_SIZE";
const std::string AttentionParamQuery::af_rent_update_freq = "AF_RENT_FREQUENCY";

// Forgetting Params
const std::string AttentionParamQuery::forg_forgetting_threshold = "ECAN_FORGET_THRESHOLD";

// Hebbian Link Params
const std::string AttentionParamQuery::heb_maxlink = "MAX_LINKS";
const std::string AttentionParamQuery::heb_max_alloc_percentage = "HEBBIAN_MAX_ALLOCATION_PERCENTAGE";
const std::string AttentionParamQuery::heb_local_farlink_ratio = "LOCAL_FAR_LINK_RATIO";

// Diffusion/Spreading Params
const std::string AttentionParamQuery::dif_spread_percentage = "MAX_SPREAD_PERCENTAGE";
const std::string AttentionParamQuery::dif_spread_hebonly = "SPREAD_HEBBIAN_ONLY";
const std::string AttentionParamQuery::dif_tournament_size = "DIFFUSION_TOURNAMENT_SIZE";
const std::string AttentionParamQuery::spreading_filter = "SPREADING_FILTER";

// Rent Params
const std::string AttentionParamQuery::rent_starting_sti_rent = "STARTING_ATOM_STI_RENT";
const std::string AttentionParamQuery::rent_starting_lti_rent = "STARTING_ATOM_LTI_RENT";
const std::string AttentionParamQuery::rent_target_sti_funds = "TARGET_STI_FUNDS";
const std::string AttentionParamQuery::rent_sti_funds_buffer = "STI_FUNDS_BUFFER";
const std::string AttentionParamQuery::rent_target_lti_funds = "TARGET_LTI_FUNDS";
const std::string AttentionParamQuery::rent_lti_funds_buffer = "LTI_FUNDS_BUFFER";
const std::string AttentionParamQuery::rent_tournament_size = "RENT_TOURNAMENT_SIZE";


/**
 * The representation of parameters in the atomspace
 * will be as follows:
 *
 * MemberLink
 *   Concept "$PARAMETER_i"
 *   Concept "ECAN_PARAMS"
 *
 * then each paramter's value will be stored in a
 * state link.
 *
 * StateLink
 *   Concept "$PARAMETER_i"
 *   Number   "x"  ; when boolean, x would be 0 or 1
 */
AttentionParamQuery::AttentionParamQuery(AtomSpace* as): _as(as)
{
    load_default_values();

    parent_param = _as->add_node(CONCEPT_NODE, "ECAN_PARAMS");

    Handle var = _as->add_node(VARIABLE_NODE, "__ECAN_PARAM__");
    Handle member = _as->add_link(MEMBER_LINK,
            HandleSeq {var, parent_param});
    hget_params = _as->add_link(BIND_LINK, HandleSeq{member, var});
}

std::string AttentionParamQuery::get_param_value(std::string param)
{
    Handle hparam = _as->get_node(CONCEPT_NODE, std::move(param));
    if (nullptr == hparam)
        throw RuntimeException(TRACE_INFO,
             "There is no parameter %s", param.c_str());

    Handle hvalue = StateLink::get_state(hparam);
    if (nullptr == hvalue)
        throw RuntimeException(TRACE_INFO,
             "Parameter %s has no value.", param.c_str());

    if (not hvalue->is_node())
        throw RuntimeException(TRACE_INFO,
            "Parameter %s must be a Node.", param.c_str());

    return hvalue->get_name();
}

Handle AttentionParamQuery::get_param_hvalue(std::string param)
{
    Handle hparam = _as->get_node(CONCEPT_NODE, std::move(param));
    if (nullptr == hparam)
        throw RuntimeException(TRACE_INFO,
             "There is no parameter %s", param.c_str());

    return StateLink::get_state(hparam);
}

HandleSeq AttentionParamQuery::get_params(void)
{
    Handle rh = HandleCast(hget_params->execute(_as));
    if (rh) rh = _as->add_atom(rh);

    return rh->getOutgoingSet();
}

void AttentionParamQuery::load_default_values(void)
{
     SchemeEval scm(_as);
     scm.eval("(load-from-path \"opencog/attention/default-param-values.scm\")");
}
