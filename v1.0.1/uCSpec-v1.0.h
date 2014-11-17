/*****************************************************************************
 *                                                                           *
 *  UcSpec - Support for behavior-driven development (specification by       *
 *           example) for uCUnit                                             *
 *                                                                           *
 *  (C) 2014  Arnd R. Strube                                                 *
 *            https://www.ucunit.org                                         *
 *                                                                           *
 *  File        : uCSpec-v1.0.h                                              *
 *  Description : Macros to support BDD for Unit-Testing                     *
 *  Author      : Arnd R. Strube                                             *
 *  Contact     : www.ucunit.org                                             *
 *                                                                           *
 *****************************************************************************/

/*
 * This file is part of ucUnit.
 *
 * You can redistribute and/or modify it under the terms of the
 * Common Public License as published by IBM Corporation; either
 * version 1.0 of the License, or (at your option) any later version.
 *
 * uCUnit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Common Public License for more details.
 *
 * You should have received a copy of the Common Public License
 * along with uCUnit.
 *
 * It may also be available at the following URL:
 *       http://www.opensource.org/licenses/cpl1.0.txt
 *
 * If you cannot obtain a copy of the License, please contact the
 * author.
 */

#ifndef UCSPEC_0101_H_
#define UCSPEC_0101_H_

/*****************************************************************************/
/* Variables                                                                 */
/*****************************************************************************/

static int ucspec_testcases_todo = 0; /* Number of specs not yet implemented */
static int ucunit_line = 0;           /* Used to detect empty specifications */

/*****************************************************************************/
/* Support behavior-driven development (specification by example             */
/*****************************************************************************/

/**
 * @Macro:         DESCRIBE(caption)
 *
 * @Description:   Macro to mark the beginning of a set of specifications.
 *
 * @Param caption: The object to be described as string.
 *
 * @Remarks:       Does not use the UCUNIT prefix, because - well that would
 *                 kind of spoil it.
 *
 */
#define DESCRIBE(caption)                                           \
    {                                                               \
        UCUNIT_WriteString("Describe: " caption "\n");              \

/**
 * @Macro:       DESCRIBE_END
 *
 * @Description: Marks the end of a set of specifications.
 *
 * @Remarks:     Does not use the UCUNIT prefix, because - well that would
 *               kind of spoil it.
 *
 */
#define DESCRIBE_END                                                \
        UCUNIT_WriteString("\n");                                   \
    }

/**
 * @Macro:         IT(caption)
 *
 * @Description:   Macro to mark the beginning of a specification in
 *                 behavior-driven development (BDD).
 *
 * @Param caption: The behavior that the object should exhibit as string.
 *
 * @Remarks:       Wraps UCUNIT_TestcaseBegin(name)
 *
 */
#define IT(caption)                                                 \
    {                                                               \
        ucunit_line = __LINE__;                                     \
        UCUNIT_WriteString("   " caption);                          \
        UCUNIT_WriteString(" -- ");                                 \
        ucunit_testcases_failed_checks = ucunit_checks_failed;

/**
 * @Macro:       IT_END
 *
 * @Description: Macro to mark the end of a specification.
 *               If no code is specified for the specification, then
 *               it is automatically treated as NOT IMPLEMENTED.
 *
 * @Remarks:     wraps UCUNIT_TestcaseBegin(name)
 *
 */
#define IT_END                                                      \
        if(ucunit_line + 1 >= __LINE__) {                           \
            UCUNIT_WriteString(" N O T   I M P L E M E N T E D\n"); \
            ucspec_testcases_todo++;                                \
        }                                                           \
        else if( 0==(ucunit_testcases_failed_checks - ucunit_checks_failed) ) \
        {                                                           \
            UCUNIT_WriteString("OK\n");                             \
            ucunit_testcases_passed++;                              \
        }                                                           \
        else                                                        \
        {                                                           \
            UCUNIT_WriteString("\n");                               \
            ucunit_testcases_failed++;                              \
        }                                                           \
    }

/*****************************************************************************/
/* Support for "SHOULD"-style assertions                                     */
/*****************************************************************************/

