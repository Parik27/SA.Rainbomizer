# Contributing to Rainbomizer

This file contains the guidelines for contributing to Rainbomizer.
All issues and pull-requests should follow these guidelines before they can be considered or merged into the main repository.

## Issues

Issues are methods to improve the project by informing the developers of bugs, or suggestions of new features.

- Any crash reports should contain the Rainbomizer log file. (rainbomizer.log.txt found in the same directory as the Rainbomizer ASI).
    - It is recommended, but not required, to also attach a minidump from Ultimate ASI Loader, found in the CrashLogs directory in the main GTA SA directory.
      
- New feature requests should contain a concise description of what the new feature should be. You should also make sure that the feature
  hasn't already been requested or been worked-on.
  
- Feature requests should be relevant to improving upon existing, or adding new randomization aspects. Any feature requests that aren't 
  relevant to the randomizing nature of the mod will be ignored. 
  
## Pull Requests

Pull requests are direct ways to contribute to Rainbomizer. Pull requests include implementing new features, bug-fixes or other improvements
made directly to the code. Pull requests are highly appreciated, but they must follow the following guidelines if they are to be merged into the main repository.

- All code should be formatted using clang-format using the .clang-format file provided in the main repository. 
  This includes the following conventions.
  - There should be a break after the return type of the function unless it is followed by a calling convention specification (eg. __fastcall)
  
    ```cpp
    int 
    function (..)
    ```
    
  - The indent width should be 4 characters.
  - Columns **must not** exceed 80 characters.
  - All parentheses must have a space preceding them. `function_call (a + b)` instead of `function_call(a+b)`
  - Pointers are right aligned. i.e `int *int_pointer` instead of `int* int_pointer`
  - There should be a line-break before all braces.
  
    ```cpp
    if(...)
    {
      ...
    }
    else
    {
      ...
    }
    ```
    
    This rule is not applicable to namespace declarations 
    
 - Use a singleton model for all randomizer classes. They should have a static instance that can be retrieved by a function.
    - See traffic.cc and traffic.hh for examples. Store any variables related to the randomizer in that class.
    - Avoid using global variables.
  
  - All function or class definitions should be divided by a comment containing 55 asterisks.
    ```cpp
    /*******************************************************/
    void
    function (...) 
    { 
      ...
    }
    
    /*******************************************************/
    void
    function2 (...)
    {
      ...
    }
    ```
    
 - Use the provided RegisterHooks functions for hooks wherever possible. 
 
    - To call the original function, use the `HookManager::CallOriginal`. (Note: This only works for hooks created with RegisterHooks)
  functions provided. This is to increase compatibillity with other mods. 
    - To resolve conflicts, it might also be nececssary to use the
  RegisterDelayedHooks or RegisterDelayedFunctions methods.
 
 - Do **not** use any compiler specific extensions that are not a part of the ISO C++ standard. At the very least, the code should be made 
   compatible with both the MSVC and GCC compilers. 
   
 - Unless absolutely necessary, do **not** use inline-ASM for the implementations of any hooks. If you have to use inline-ASM, 
  make it compatible with both MSVC and GCC. 
  
    Using inline-asm makes the code compiler-specific and more-importantly, makes the code unreadable. In most cases, inline-ASM can be
    replaced by equivalent C++ code, or replaced with simple hooks.
    
    ```cpp
    // Replaced code
    mov eax, CommandsExecuted
    inc dword ptr [eax]
    ```
    For example, this example code from scrlog can simply be rewritten in C++ as:
    ```cpp
    CommandsExecuted++;
    ```
  
  - All include statements except includes from the standard library should be included in the source file and not the header file. 
    This speeds up compile time and is a good practise in general.
 
