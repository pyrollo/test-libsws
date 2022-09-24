/*
Test Lib SWS - Unit tests for libsws
Copyright (C) 2022 Pierre-Yves Rollo <dev@pyrollo.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <iostream>
#include "swsengine.h"
#include "test.h"

void testModuleHierarchy()
{
    TEST("Module hierarchy",

        swsEngine sws;

        TESTNOEXCEPTION("Create basic module at root level",
            sws.newModule("myadd", "add");
        );

        TESTEXCEPTION("Cannot create module at same level with same name",
            sws::duplicate_name,
            sws.newModule("myadd", "add");
        );

        TESTEXCEPTION("Cannot create module as basic module child",
            sws::unknown_schema,
            sws.newModule("myadd/myadd2", "add");
        );

        TESTNOEXCEPTION("Create container module at root level",
            sws.newModule("container", "container");
        );

        TESTNOEXCEPTION("Create basic module in container",
            sws.newModule("container/myadd", "add");
        );

        TESTEXCEPTION("Cannont create basic module in non existing container",
            sws::unknown_schema,
            sws.newModule("containerZ/myadd", "add");
        );

        TESTNOEXCEPTION("Create container in container",
            sws.newModule("container/container", "container");
        );

        TESTNOEXCEPTION("Create basic module in container in container",
            sws.newModule("container/container/myadd", "add");
        );
    );
}

void testModuleDeletion()
{
    TEST("Module deletion",

        swsEngine sws;

        TESTNOEXCEPTION("Create fixtures",
            sws.newModule("myadd", "add");
            sws.newModule("myemptycontainer", "container");
            sws.newModule("myonelevelcontainer", "container");
            sws.newModule("myonelevelcontainer/myadd", "add");
            sws.newModule("myonelevelcontainer/myadd2", "add");
            sws.newModule("myonelevelcontainer/myadd3", "add");
            sws.newModule("mycontainer", "container");
            sws.newModule("mycontainer/mycontainer2", "container");
            sws.newModule("mycontainer/mycontainer2/myadd", "add");
            sws.newModule("mycontainer/mycontainer2/myadd2", "add");
            sws.newModule("mycontainer/mycontainer2/myadd3", "add");
        );

        TESTNOEXCEPTION("Delete an existing base module at root",
            sws.deleteModule("myadd");
        );

        TESTNOEXCEPTION("Delete an existing base module in container",
            sws.deleteModule("mycontainer/mycontainer2/myadd");
        );

        TESTEXCEPTION("Cannot delete non existing module at root",
            sws::unknown_module,
            sws.deleteModule("nonexistant");
        );

        TESTEXCEPTION("Cannot delete non existing module in container",
            sws::unknown_module,
            sws.deleteModule("mycontainer/mycontainer2/nonexistant");
        );

        TESTNOEXCEPTION("Delete an empty container",
            sws.deleteModule("myemptycontainer");
        );

        TESTNOEXCEPTION("Delete an one level container",
            sws.deleteModule("myonelevelcontainer");
        );

        TESTNOEXCEPTION("Delete an several level container",
            sws.deleteModule("mycontainer");
        );
    );
}

void testConnecting()
{
    TEST("Connecting",
        swsEngine sws;

        TESTNOEXCEPTION("Create fixtures",
            sws.newModule("add1", "add");
            sws.newModule("add2", "add");
            sws.newModule("add3", "add");
            sws.newModule("container", "container");
            sws.newModule("container/add", "add");
        );

        TESTEXCEPTION("Cannot connect input to input",
            sws::illegal_connection,
            sws.connect("add1#op1", "add2#op1");
        );

        TESTEXCEPTION("Cannot connect output to output",
            sws::illegal_connection,
            sws.connect("add1#result", "add2#result");
        );

        TESTEXCEPTION("Cannot connect between different schemas",
            sws::illegal_connection,
            sws.connect("add1#result", "container/add#op1");
        );

        TESTEXCEPTION("Cannot connect interconnected module to itself",
            sws::illegal_connection,
            sws.connect("add1#result", "add1#op1");
        );

        TESTEXCEPTION("Cannot connect interconnected module to itself (reverse)",
            sws::illegal_connection,
            sws.connect("add1#op1", "add1#result");
        );

        TESTNOEXCEPTION("Can connect input to output",
            sws.connect("add1#op1", "add2#result");
        );

        TESTNOEXCEPTION("Can connect output to input",
            sws.connect("add3#result", "add2#op1");
        );

        TESTEXCEPTION("Cannot create circular connection",
            sws::illegal_connection,
            sws.connect("add3#op1", "add1#result");
        );

        TESTEXCEPTION("Cannot create circular connection (reverse)",
            sws::illegal_connection,
            sws.connect("add1#result", "add3#op1");
        );

        TESTEXCEPTION("Cannot connect same input twice",
            sws::already_connected,
            sws.connect("add1#op1", "add3#result");
        );

        TESTNOEXCEPTION("Can connect same output twice",
            sws.connect("add1#op2", "add2#result");
        );

        TESTNOEXCEPTION("Can disconnect",
            sws.disconnect("add1#op2", "add2#result");
            sws.disconnect("add2#op1", "add3#result");
        );

        TESTNOEXCEPTION("Can reconnect",
            sws.connect("add1#op2", "add2#result");
            sws.connect("add2#op1", "add3#result");
        );

        TESTNOEXCEPTION("Can disconnect (reverse)",
            sws.disconnect("add2#result", "add1#op2");
            sws.disconnect("add3#result", "add2#op1");
        );

        TESTNOEXCEPTION("Can reconnect (reverse)",
            sws.connect("add2#result", "add1#op2");
            sws.connect("add3#result", "add2#op1");
        );

        TESTEXCEPTION("Still cant connect same input twice",
            sws::already_connected,
            sws.connect("add1#op1", "add3#result");
        );

    );
}

template<typename T>
bool compareUnorderedSet(std::unordered_set<T> expected, std::unordered_set<T> got)
{
  if (expected == got)
      return true;

  printf("\tExpected:\n\t\t");
  for (auto it: expected) { printf("%s, ", it.c_str()); }
  printf("\n");
  printf("\tGot:\n\t\t");
  for (auto it: got) { printf("%s, ", it.c_str()); }
  printf("\n");

  return false;
}

void testListConnectable()
{
    // +--------+     +--------+     +--------+
    // |  add1  |     |  add2  |     |  add3  |
    // >op1     |  +-->op1     |     >op1     |
    // |  result<--+  |  result<     |  result<
    // >op2     |     >op2     |     >op2     |
    // +--------+     +--------+     +--------+

    typedef std::unordered_set<std::string> PathList;
    TEST("List connectable",
        swsEngine sws;

        TESTNOEXCEPTION("Create fixtures",
            sws.newModule("add1", "add");
            sws.newModule("add2", "add");
            sws.newModule("add3", "add");
        );

        TESTTRUE("Without connections - input",
            PathList got = sws.listConnectable("add1#op1");
            PathList expected;
            expected.insert("add2#result");
            expected.insert("add3#result");
            return compareUnorderedSet(expected, got);
        );

        TESTTRUE("Without connections - output",
            PathList got = sws.listConnectable("add1#result");
            PathList expected;
            expected.insert("add2#op1");
            expected.insert("add2#op2");
            expected.insert("add3#op1");
            expected.insert("add3#op2");
            return compareUnorderedSet(expected, got);
        );

        TESTNOEXCEPTION("Create connections",
            sws.connect("add1#result", "add2#op1");
        );

        TESTTRUE("With connections - connected input (add2#op1)",
            PathList got = sws.listConnectable("add2#op1");
            PathList expected;
            return compareUnorderedSet(expected, got);
        );

        TESTTRUE("With connections - other input (add2#op2)",
            PathList got = sws.listConnectable("add2#op2");
            PathList expected;
            expected.insert("add1#result");
            expected.insert("add3#result");
            return compareUnorderedSet(expected, got);
        );

        TESTTRUE("With connections - upstream input (add1#op1)",
            PathList got = sws.listConnectable("add1#op1");
            PathList expected;
            expected.insert("add3#result");
            return compareUnorderedSet(expected, got);
        );

        TESTTRUE("With connections - downstream output (add2#result)",
            PathList got = sws.listConnectable("add2#result");
            PathList expected;
            expected.insert("add3#op1");
            expected.insert("add3#op2");
            return compareUnorderedSet(expected, got);
        );
    );
}

void testBasicSchemaStep()
{
    TEST("Basic schema step",
        swsEngine sws;

        TESTNOEXCEPTION("Create fixtures",
            sws.newModule("val1", "value");
            sws.newModule("val2", "value");
            sws.newModule("val3", "value");
            sws.newModule("add1", "add");
            sws.newModule("add2", "add");
            sws.connect("val3#value",  "add2#op1");
            sws.connect("add1#result", "add2#op2");
            sws.connect("val1#value",  "add1#op1");
            sws.connect("val2#value",  "add1#op2");
        );

        TESTEQUAL("Step 1", 6,
            sws.set("val1#value", 1);
            sws.set("val2#value", 2);
            sws.set("val3#value", 3);
            sws.step();
            return sws.get("add2#result");
        );

        TESTEQUAL("Step 2", 10,
            sws.set("val1#value", 5);
            sws.step();
            return sws.get("add2#result");
        );
    );
}


void testNestedSchemaStep()
{
    TEST("Nested schema step",
        swsEngine sws;

        TESTNOEXCEPTION("Create fixtures",

            // Custom1: 3 input add
            sws.newModule("custom1", "container");

            sws.newModule("custom1/add1", "add");
            sws.newModule("custom1/add2", "add");

            sws.newModule("custom1/op1", "input");
            sws.newModule("custom1/op2", "input");
            sws.newModule("custom1/op3", "input");
            sws.newModule("custom1/result", "output");

            sws.connect("custom1/op1#value", "custom1/add1#op1");
            sws.connect("custom1/op2#value", "custom1/add1#op2");
            sws.connect("custom1/op3#value", "custom1/add2#op1");
            sws.connect("custom1/add1#result", "custom1/add2#op2");
            sws.connect("custom1/add2#result", "custom1/result#value");

            // Custom2: Doubler
            sws.newModule("custom2", "container");

            sws.newModule("custom2/mult", "multiply");
            sws.newModule("custom2/value", "value");
            sws.set("custom2/value#value", 2);

            sws.newModule("custom2/op", "input");
            sws.newModule("custom2/result", "output");

            sws.connect("custom2/mult#op1",  "custom2/op#value");
            sws.connect("custom2/mult#op2",  "custom2/value#value");
            sws.connect("custom2/mult#result",  "custom2/result#value");

            // Main schema
            sws.newModule("in1", "input");
            sws.newModule("in2", "input");
            sws.newModule("in3", "input");
            sws.newModule("out", "output");

            sws.connect("in1#value", "custom1#op1");
            sws.connect("in2#value", "custom1#op2");
            sws.connect("in3#value", "custom1#op3");
            sws.connect("custom1#result", "custom2#op");
            sws.connect("custom2#result", "out#value");
        );

        TESTEQUAL("Step 1", 12,
            sws.set("in1#value", 1);
            sws.set("in2#value", 2);
            sws.set("in3#value", 3);

            sws.step();
            return sws.get("out#value");
        );

        TESTEQUAL("Step 2", 16,
            sws.set("in3#value", 5);

            sws.step();
            return sws.get("out#value");
        );
    );
}

int main()
{
    testModuleHierarchy();
    testModuleDeletion();
    testConnecting();
    testListConnectable();
    testBasicSchemaStep();
    testNestedSchemaStep();

    return 0;
}
