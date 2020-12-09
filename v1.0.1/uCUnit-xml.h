#ifndef UCUNIT_XML_H_
#define UCUNIT_XML_H_

#include <time.h>
#include <stdbool.h>
#include <stdio.h>

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
        UCUNIT_XML_CheckExecuted(true, msg, args, __FILE__, UCUNIT_DefineToString(__LINE__));      \
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
        UCUNIT_XML_CheckExecuted(false, msg, args, __FILE__, UCUNIT_DefineToString(__LINE__));     \
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
        }                                                               \
        else                                                            \
        {                                                               \
            UCUNIT_XML_TestcaseEnd(false);                              \
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
#define UCUNIT_WriteSummary()
#endif

/* TODO: Calculate the real value of this after the implementation is done. */
#define MAX_NUM_OF_TEST_CASES            20
#define MAX_NUM_OF_CHECKS_PER_TESTCASE   40

/* ----- Structures -------------------------------------------------------- */

/**
 * Contains information and result data of a uCUnit test check
 */
typedef struct UCUNIT_XmlChecks
{
    char* type;      			/* Pointer to the type of the check */
    bool isPassed;              /* Result of the check */
    char* filePath;             /* Pointer to the checked test file path string */
    char* lineNumber;           /* Pointer to the checked test file line string */
    char* arguments;            /* Pointer to the check arguments string */
} UCUNIT_XmlCheck;

/**
 * Contains information and result data of a uCUnit test case
 */
typedef struct UCUNIT_XmlTestCases
{
  char* testCaseName;           /* Pointer to the test case name string */
  bool isPassed;                /* Result of the test case */
  unsigned int numOfChecks;     /* Number of the used test checks */
  UCUNIT_XmlCheck checks[MAX_NUM_OF_CHECKS_PER_TESTCASE];   /* Array of the test check objects */
} UCUNIT_XmlTestCase;

/**
 * Contains information of a uCUnit test suite
 */
typedef struct UCUNIT_XmlTestSuites
{
  unsigned int xml_buffer_size; /* Size of the XML string's output array*/
  char* testSuiteName;          /* Pointer to the test suite name string */
  struct tm time;				/* The time of the test suite start */
  char* ucunitVersion;        	/* Pointer to the uCUnit version string */
  unsigned int numOfTestCases;  /* Number of the used test cases */
  UCUNIT_XmlTestCase testCases[MAX_NUM_OF_TEST_CASES];  /* Array of the test case objects */
} UCUNIT_XmlTestSuite;

/* ----- Prototypes -------------------------------------------------------- */

/**
 * Begins the test suite. Stores the test suite data into the static UCUNIT_XmlTestSuite structure.
 * The following data are stored:
 *     * the test suite name (testSuiteName parameter)
 *     * the start time of the test suite
 *     * the uCUnit version
 * The function also set the number of test cases to 0.
 *
 * @param [in] testSuiteName Pointer to the test suite name.
 */
void UCUNIT_XML_TestBegin(char* testSuiteName);

/**
 * Begins the next test case. Stores the test case data into the UCUNIT_XmlTestCase structure.
 * The next UCUNIT_XmlTestCase object is get from the static UCUNIT_XmlTestSuite
 * and the number of test cases is increased.
 * TODO: What should happen when there are no more available test case in the static object?
 * The following data are stored:
 *     * the test case name (testCaseName parameter)
 * The function also set the number of test checks to 0.
 *
 * @param [in] testSuiteName Pointer to the test case name.
 */
void UCUNIT_XML_TestcaseBegin(char* testCaseName);

/**
 * Ends the actual test case. Stores the result into the UCUNIT_XmlTestCase structure.
 * The actual UCUNIT_XmlTestCase object is get from the static UCUNIT_XmlTestSuite.
 *
 * @param [in] isPassed Result of the test case.
 */
void UCUNIT_XML_TestcaseEnd(bool isPassed);

/**
 * Adds a test check to the actual test case. Stores the check data into the UCUNIT_XmlCheck structure.
 * The next UCUNIT_XmlCheck object is get from the actual UCUNIT_XmlTestCase
 * and the number of test checks is increased.
 * The following data are stored:
 *     * the check result (isPassed parameter)
 *     * the check type (maps the type parameter from string to UCUNIT_CheckType)
 *     * the check arguments (arguments parameter)
 *     * the checked file path string
 *     * the checked file line string
 *
 * @param [in] isPassed Result of the test case.
 * @param [in] type Pointer to the check's type string.
 * @param [in] arguments Pointer to the check's arguments.
 * @param [in] file The file where the check was executed.
 * @param [in] line The line number of the check where it was executed.
 */
void UCUNIT_XML_CheckExecuted(bool isPassed, char* type, char* arguments, char* file, char* line);

/**
 * TODO: Create description
 */
void UCUNIT_XML_TestSummary(int testCasesFailed, int testCasesPassed, int checksFailed, int checksPassed);

/**
 * Returns with the static UCUNIT_XmlTestSuite object.
 *
 * @param [out] testSuite Pointer to the output test suite object.
 */
void UCUNIT_XML_GetTestsuite(UCUNIT_XmlTestSuite* testSuite);

/**
 * Create the header meta-data part of the an XML file.
 * The output will be to following:
 *     <?xml version="1.0" encoding="utf-8"?>
 *
 * @param [out] xmlString Pointer to the output string array.
 */
void UCUNIT_XML_GetXmlHeader(char* xmlString);

/**
 * Calculates the test suite results and converts them into an XML string.
 * The output will have the following structure:
 *     <testsuite errors="[number of test case errors]"
 *                failures="[number of test check failures]"
 *                name="[test suite name]"
 *                tests="[number of test cases]">
 *
 * @param [out] xmlString Pointer to the output string array.
 */
void UCUNIT_XML_GetTestsuiteBegin(char* xmlString);

/**
 * Converts the test suite meta-data into an XML string.
 * The output will have the following structure:
 *     <properties>
 *         <property name="compiled" value="[date in 'Month(short) Day Year' format]"/>
 *         <property name="time" value="[start time of the test suite]"/>
 *         <property name="ucunit-version" value="[version of the uCUnit]"/>
 *     </properties>
 *
 * @param [out] xmlString Pointer to the output string array.
 */
void UCUNIT_XML_GetProperties(char* xmlString);

/**
 * Converts the test cases into an XML string.
 * The output will have the following structure:
 *     <testcase name="[test case name]">
 *         <system-out>
 *             <![CDATA[
 *                 [file path]:[line] [check1 type]([check1 arguments]) [check1 result]
 *                 [file path]:[line] [check2 type]([check2 arguments]) [check2 result]
 *             ]]>
 *         </system-out>
 *         (if there's failed a check in the testcase)
 *         <failure>
 *             [file path]:[line] [check1 type]([check1 arguments]) [check1 result]
 *             [file path]:[line] [check2 type]([check2 arguments]) [check2 result]
 *         </failure>
 *     </testcase>
 *
 * @param [out] xmlString Pointer to the output string array.
 */
void UCUNIT_XML_GetTestcases(char* xmlString);

/**
 * Create the test suite close tag into an XML string.
 * The output will be to following:
 *     </testsuite>
 *
 * @param [out] xmlString Pointer to the output string array.
 */
void UCUNIT_XML_GetTestsuiteClose(char *xmlString);

/**
 * Calls all the methods that creates the XML string.
 * The output will be the complete XML string.
 *
 * @param [out] xmlString Pointer to the output string array.
 */
void UCUNIT_XML_GetXmlObject(char *xmlString);

/**
 * Converts the test checks into an XML string.
 * The output will have the following structure:
 *     [file path]:[line] [check1 type]([check1 arguments]) [check1 result]
 *     [file path]:[line] [check2 type]([check2 arguments]) [check2 result]
 *
 * @param [out] xmlString Pointer to the output string array.
 */
void UCUNIT_XML_GetChecks(char *xmlString, int i, int j,const char *result);

unsigned int getSizeOfSystemOut();
unsigned int getSizeOfFailures();
unsigned int getSizeOfCheck(int i, int j, const char *result);
unsigned int getSizeOfTestsuite();

#endif /* UCUNIT_XML_H_ */
