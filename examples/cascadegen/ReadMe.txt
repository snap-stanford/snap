========================================================================
    Cascadegen: Cascade Detection
========================================================================

The example detects top cascades from a list of events. The user can specify
a paramter W to detect W-adjacent events.

The code works under Windows with Visual Studio or Cygwin with GCC,
Mac OS X, Linux and other Unix variants with GCC. Make sure that a
C++ compiler is installed on the system. Visual Studio project files
and makefiles are provided. For makefiles, compile the code with
"make all".

/////////////////////////////////////////////////////////////////////////////
Parameters:

/////////////////////////////////////////////////////////////////////////////
Usage: ./cascadegen <filename> <W>

/////////////////////////////////////////////////////////////////////////////
Output: A directed graph of W-adjacent events.
