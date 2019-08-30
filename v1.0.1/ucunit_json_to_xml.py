'''
 uCUnit - A unit testing framework for microcontrollers

 (C) 2018 Balazs Grill - IncQuery Labs Ltd.
     2019 Rainer Poisel - logi.cals GmbH

 File        : ucunit_json_toxml.py
 Description : Python script to convert UCunit JSON output to JUnit-compatible XML reports
 Author      : Balazs Grill
 Contact     : www.ucunit.org

  This file is part of ucUnit.

  You can redistribute and/or modify it under the terms of the
  Common Public License as published by IBM Corporation; either
  version 1.0 of the License, or (at your option) any later version.

  uCUnit is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  Common Public License for more details.

  You should have received a copy of the Common Public License
  along with uCUnit.

  It may also be available at the following URL:
        http://www.opensource.org/licenses/cpl1.0.txt

  If you cannot obtain a copy of the License, please contact the
  author.
'''
import sys
import json
import subprocess
from xml.dom import getDOMImplementation


def checkToString(check):
    return check["file"] + ":" + check["line"] + " " + check["msg"] + \
        "(" + check["args"] + ") " + check["result"] + "\n"


def setProperty(doc, props, name, value):
    p = doc.createElement("property")
    p.setAttribute("name", name)
    p.setAttribute("value", value)
    props.appendChild(p)


def main():
    try:
        processOutput = subprocess.check_output(sys.argv[2:])
        retVal = 0
    except (OSError):
        sys.exit(1)
    except subprocess.CalledProcessError as exc:
        processOutput = exc.output
        retVal = exc.returncode

    data = json.loads(processOutput)
    impl = getDOMImplementation()

    doc = impl.createDocument(None, "testsuite", None)
    root = doc.documentElement

    totaltests = int(data["passed"]) + int(data["failed"])
    root.setAttribute("name", data["name"])
    root.setAttribute("tests", str(totaltests))
    root.setAttribute("errors", "0")
    root.setAttribute("failures", data["failed"])
    props = doc.createElement("properties")
    setProperty(doc, props, "compiled", data["compiled"])
    setProperty(doc, props, "time", data["time"])
    setProperty(doc, props, "ucunit-version", data["version"])
    root.appendChild(props)

    for tc in data['testcases']:
        if 'testcasename' in tc:
            tcelement = doc.createElement('testcase')
            tcelement.setAttribute("name", tc['testcasename'])
            failures = ""
            stdout = ""
            for check in tc["checks"]:
                if "result" in check:
                    if check["result"] == "passed":
                        stdout += checkToString(check)
                    else:
                        failures += checkToString(check)
            root.appendChild(tcelement)
            sysout = doc.createElement('system-out')
            sysout.appendChild(doc.createCDATASection(stdout))
            tcelement.appendChild(sysout)
            if tc["result"] != "passed":
                f = doc.createElement("failure")
                f.appendChild(doc.createTextNode(failures))
                tcelement.appendChild(f)

    with open(sys.argv[1], mode='w') as fh:
        fh.write(doc.toxml('utf-8').decode('utf-8'))

    sys.exit(retVal)


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: " + sys.argv[0] + " <output-file> <executable> [args]")
        sys.exit(1)
    main()
