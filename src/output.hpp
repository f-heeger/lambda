// ==========================================================================
//                                  lambda
// ==========================================================================
// Copyright (c) 2013-2015, Hannes Hauswedell, FU Berlin
// All rights reserved.
//
// This file is part of Lambda.
//
// Lambda is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Lambda is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Lambda.  If not, see <http://www.gnu.org/licenses/>.
// ==========================================================================
// Author: Hannes Hauswedell <hannes.hauswedell @ fu-berlin.de>
// ==========================================================================
// output.hpp: contains routines for file-writing
// ==========================================================================

#ifndef SEQAN_LAMBDA_OUTPUT_H_
#define SEQAN_LAMBDA_OUTPUT_H_

#include <seqan/blast.h>
#include <seqan/bam_io.h>

using namespace seqan;

//TODO redo this, use own ids
// like blastmatchfield, but adapted for SAM/BAM
template <typename TVoidSpec = void>
class BlastMatchFieldSam
{
    static constexpr const std::array<char const *, 45> tagLabels
    {
      {
        "std",            //  STD,
        "qseqid",         //  Q_SEQ_ID,
        "qgi",            //  Q_GI,
        "qacc",           //  Q_ACC,
        "qaccver",        //  Q_ACCVER,
        "qlen",           //  Q_LEN,
        "sseqid",         //  S_SEQ_ID,
        "sallseqid",      //  S_ALL_SEQ_ID,
        "sgi",            //  S_GI,
        "sallgi",         //  S_ALL_GI,
        "sacc",           //  S_ACC,
        "saccver",        //  S_ACCVER,
        "sallacc",        //  S_ALLACC,
        "slen",           //  S_LEN,
        "ZS",             //  Q_START,
        "qend",           //  Q_END,
        "YS",             //  S_START,
        "send",           //  S_END,
        "qseq",           //  Q_SEQ,
        "sseq",           //  S_SEQ,
        "ZE",             //  E_VALUE,
        "AS",             //  BIT_SCORE,
        "ZR",             //  SCORE,
        "length",         //  LENGTH,
        "ZI",             //  P_IDENT,
        "nident",         //  N_IDENT,
        "mismatch",       //  MISMATCH,
        "positive",       //  POSITIVE,
        "gapopen",        //  GAP_OPEN,
        "gaps",           //  GAPS,
        "ZP",             //  P_POS,
        "frames",         //  FRAMES,
        "ZF",             //  Q_FRAME,
        "YF",             //  S_FRAME,
        "btop",           //  BTOP,
        "staxids",        //  S_TAX_IDS,
        "sscinames",      //  S_SCI_NAMES,
        "scomnames",      //  S_COM_NAMES,
        "sblastnames",    //  S_BLAST_NAMES,
        "sskingdoms",     //  S_S_KINGDOMS,
        "stitle",         //  S_TITLE,
        "salltitles",     //  S_ALL_TITLES,
        "sstrand",        //  S_STRAND,
        "qcovs",          //  Q_COV_S,
        "qcovhsp"         //  Q_COV_HSP
      }
    };
    
    static constexpr const std::array<bool, 45> implemented
    {
      {
        true,             //  STD,
        true,             //  Q_SEQ_ID,
        false,            //  Q_GI,
        false,            //  Q_ACC,
        false,            //  Q_ACCVER,
        true,             //  Q_LEN,
        true,             //  S_SEQ_ID,
        false,            //  S_ALL_SEQ_ID,
        false,            //  S_GI,
        false,            //  S_ALL_GI,
        false,            //  S_ACC,
        false,            //  S_ACCVER,
        false,            //  S_ALLACC,
        true,             //  S_LEN,
        true,             //  Q_START,
        true,             //  Q_END,
        true,             //  S_START,
        true,             //  S_END,
        false,            //  Q_SEQ,
        false,            //  S_SEQ,
        true,             //  E_VALUE,
        true,             //  BIT_SCORE,
        true,             //  SCORE,
        true,             //  LENGTH,
        true,             //  P_IDENT,
        true,             //  N_IDENT,
        true,             //  MISMATCH,
        true,             //  POSITIVE,
        true,             //  GAP_OPEN,
        true,             //  GAPS,
        true,             //  P_POS,
        true,             //  FRAMES,
        true,             //  Q_FRAME,
        true,             //  S_FRAME,
        false,            //  BTOP,
        false,            //  S_TAX_IDS,
        false,            //  S_SCI_NAMES,
        false,            //  S_COM_NAMES,
        false,            //  S_BLAST_NAMES,
        false,            //  S_S_KINGDOMS,
        false,            //  S_TITLE,
        false,            //  S_ALL_TITLES,
        false,            //  S_STRAND,
        false,            //  Q_COV_S,
        false             //  Q_COV_HSP
      }
    };
    
    
    
};


