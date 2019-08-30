/*****************************************************************************
 *                                                                           *
 *  uCUnit - A unit testing framework for microcontrollers                   *
 *                                                                           *
 *  (C) 2007 - 2008 Sven Stefan Krauss                                       *
 *                  https://www.ucunit.org                                   *
 *  (C) 2018 Balazs Grill - IncQuery Labs Ltd.                               *
 *                                                                           *
 *  File        : uCUnit-hr.h                                                *
 *  Description : Human-readable output                                      *
 *  Author      : Sven Stefan Krauss, Balazs Grill                           *
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
#ifndef UCUNIT_HR_H_
#define UCUNIT_HR_H_

#ifdef UCUNIT_MODE_VERBOSE
/**
 * @Macro:       UCUNIT_TestBegin(name)
 *
 * @Description: Writes a message that identifies the test
 *
 * @Param name:  Identifier of the test suite.
 *
 *
 */
#define UCUNIT_TestBegin(name)                                     \
    do                                                             \
    {                                                              \
    UCUNIT_WriteString("\n**************************************");\
    UCUNIT_WriteString("\nName:     ");                            \
    UCUNIT_WriteString(name);                                      \
    UCUNIT_WriteString("\nCompiled: ");                            \
    UCUNIT_WriteString(__DATE__);                                  \
    UCUNIT_WriteString("\nTime:     ");                            \
    UCUNIT_WriteString(__TIME__);                                  \
    UCUNIT_WriteString("\nVersion:  ");                            \
    UCUNIT_WriteString(UCUNIT_VERSION);                            \
    UCUNIT_WriteString("\n**************************************");\
    } while(0)
#else
#define UCUNIT_TestBegin(name)
#endif

#ifdef UCUNIT_MODE_VERBOSE
/**
 * @Macro:       UCUNIT_WritePassedMsg(msg, args)
 *
 * @Description: Writes a message that check has passed.
 *
 * @Param msg:   Message to write. This is the name of the called
 *               Check, without the substring UCUNIT_Check.
 * @Param args:  Argument list as string.
 *
 * @Remarks:     This macro is used by UCUNIT_Check(). A message will
 *               only be written if verbose mode is set
 *               to UCUNIT_MODE_VERBOSE.
 *
 */
#define UCUNIT_WritePassedMsg(msg, args)                        \
    do                                                          \
    {                                                           \
        UCUNIT_WriteString(__FILE__);                           \
        UCUNIT_WriteString(":");                                \
        UCUNIT_WriteString(UCUNIT_DefineToString(__LINE__));    \
        UCUNIT_WriteString(": passed:");                        \
        UCUNIT_WriteString(msg);                                \
        UCUNIT_WriteString("(");                                \
        UCUNIT_WriteString(args);                               \
        UCUNIT_WriteString(")\n");                              \
    } while(0)
#else
#define UCUNIT_WritePassedMsg(msg, args)
#endif

#ifdef UCUNIT_MODE_SILENT
#define UCUNIT_WriteFailedMsg(msg, args)
#else

/**
 * @Macro:       UCUNIT_WriteFailedMsg(msg, args)
 *
 * @Description: Writes a message that check has failed.
 *
 * @Param msg:   Message to write. This is the name of the called
 *               Check, without the substring UCUNIT_Check.
 * @Param args:  Argument list as string.
 *
 * @Remarks:     This macro is used by UCUNIT_Check(). A message will
 *               only be written if verbose mode is set
 *               to UCUNIT_MODE_NORMAL and UCUNIT_MODE_VERBOSE.
 *
 */
#define UCUNIT_WriteFailedMsg(msg, args)                        \
    do                                                          \
    {                                                           \
        UCUNIT_WriteString(__FILE__);                           \
        UCUNIT_WriteString(":");                                \
        UCUNIT_WriteString(UCUNIT_DefineToString(__LINE__));    \
        UCUNIT_WriteString(": failed:");                        \
        UCUNIT_WriteString(msg);                                \
        UCUNIT_WriteString("(");                                \
        UCUNIT_WriteString(args);                               \
        UCUNIT_WriteString(")\n");                              \
    } while(0)
#endif

/*****************************************************************************/
/* Testcases */
/*****************************************************************************/

/**
 * @Macro:       UCUNIT_TestcaseBegin(name)
 *
 * @Description: Marks the beginning of a test case and resets
 *               the test case statistic.
 *
 * @Param name:  Name of the test case.
 *
 * @Remarks:     This macro uses UCUNIT_WriteString(msg) to print the name.
 *
 */
#define UCUNIT_TestcaseBegin(name)                                        \
    do                                                                    \
    {                                                                     \
        UCUNIT_WriteString("\n======================================\n"); \
        UCUNIT_WriteString(name);                                         \
        UCUNIT_WriteString("\n======================================\n"); \
        ucunit_testcases_failed_checks = ucunit_checks_failed;            \
    }                                                                     \
    while(0)

/**
 * @Macro:       UCUNIT_TestcaseEnd()
 *
 * @Description: Marks the end of a test case and calculates
 *               the test case statistics.
 *
 * @Remarks:     This macro uses UCUNIT_WriteString(msg) to print the result.
 *
 */
#define UCUNIT_TestcaseEnd()                                         \
    do                                                               \
    {                                                                \
        UCUNIT_WriteString("======================================\n");  \
        if( 0==(ucunit_testcases_failed_checks - ucunit_checks_failed) ) \
        {                                                            \
            UCUNIT_WriteString("Testcase passed.\n");                \
            ucunit_testcases_passed++;                               \
        }                                                            \
        else                                                         \
        {                                                            \
            UCUNIT_WriteFailedMsg("EndTestcase","");                 \
            ucunit_testcases_failed++;                               \
        }                                                            \
        UCUNIT_WriteString("======================================\n"); \
    }                                                                \
    while(0)
/*****************************************************************************/
/* Testsuite Summary                                                         */
/*****************************************************************************/

/**
 * @Macro:       UCUNIT_WriteSummary()
 *
 * @Description: Writes the test suite summary.
 *
 * @Remarks:     This macro uses UCUNIT_WriteString(msg) and
 *               UCUNIT_WriteInt(n) to write the summary.
 *
 */
#define UCUNIT_WriteSummary()                                         \
{                                                                     \
    UCUNIT_WriteString("\n**************************************");   \
    UCUNIT_WriteString("\nTestcases: failed: ");                      \
    UCUNIT_WriteInt(ucunit_testcases_failed);                         \
    UCUNIT_WriteString("\n           passed: ");                      \
    UCUNIT_WriteInt(ucunit_testcases_passed);                         \
    UCUNIT_WriteString("\nChecks:    failed: ");                      \
    UCUNIT_WriteInt(ucunit_checks_failed);                            \
    UCUNIT_WriteString("\n           passed: ");                      \
    UCUNIT_WriteInt(ucunit_checks_passed);                            \
    UCUNIT_WriteString("\n**************************************\n"); \
}

#endif /* UCUNIT_HR_H_ */
