#ifndef UCUNIT_JSON_H_
#define UCUNIT_JSON_H_


/**
 * @Macro:       UCUNIT_TestBegin(name)
 *
 * @Description: Writes a message that identifies the test
 *
 * @Param name:  Identifier of the test suite.
 *
 *
 */
#define UCUNIT_TestBegin(name)                        \
    do                                                          \
    {                                                           \
        UCUNIT_WriteString("{\"name\":\"");                           \
        UCUNIT_WriteString(name);\
        UCUNIT_WriteString("\", \"compiled\":\"");                                \
        UCUNIT_WriteString(__DATE__);    \
        UCUNIT_WriteString("\", \"time\":\"");                                \
        UCUNIT_WriteString(__TIME__);    \
        UCUNIT_WriteString("\", \"version\":\"");     \
        UCUNIT_WriteString(UCUNIT_VERSION);        \
        UCUNIT_WriteString("\", \"testcases\":[");     \
    } while(0)


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
        UCUNIT_WriteString("{\"file\":\"");                                \
        UCUNIT_WriteString(__FILE__);                           \
        UCUNIT_WriteString("\", \"line\":\"");                                \
        UCUNIT_WriteString(UCUNIT_DefineToString(__LINE__));    \
        UCUNIT_WriteString("\", \"result\":\"passed\",  \"msg\":\"");                        \
        UCUNIT_WriteString(msg);                                \
        UCUNIT_WriteString("\", \"args\":");                                \
        UCUNIT_WriteString(UCUNIT_DefineToString(args));                               \
        UCUNIT_WriteString("},\n");                              \
    } while(0)

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
      UCUNIT_WriteString("{\"file\":\"");                                \
      UCUNIT_WriteString(__FILE__);                           \
      UCUNIT_WriteString("\", \"line\":\"");                                \
      UCUNIT_WriteString(UCUNIT_DefineToString(__LINE__));    \
      UCUNIT_WriteString("\", \"result\":\"failed\",  \"msg\":\"");                        \
      UCUNIT_WriteString(msg);                                \
      UCUNIT_WriteString("\", \"args\":\"");                                \
      UCUNIT_WriteString(UCUNIT_DefineToString(args));                               \
      UCUNIT_WriteString("\"},\n");                              \
    } while(0)

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
        UCUNIT_WriteString("{ \"testcasename\":\""); \
        UCUNIT_WriteString(name);                                         \
        UCUNIT_WriteString("\", \"checks\":[\n"); \
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
        UCUNIT_WriteString("{}]");              \
        if( 0==(ucunit_testcases_failed_checks - ucunit_checks_failed) ) \
        {                                                            \
            UCUNIT_WriteString(", \"result\":\"passed\"\n");          \
            ucunit_testcases_passed++;                               \
        }                                                            \
        else                                                         \
        {                                                            \
            UCUNIT_WriteString(", \"result\":\"failed\"\n");          \
            ucunit_testcases_failed++;                               \
        }                                                            \
        UCUNIT_WriteString("},\n"); \
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
    UCUNIT_WriteString("{}]\n");\
    UCUNIT_WriteString(",\"failed\":\"");                      \
    UCUNIT_WriteInt(ucunit_testcases_failed);                         \
    UCUNIT_WriteString("\", \"passed\":\"");                      \
    UCUNIT_WriteInt(ucunit_testcases_passed);                         \
    UCUNIT_WriteString("\", \"checksFailed\":\"");                      \
    UCUNIT_WriteInt(ucunit_checks_failed);                            \
    UCUNIT_WriteString("\", \"checksPassed\":\"");                      \
    UCUNIT_WriteInt(ucunit_checks_passed);                            \
    UCUNIT_WriteString("\"}"); \
}

#endif /* UCUNIT_JSON_H_ */
