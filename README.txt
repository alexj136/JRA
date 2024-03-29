=== AN EXEMPLARY JIT COMPILER ===

This is the repository for the project with the above title - it is a simple JIT
(or 'just-in-time') compiler.

    /minty-spec contains files that describe a simple programming language
    called 'minty'

    /minty-py contains an interpreted implementation of the language written in
    python

    /minty-c contains a c implementation of the language with an interpreter and
    code generator, that dynamically shifts between interpretation and JIT
    compilation as the execution strategy, based on a cost/benefit analysis

=== PROJECT DESCRIPTION ===

Compilers are used in informatics to translate from programming languages humans
can understand to machine code that can be executed by computers.  Compilers
come in two forms:

    Conventional compilers, where translation and program execution happen at 
    unrelated times.

    Just-in-time (JIT) compilers, where translation happens at the last possible
    moment, microseconds (or less) before execution.

In the last few years JIT compilers have become widely used (e.g. they are now
found in every web-browser) because the code they produce can be faster in many
circumstances than the code produced by conventional compilation. JIT compilers
can often better optimise the machine code they produce, because when a program
gets executed, more information is available about the execution context, and
is information is helpful for optimisation.

One key idea in JIT compilation is to have two program execution mechanisms, one
simple but slow, the other complex yet fast. In addition, while the slow one
executes, code execution statistics are collected, e.g. how often is a loop
executed? If the execution statistics indicate that a part of the code is
executed much more frequently than others (called "hot code"), the JIT compiler
switches to the fast execution mechanism for the hot code. Later, when the hot
code has finished, execution is transferred back to the slow mechanism.

JIT compilers are substantially more complicated than conventional compilers.
In particular, the hand-over from slow to fast execution mechanism and back, is
highly intricate.  For this reason, but also because they are much newer, most
research on compiler correctness has been carried out for conventional compilers
only.

As JIT compilers are ubiquitous, it is not only a technically interesting
challenge to transfer the existing work on compiler correctness from
conventional compilers to JIT compilers, but also vitally important because
faulty JIT compilation can be used by attackers to gain illegal access to
computers.  JIT compiler complexity is a major stumbling block in studying
their correctness. It would be helpful in this context, to have available a
simple, idealised JIT compiler for developing and testing theories of JIT
compiler correctness. When this is successful, the results can be generalised to
realistic JIT compilers.

As a JRA, I will develop  such an idealised JIT compiler, and make it available
for researchers. This means choosing an idealised programming language, and
developing a simple, but slow execution mechanism for it (called an
interpreter). Then I will add facilities for taking simple code execution
statistics to the interpreter. Then I will write a second, faster code execution
mechanism. All these steps are relatively straightforward, based on what I
learned in Dr. Berger's compilers course. The next step will be challenging: I
will add the ability to switch between the two execution engines at program
execution time. The result will be a fully function idealised JIT compiler that
is an ideal prototype for further research, by Dr. Berger or others.

This work will be of benefit to the department, particularly to Dr. Berger.
Much of his work focuses on program correctness for non-trivial programming
languages and models, and my research will enable him to transfer his existing
work to a new domain, namely JIT compilers.

=== HOW IT WORKS ===

Programs are lexically analysed in the typical manner - a table-driven finite
state machine describes the lexical structure of the language and functions as a
tokeniser.
    Parsing is also handled in a conventional manner - A non-left-recursive
context-free grammar is used as a basis for a top-down parsing algorithm.
    The code generated by the code generator is x86 machine code, that adheres
to the C calling convention. This allows it to be called by, and to call,
C code, which is the main principle that the JIT compiler uses to operate.
    Initially, an input program is given to the lexer & parser, and the
generated AST is passed to the interpreter. Every function object, and every
statement object within the AST (that is, print, return, if/else, assignment,
for & while statement objects), each contain an integer variable that records
the number of times that the interpreter has looked at that object. When that
variable reaches a certain threshold value, and the interpreter reaches that
AST object, instead of interpreting it, it invokes the JIT compiler. With a
function, a machine code string is generated that represents the entire
function. This is then written to some executable memory. Since the generated
machine code uses the C calling convention, that code can be called directly
from C using a function pointer to that executable memory. This function pointer
is then stored in the AST node for that function, and from that point on,
whenever the interpreter reaches that function, instead of interpreting the AST,
it simply calls the machine code 'version' of the function. The machine code
functions themselves handle function calls using function pointers to
the interpreter functions, which again is possible because the JIT generated
code adheres to the C calling convention.
    Statements are not JIT compiled independently (i.e. without compiling the
entire function that contains them) as the communication between the interpreter
and JIT compiled code becomes too complex when handling variables. Expressions,
however, are JIT compiled independently of functions and statements. Variables
are handled within expressions by giving the JIT machine code pointers to the
variable values within the data structure that handle variables in the
interpreter.
