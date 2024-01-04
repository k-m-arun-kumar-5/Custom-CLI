Description :
=============
I, K.M. Arun Kumar alias Arunkumar Murugeswaran, just shared my works, in which I worked as a learning path and practiced some C programming such as custom Command Line Interface(CLI).
There are 3 Levels of CLI users: Normal (user), configuration (root), and engineering (engg). These levels will be protected by a password. User mode has the least privileged access to RAS commands.
Engg mode is the most privileged access to RAS commands. Default CLI user's password are as follows: for CLI user, the password is user and prompt is user>, for CLI root user, the password is the root, and the prompt is root# 
and for CLI engg user, password is engg and prompt is engg$.

CLI commands are as follows:
============================
CLI command                           description                                CLI user's level access
login <CLI user level>              - login to <CLI user level>                  user, root, engg      
set passwd <CLI user level>         - set passwd for <CLI user level>            root, engg 
set system prompt <prompt>          - set prompt for current CLI user level      user, root, engg      
who am i                            - display the current CLI user level.        user, root, engg    
show mode list                      - display all CLI user levels.               engg
exit application                    - exit from application                      user, root, engg  

CLI command for numeric operations    description                                CLI user's level access
<num1> + <num2>                     - numeric addition                           user, root, engg
<num1> - <num2>                     - numeric subtraction                        user, root, engg  
<num1> * <num2>                     - numeric multiplication                     user, root, engg  
<num1> / <num2>                     - numeric division                           user, root, engg
<num1> % <num2>                     - numeric modulus                            user, root, engg

? will provide various next-level CLI command options. 

for example 
user>?

<num1>           exit        login
set              who

Purpose :
==========
In all my respective repositories, I just shared the works that I worked as the learning path and practiced, with designed, developed, implemented, simulated, and tested, including some projects, assignments, documentation, and all other related files and some programming that might not be implemented, not being completed, lacks some features or have some bugs. The purpose of all my repositories, if used, can be used for LEARNING AND EDUCATIONAL PURPOSE ONLY. It can be used as open source and freeware. Kindly read the LICENSE.txt for license, terms, and conditions about the use of source codes, binaries, documentation, and all other files, located in all my repositories.

My Thanks and Tribute :
=======================
I thank to my family, Friends, Teachers, People behind the toolchains and references that I used, and all those who directly or indirectly supported me and/or helped me and/or my family, Nature and God. My tribute to my family, Friends, Teachers, People behind the toolchains and references that I used, Nature, Jimmy Dog, God, and all those, who directly or indirectly help and/or support me and/or my family.

Toolchains that I used for C Application design and development are as follows :
================================================================================
1: IDE or Compiler used - CodeBlocks with GNU GCC Compiler. 2: Desktop Computer Architecture and OS for C Programming - Intel X64 & Windows 10 (64 bit). 3: Code editor - Notepad++. 4: Documentation - Microsoft Office 2007 (alternative LibreOffice) and Text Editor. 5: File Archiver - 7-Zip

Note :
======
Kindly read in the source codes, if mentioned, about the Program Description or Purpose, Known Bugs, Caution, and Notes and documentation.

My Repositories Web Link :
==========================
https://github.com/k-m-arun-kumar-5
