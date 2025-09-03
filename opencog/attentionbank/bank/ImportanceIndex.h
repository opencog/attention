/*
 * opencog/attentionbank/ImportanceIndex.h
 *
 * Copyright (C) 2008-2011 OpenCog Foundation
 * Copyright (C) 2008 Joel Pitt <joel@fruitionnz.com>
 * Copyright (C) 2017 Linas Vepstas <linasvepstas@gmail.com>
 * All Rights Reserved
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

#ifndef _OPENCOG_IMPORTANCEINDEX_H
#define _OPENCOG_IMPORTANCEINDEX_H

#include <mutex>

#include <opencog/attentionbank/avalue/AttentionValue.h>
#include <opencog/attentionbank/bank/AtomBins.h>
#include <opencog/attentionbank/bank/AVUtils.h>

namespace opencog
{
/** \addtogroup grp_atomspace
 *  @{
 */

//! recent_val is a value that can update which also
//! keeps a exponential decaying record of it's recent value
template<class ValueType> struct recent_val {
    //! The current value
    ValueType val;
    //! The recent record of the value
    float recent;
    //! The decay rate of the recent value
    float decay;

    //! constructor with initial value
    recent_val(ValueType x): val(x), recent((float)x), decay(0.5f) {}
    //! constructor with 0 as initial value
    recent_val(): val(0), recent(0.0f), decay(0.5f) {}

    //! update the current value and the decaying record
    /**
     * \param x the new value
     */
    inline void update(ValueType x) {
        val = x;
        recent = ((decay) * val) + ((1.0f - decay) * recent);
    }
};

/**
 * Implements an index with additional routines needed for managing
 * short-term importance.  This index is thread-safe.
 */
using HandleSTIPair = std::pair<Handle, AttentionValue::sti_t>;

namespace ecan {
    class StochasticDiffusionAmountCalculator;
};
class ImportanceIndex
{
    // Needs to access importanceBin
    friend class ecan::StochasticDiffusionAmountCalculator;
private:
    mutable std::mutex _mtx;

    AtomBins _index;

    /// Running average min and max STI.
    opencog::recent_val<AttentionValue::sti_t> _maxSTI;
    opencog::recent_val<AttentionValue::sti_t> _minSTI;

    /**
     * This method returns which importance bin an atom with the given
     * STI should be placed.
     *
     * @param Importance value to be mapped.
     * @return The importance bin which an atom of the given importance
     * should be placed.
     */
    static size_t importanceBin(AttentionValue::sti_t);

public:
    ImportanceIndex();
    void removeAtom(const Handle&);

    void update(void);

    /**
     * Get the maximum STI observed.
     *
     * @param average If true, return an exponentially decaying
     *        average of maximum STI, otherwise return the actual
     *        maximum.
     * @return Maximum STI
     */
    AttentionValue::sti_t getMaxSTI(bool average=true) const;

    /**
     * Get the minimum STI observed.
     *
     * @param average If true, return an exponentially decaying
     *        average of minimum STI, otherwise return the actual
     *        minimum.
     * @return Minimum STI
     */
    AttentionValue::sti_t getMinSTI(bool average=true) const;

    /**
     * Updates the importance index for the given atom.
     */
    void updateImportance(const Handle&,
                          const AttentionValuePtr& oldav,
                          const AttentionValuePtr& newav);

    /**
     * Returns the set of atoms within the given importance range.
     *
     * @param Importance range lower bound (inclusive).
     * @param Importance range upper bound (inclusive).
     * @return The set of atoms within the given importance range.
     */
    UnorderedHandleSet getHandleSet(AttentionValue::sti_t lowerBound,
                                    AttentionValue::sti_t upperBound =
                                         AttentionValue::MAXSTI) const;

    // Get the content of an ImportanceBin at index i.
    template <typename OutputIterator> OutputIterator
    getContent(size_t i,OutputIterator out) const
    {
        return _index.getContent(i,out);
    }

    Handle getRandomAtom(void) const;

    /**
     * Get the highest bin which contains Atoms
     */
    UnorderedHandleSet getMaxBinContents();

    /**
     * Get the lowest bin which contains Atoms
     */
    UnorderedHandleSet getMinBinContents();

    size_t bin_size(void) const;

    /**
     * Get the size of the bin at the given index.
     */
    size_t size(int) const;
};

/** @}*/
} //namespace opencog

#endif // _OPENCOG_IMPORTANCEINDEX_H