// ----------------------------------------------------------------------------
// Function _untranslatedClipPositions()
// ----------------------------------------------------------------------------

// similar to _untranslatePositions() from the blast module
template <typename TSequence1, typename TSequence2, typename TBlastMatch>
inline void
_untranslateSequence(TSequence1                     & target,
                     TSequence2               const & source,
                     TBlastMatch              const & m)
{
    if (m.qFrameShift >= 0)
    {
        target = infix(source,
                       3 * m.qStart + std::abs(m.qFrameShift) - 1,
                       3 * m.qEnd + std::abs(m.qFrameShift) - 1);
    }
    else
    {
        static thread_local Dna5String buf;
        buf = source;
        reverseComplement(buf);
        target = infix(buf,
                       3 * m.qStart + std::abs(m.qFrameShift) - 1,
                       3 * m.qEnd + std::abs(m.qFrameShift) - 1);
    }
}

// ----------------------------------------------------------------------------
// Function _untranslatedClipPositions()
// ----------------------------------------------------------------------------

// similar to _untranslatePositions() from the blast module
template <BlastProgram p>
inline void
_untranslatedClipPositions(unsigned & leftClip,         // should start with qStart
                           unsigned & rightClip,        // should start with qEnd
                           int const frameShift,
                           unsigned const realLength,   // untranslated Length if translated
                           BlastProgramSelector<p> const & selector)
{
    if (qIsTranslated(selector))
    {
        leftClip  = leftClip * 3 + std::abs(frameShift) - 1;
        rightClip = rightClip * 3 + std::abs(frameShift) - 1;
    }

    rightClip = realLength - rightClip;

    if (frameShift < 0)
        std::swap(leftClip, rightClip);
}

// ----------------------------------------------------------------------------
// Function blastMatchToCigar() convert seqan align to cigar
// ----------------------------------------------------------------------------

//TODO this could be done nicer, I guess
template <typename TCigar, typename TBlastMatch, typename TLocalHolder>
inline void
blastMatchToDnaCigar(TCigar & cigar, TBlastMatch const & m, unsigned const untransMatchLength, TLocalHolder const & lH)
{
    using TCElem = typename Value<TCigar>::Type;

    SEQAN_ASSERT_EQ(length(m.alignRow0), length(m.alignRow1));

    // hard clipping
    unsigned leftClip   = m.qStart;
    unsigned rightClip  = m.qEnd;
    _untranslatedClipPositions(leftClip,
                               rightClip,
                               m.qFrameShift,
                               untransMatchLength,
                               context(lH.gH.outfile).blastProgram);
    if (leftClip > 0)
        appendValue(cigar, TCElem('H', leftClip));

    for (unsigned i = 0, count = 0; i < length(m.alignRow0); /* incremented below */)
    {
        // deletion in query
        count = 0;
        while (isGap(m.alignRow0, i) && (i < length(m.alignRow0)))
        {
            ++count;
            ++i;
        }
        if (count > 0)
            appendValue(cigar, TCElem('D', (qIsTranslated(lH.gH.blastProgram) ? count * 3 : count)));

        // insertion in query
        count = 0;
        while (isGap(m.alignRow1, i) && (i < length(m.alignRow0)))
        {
            ++count;
            ++i;
        }
        if (count > 0)
            appendValue(cigar, TCElem('I', (qIsTranslated(lH.gH.blastProgram) ? count * 3 : count)));

        // match or mismatch
        count = 0;
        while ((!isGap(m.alignRow0, i)) && (!isGap(m.alignRow1, i)) && (i < length(m.alignRow0)))
        {
            ++count;
            ++i;
        }
        if (count > 0)
            appendValue(cigar, TCElem('M', (qIsTranslated(lH.gH.blastProgram) ? count * 3 : count)));
    }

    if (rightClip > 0)
        appendValue(cigar, TCElem('H', rightClip));
}

