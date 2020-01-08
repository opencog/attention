/*
 * Neighbors.cc
 *
 * Copyright (C) 2014 OpenCog Foundation
 *
 * Author: William Ma <https://github.com/williampma>
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

#include <iostream>

#include <opencog/atoms/base/Link.h>
#include <opencog/atoms/base/Node.h>
#include "Neighbors.h"

namespace opencog
{

HandleSeq get_target_neighbors(const Handle& h, Type desiredLinkType,
                               bool match_subtype/* = false*/)
{
    if (nameserver().isA(desiredLinkType, UNORDERED_LINK))
        return HandleSeq();

    HandleSeq answer;
    for (const Handle& link : h->getIncomingSet())
    {
        Type t = link->get_type();
        if (not(t == desiredLinkType or
               (match_subtype and nameserver().isA(t, desiredLinkType))))
	    continue;
        if (link->getOutgoingAtom(0) != h) continue;

        for (const Handle& handle : link->getOutgoingSet()) {
           if (handle == h) continue;
           answer.emplace_back(handle);
        }
    }
    return answer;
}


HandleSeq get_source_neighbors(const Handle& h, Type desiredLinkType,
                               bool match_subtype/* = false*/)
{
    if (nameserver().isA(desiredLinkType, UNORDERED_LINK))
        return HandleSeq();

    HandleSeq answer;

    for (const Handle& link : h->getIncomingSet())
    {
        Type t = link->get_type();
        if (not(t == desiredLinkType or
               (match_subtype and nameserver().isA(t, desiredLinkType))))
	    continue;
        if (link->getOutgoingAtom(0) == h) continue;

        for (const Handle& handle : link->getOutgoingSet()) {
           if (handle == h) continue;
            answer.emplace_back(handle);
        }
    }
    return answer;
}

} // namespace OpenCog
