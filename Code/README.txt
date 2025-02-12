This source code is an incomplete implementation of a barbershop simulation.
Some of its modules are the folowing:

- thread: POSIX threads wrapper module (removes the burden for result
          verification and error handling)

- process: process and System V IPC wrapper module (removes the burden for
           result verification and error handling)

- dbc: a Design-by-Contract module (replaces assert.h)

- logger: a smart log module, able to visualize information in a window mode
          or in a (classical) line mode.

- timer: module that implements a pause function (named: spend) applicable to
         multiples of a defined period (named: time unit).

- utils: some supplementary functions, including a wrapper for malloc (mem_alloc)
         that removes the burden for result verification and error handling;
         and random number generators.

- global: module containing the simulation parameters data structure, a global
          variable (yuk!) for them, and some macro definition (eg. utf8 symbols)

To better understand the barber and client modules start by looking to the
life function.

--

To ease your work in this assignment, I have annotated the locations in the
provided code in which changes are required. You can, of course, change other
parts of the source code, but please be careful not to destroy important
functionalities and correctness verifications.  These locations can be
identified with comments containing a TODO keyword (as: "to do"). Sometimes,
besides the TODO annotation, a description of the actions required are also
included.

Not taking into account possible BUGS in the provided code [2],
a correct implementation only needs to change four modules:
- simulation
- barber-shop
- barber
- client

--

The source code contains a Makefile, which should be used for project
compilation and cleanup:

make           # compile the code
make clean     # removes object files
make cleanall  # removes object file and executables

The project can be compiled to adapt to the project needs (ASCII output,
EXCEPTION error handling policy, and so on).  For example:

make SYMBOLS="-DASCII_MODE -DEXCEPTION_POLICY"

(a script is provided to use the exception policy library, though such a mode
is not necessary)

--

Code development had followed a Design-by-Contract approach [1].
Since C/C++ has no native support for this methodology, we provided
a dbc module.

Contracts are used to verify the correctness of the code, hence it can (sometimes)
catch concurrency errors resulting, for instance, from race-conditions.
Sometimes, contracts involving concurrent conditions (i.e. conditions whose value
may depend on the activity of another active entity), may express conditional
synchronization points).

Miguel Oliveira e Silva, December 2018

[1] https://en.wikipedia.org/wiki/Design_by_contract

[2] That's programming folks!