// ----------------------------------------------------------------------------
// Function myWriteHeader()
// ----------------------------------------------------------------------------

template <typename TGH>
inline void
myWriteHeader(TGH & globalHolder, LambdaOptions const & options)
{
    if (options.outFileFormat == 0) // BLAST
    {
        open(globalHolder.outfile, toCString(options.output));
        context(globalHolder.outfile).fields = options.columns;
        writeHeader(globalHolder.outfile);
    } else // SAM or BAM
    {
        open(globalHolder.outfileBam, toCString(options.output));
        auto & context          = seqan::context(globalHolder.outfileBam);
        auto & subjSeqLengths   = contigLengths(context);
        auto & subjIds          = contigNames(context);

        // set sequence lengths
        if (sIsTranslated(globalHolder.blastProgram))
        {
            //TODO can we get around a copy?
            subjSeqLengths = globalHolder.untransSubjSeqLengths;
        } else
        {
            // compute lengths ultra-fast
            resize(subjSeqLengths, length(globalHolder.subjSeqs));
            SEQAN_OMP_PRAGMA(parallel for simd)
            for (unsigned i = 0; i < length(subjSeqLengths); ++i)
                subjSeqLengths[i] = globalHolder.subjSeqs.limits[i+1] - globalHolder.subjSeqs.limits[i];
        }
        // set namestore
        resize(subjIds, length(globalHolder.subjIds));
        SEQAN_OMP_PRAGMA(parallel for)
        for (unsigned i = 0; i < length(globalHolder.subjIds); ++i)
            subjIds[i] = prefix(globalHolder.subjIds[i],
                                std::find(begin(globalHolder.subjIds[i], Standard()),
                                          end(globalHolder.subjIds[i], Standard()),
                                          ' ')
                                - begin(globalHolder.subjIds[i], Standard()));

        typedef BamHeaderRecord::TTag   TTag;

        // CREATE HEADER
        BamHeader header;
        // Fill first header line.
        BamHeaderRecord firstRecord;
        firstRecord.type = BAM_HEADER_FIRST;
        appendValue(firstRecord.tags, TTag("VN", "1.4"));
//         appendValue(firstRecord.tags, TTag("SO", "unsorted"));
        appendValue(firstRecord.tags, TTag("GO", "query"));
        appendValue(header, firstRecord);

        // Fill program header line.
        BamHeaderRecord pgRecord;
        pgRecord.type = BAM_HEADER_PROGRAM;
        appendValue(pgRecord.tags, TTag("ID", "lambda"));
        appendValue(pgRecord.tags, TTag("PN", "lambda"));
        appendValue(pgRecord.tags, TTag("VN", SEQAN_APP_VERSION));
        appendValue(pgRecord.tags, TTag("CL", options.commandLine));
        appendValue(header, pgRecord);

        // TODO add comment line describing extra fields and possibly link to homepage

        // sam and we don't want the headers
        if (!options.samWithRefHeader && (options.outFileFormat == 1))
        {
            // we only write the header records that we actually created ourselves
            for (unsigned i = 0; i < length(header); ++i)
                write(globalHolder.outfileBam.iter, header[i], seqan::context(globalHolder.outfileBam), Sam());
        }
        else
        {
            // ref header records are automatically added with default writeHeader()
            writeHeader(globalHolder.outfileBam, header);
        }
    }
}

// ----------------------------------------------------------------------------
// Function myWriteRecord()
// ----------------------------------------------------------------------------

