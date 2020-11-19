#ifndef UCUNIT_XML_H_
#define UCUNIT_XML_H_

#include <time.h>
#include <stdbool.h>

#ifdef UCUNIT_MODE_XML
/**
 * @Macro:       UCUNIT_TestBegin(name)
 *
 * @Description: Writes a message that identifies the test
 *
 * @Param name:  Identifier of the test suite.
 *
 *
 */
#define UCUNIT_TestBegin(name)          \
    do                                  \
    {                                   \
      UCUNIT_XML_TestBegin(name);       \
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
#define UCUNIT_WritePassedMsg(msg, args)                \
    do                                                  \
    {                                                   \
        UCUNIT_XML_CheckExecuted(true, msg, args);      \
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
#define UCUNIT_WriteFailedMsg(msg, args)                \
    do                                                  \
    {                                                   \
        UCUNIT_XML_CheckExecuted(false, msg, args);     \
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
#define UCUNIT_TestcaseBegin(name)                              \
    do                                                          \
    {                                                           \
        UCUNIT_XML_TestcaseBegin(name);                         \
        ucunit_testcases_failed_checks = ucunit_checks_failed;  \
    } while(0)

/**
 * @Macro:       UCUNIT_TestcaseEnd()
 *
 * @Description: Marks the end of a test case and calculates
 *               the test case statistics.
 *
 * @Remarks:     This macro uses UCUNIT_WriteString(msg) to print the result.
 *
 */
#define UCUNIT_TestcaseEnd()                                            \
    do                                                                  \
    {                                                                   \
        if (0==(ucunit_testcases_failed_checks - ucunit_checks_failed)) \
        {                                                               \
            UCUNIT_XML_TestcaseEnd(true);                               \
            ucunit_testcases_passed++;                                  \
        }                                                               \
        else                                                            \
        {                                                               \
            UCUNIT_XML_TestcaseEnd(false);                              \
            ucunit_testcases_failed++;                                  \
        }                                                               \
    } while(0)

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
#define UCUNIT_WriteSummary()   \
{                               \
                                \
}
#endif

/* TODO: Calculate the real value of this after the implementation is done. */
#define MAX_NUM_OF_TEST_CASES            20
#define MAX_NUM_OF_CHECKS_PER_TESTCASE   20

/* ----- Structures -------------------------------------------------------- */

/**
 * TODO define all check type
 */
typedef enum UCUNIT_CheckTypes
{
    UCUNIT_CheckTypeIsEqual = 0
} UCUNIT_CheckType;

/**
 * TODO create description
 */
typedef struct UCUNIT_XmlChecks
{
    UCUNIT_CheckType type;
    bool isPassed;
    char* filePath;
    char* lineNumber;
    char* arguments;
} UCUNIT_XmlCheck;

/**
 * TODO create description
 */
typedef struct UCUNIT_XmlTestCases
{
  char* testCaseName;
  bool isPassed;
  unsigned int numOfChecks;
  UCUNIT_XmlCheck checks[MAX_NUM_OF_CHECKS_PER_TESTCASE];
} UCUNIT_XmlTestCase;

/**
 * TODO create description
 */
typedef struct UCUNIT_XmlTestSuites
{
  char* testSuiteName;
  struct tm time;
  char* ucunitVersion;
  unsigned int numOfTestCases;
  UCUNIT_XmlTestCase testCases[MAX_NUM_OF_TEST_CASES];
} UCUNIT_XmlTestSuite;

/* ----- Prototypes -------------------------------------------------------- */

/**
 * TODO create description
 */
void UCUNIT_XML_TestBegin(char* testSuitName);

/**
 * TODO create description
 */
void UCUNIT_XML_TestcaseBegin(char* testCaseName);

/**
 * TODO create description
 */
void UCUNIT_XML_TestcaseEnd(bool isPassed);

/**
 * TODO create description
 */
void UCUNIT_XML_CheckExecuted(bool isPassed, char* type, char* arguments);

/**
 * TODO create description
 */
void UCUNIT_XML_TestSummary(int testCasesFailed, int testCasesPassed, int checksFailed, int checksPassed);

/**
 * TODO create description
 */
void UCUNIT_XML_GetTestsuite(UCUNIT_XmlTestSuite* test);

/**
 * TODO create description
 */
void UCUNIT_XML_GetHeaderString(char* xmlString);

/**
 * TODO create description
 */
void UCUNIT_XML_GetPropertiesString(char* xmlString);

/**
 * TODO create description
 */
void UCUNIT_XML_GetTestcasesString(char* xmlString);

#endif /* UCUNIT_XML_H_ */
