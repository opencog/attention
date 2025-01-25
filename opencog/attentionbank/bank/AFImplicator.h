/*
 * AFImplicator.h
 *
 * Copyright (C) 2009, 2014 Linas Vepstas
 *
 * Author: Linas Vepstas <linasvepstas@gmail.com>  January 2009
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

#ifndef _OPENCOG_AF_IMPLICATOR_H
#define _OPENCOG_AF_IMPLICATOR_H

#include <opencog/attentionbank/bank/AttentionalFocusCB.h>
#include <opencog/query/RewriteMixin.h>
#include <opencog/query/InitiateSearchMixin.h>
#include <opencog/query/SatisfyMixin.h>

namespace opencog {

/**
 * Attentional Focus specific PatternMatchCallback implementation
 */
class AFImplicator:
	public AttentionalFocusCB,
	public InitiateSearchMixin,
	public RewriteMixin,
	public SatisfyMixin
{
	public:
		AFImplicator(AtomSpace* asp, ContainerValuePtr cvp) :
			AttentionalFocusCB(asp),
			InitiateSearchMixin(asp),
			RewriteMixin(asp, cvp)
		{}

	virtual void set_pattern(const Variables& vars,
	                         const Pattern& pat)
	{
		InitiateSearchMixin::set_pattern(vars, pat);
		AttentionalFocusCB::set_pattern(vars, pat);
	}
};

Handle af_bindlink(AtomSpace*, const Handle&);

}; // namespace opencog

#endif // _OPENCOG_AF_IMPLICATOR_H