template <typename TLH, typename TRecord>
inline void
myWriteRecord(TLH & lH, TRecord const & record)
{
    if (lH.options.outFileFormat == 0) // BLAST
    {
        SEQAN_OMP_PRAGMA(critical(filewrite))
        {
            writeRecord(lH.gH.outfile, record);
        }
    } else // SAM or BAM
    {
        // convert multi-match blast-record to multiple SAM/BAM-Records

        std::vector<BamAlignmentRecord> bamRecords;
        bamRecords.resize(record.matches.size());

        auto mIt = begin(record.matches, Standard());
        for (auto & bamR : bamRecords)
        {
            bamR.beginPos   = mIt->sStart;

            bamR.flag       = BAM_FLAG_SECONDARY; // all are secondary for now
            if (mIt->qFrameShift < 0)
                bamR.flag   |= BAM_FLAG_RC;
            // truncated query name
            bamR.qName      = prefix(mIt->qId,
                                     std::find(begin(mIt->qId, Standard()),
                                               end(mIt->qId, Standard()),
                                               ' ')
                                     - begin(mIt->qId, Standard()));
            // reference ID
            bamR.rID        = mIt->_n_sId;
            // compute cigar
            blastMatchToDnaCigar(bamR.cigar, *mIt, record.qLength, lH);
            // we want to include the seq
            if (lH.options.samBamSeq)
            {
                // only dna sequences supported
                if (lH.gH.blastProgram == BlastProgram::BLASTN)
                    bamR.seq = infix(source(mIt->alignRow0),
                                     beginPosition(mIt->alignRow0),
                                     endPosition(mIt->alignRow0));
                // untranslation is ok, too
                else if (qIsTranslated(lH.gH.blastProgram))
                    _untranslateSequence(bamR.seq,
                                         lH.gH.untranslatedQrySeqs[mIt->_n_qId],
                                        * mIt);
                // else no sequence is available
            }

            // custom tags? TODO
            
            for (auto const & tag : lH.options.samBamExtraTags)
            {
                switch (tag)
                {
        S_LEN,
        Q_START,
        Q_END,
        S_START,
        S_END,
        Q_SEQ,
        S_SEQ,
        E_VALUE,
        //             appendTagValue(bamR.tags, "ZE", mIt->eValue, 'f');
        BIT_SCORE,
        appendTagValue(bamR.tags, "AS", uint16_t(mIt->bitScore), 'S');
        SCORE,
        appendTagValue(bamR.tags, "ZR", uint8_t(mIt->alignStats.alignmentScore), 'C');
        LENGTH,
        P_IDENT,
        appendTagValue(bamR.tags, "ZI", uint8_t(mIt->alignStats.alignmentIdentity), 'C');
        N_IDENT,
        MISMATCH,
        POSITIVE,
        GAP_OPEN,
        GAPS,
        P_POS,
        FRAMES,
        Q_FRAME,
                    appendTagValue(bamR.tags, "ZF", int8_t(mIt->qFrameShift), 'c');
        S_FRAME,

                   

            appendTagValue(bamR.tags, "NM",
                           uint32_t(mIt->alignStats.alignmentLength - mIt->alignStats.numMatches),
                           'I');

            
            

            //TODO also add protein sequence and protein cigar if != BLASTN
            // goto next match
            ++mIt;
        }

        bamRecords.front().flag = 0; // remove BAM_FLAG_SECONDARY for first

        SEQAN_OMP_PRAGMA(critical(filewrite))
        {
            for (auto & r : bamRecords)
                writeRecord(lH.gH.outfileBam, r);
        }
    }
}

// ----------------------------------------------------------------------------
// Function myWriteFooter()
// ----------------------------------------------------------------------------

template <typename TGH>
inline void
myWriteFooter(TGH & globalHolder, LambdaOptions const & options)
{
    if (options.outFileFormat == 0) // BLAST
    {
        writeFooter(globalHolder.outfile);
    }
}

#endif // SEQAN_LAMBDA_OUTPUT_H_