#define SHOULD(condition, msg, args) UCUNIT_Check(condition, msg, args)
#define SHOULD_EQ(expected, actual) UCUNIT_CheckIsEqual(expected, actual)
#define SHOULD_BE_NULL(pointer) UCUNIT_CheckIsNull(pointer)
#define SHOULD_NOT_BE_NULL(pointer) UCUNIT_CheckIsNotNull(pointer)
#define SHOULD_BE_IN_RANGE() UCUNIT_CheckIsInRange(value, lower, upper)
#define SHOULD_BE_8BIT(value, lower, upper) UCUNIT_CheckIs8Bit(value)
#define SHOULD_BE_16BIT(value) UCUNIT_CheckIs16Bit(value)
#define SHOULD_BE_32BIT(value) UCUNIT_CheckIs32Bit(value)
#define SHOULD_BE_BITSET(value) UCUNIT_CheckIsBitSet(value, bitno)
#define SHOULD_BE_BITCLEAR(value, bitno) UCUNIT_CheckIsBitClear(value, bitno)

/*****************************************************************************/
/* Eliminate -Werror for unused static variables                             */
/*****************************************************************************/
/**
 * @Macro:       UCSPEC_UseStaticsAtLeastOnce()
 *
 * @Description: Silences "defined but not used"-Warnings.
 *
 * @Remarks:     This is really a hack - any better ideas?
 *
 */
#define UCSPEC_UseStaticsAtLeastOnce()                                \
    (void)ucunit_checks_failed;                                       \
    (void)ucunit_checks_passed;                                       \
    (void)ucunit_testcases_failed;                                    \
    (void)ucunit_testcases_passed;                                    \
    (void)ucunit_testcases_failed_checks;                             \
    (void)ucunit_checklist_failed_checks;                             \
    (void)ucunit_action;                                              \
    (void)ucunit_checkpoints;                                         \
    (void)ucunit_index;

/*****************************************************************************/
/* uCSpec Replacement Macros                                                 */
/*****************************************************************************/

/**
 * @Macro:       uCSpec replacement for UCUNIT_WriteFailedMsg(msg, args)
 *
 * @Description: Writes a message that check has failed.
 *
 * @Param msg:   Message to write. This is the name of the called
 *               Check, without the substring UCUNIT_Check.
 * @Param args:  Argument list as string.
 *
 * @Remarks:     This has been slightly adjusted to fit the specification
 *               output format.
 *
 */
#undef  UCUNIT_WriteFailedMsg
#ifdef  UCUNIT_MODE_SILENT
#define UCUNIT_WriteFailedMsg(msg, args)                            \
    UCUNIT_WriteString(" F A I L E D");
#else
#define UCUNIT_WriteFailedMsg(msg, args)                            \
    UCUNIT_WriteString(" F A I L E D :\n");                         \
    UCUNIT_WriteString("        " __FILE__);                        \
    UCUNIT_WriteString(":");                                        \
    UCUNIT_WriteString(UCUNIT_DefineToString(__LINE__));            \
    UCUNIT_WriteString(": failed:");                                \
    UCUNIT_WriteString(msg);                                        \
    UCUNIT_WriteString("(");                                        \
    UCUNIT_WriteString(args);                                       \
    UCUNIT_WriteString(")");
#endif

/**
 * @Macro:       UCSPEC_WriteSummary()
 *
 * @Description: Writes the the number of specs not iplemented,
 *               then calls UCUNIT_WriteSummary to write the
 *               remainder of the test suite summary.
 *
 * @Remarks:     This macro uses UCUNIT_WriteString(msg) and
 *               UCUNIT_WriteInt(n) to write the summary.
 *
 */
#define UCSPEC_WriteSummary()                                       \
{                                                                   \
    UCUNIT_WriteString("\n**************************************"); \
    UCUNIT_WriteString("\nSpecs not implemented: ");                \
    UCUNIT_WriteInt(ucspec_testcases_todo);                         \
    UCUNIT_WriteSummary();                                          \
}

#endif /*UCSPEC_H_*/
