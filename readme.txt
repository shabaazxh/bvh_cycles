

This is a variant on the triangle renderer that reads in an existing model in directed edge format with normal vectors.

UNIVERSITY LINUX:
=================

The university machines have QT 5.15.2 installed.

To compile on the University Linux machines, you will need to do the following:

[userid@machine LoopSubdivisionRelease]$ module add legacy-eng
[userid@machine LoopSubdivisionRelease]$ module add qt/5.15.2
[userid@machine LoopSubdivisionRelease]$ qmake -project QT+=opengl LIBS+=-lGLU
[userid@machine LoopSubdivisionRelease]$ qmake
[userid@machine LoopSubdivisionRelease]$ make

You should see a compiler warning about an unused parameter, which can be ignored.

To execute the renderer, pass the file name on the command line:

[userid@machine LoopSubdivisionRelease]$ ./LoopSubdivisionRelease ./diredgenormals/tetrahedron.diredgenormal

RASPBIAN (RASPBERRY PI):
========================

If you have a raspberry pi, you can install QT 5 and the necessary tools with:
	sudo apt install gcc
	sudo apt install qt5-base5-dev qt5-qmake qtbase-5-dev-tools
	
This will install QT 5.15.2, and should work as described for the University machines.

UBUNTU 22.04.01:
================

Under Ubuntu, it is possible to install either QT 5 or 6.  This involves (at a minimum):
	sudo apt install gcc
and
	sudo apt install qt5-base5-dev qt5-qmake qtbase-5-dev-tools
or
	sudo apt install qt6-base-dev qt6-base-dev-tools qmake6

It should then work in much the same way as Raspbian or the university machines.

MAC OS X SONOMA:
================

To compile on your Mac running OS X Sonoma (earlier versions are similar):

[userid@machine LoopSubdivisionRelease]$ qmake -project QT+=opengl QT+=openglwidgets QMAKE_CXXFLAGS+=-DGL_SILENCE_DEPRECATION
[userid@machine LoopSubdivisionRelease]$ qmake
[userid@machine LoopSubdivisionRelease]$ make

You will see some compiler warnings about deprecated use of x() and y().  These are due to changes in QT 6, which is what homebrew installs on OSX.  It will still compile.

On OSX, QT applications are stored in a "bundle" - a directory containing all of the resources the program needs in a UI environment.  To execute the application, use:

[userid@machine LoopSubdivisionRelease]$ ./LoopSubdivisionRelease.app/Contents/MacOS/LoopSubdivisionRelease ../handout_models/tetrahedron.tri

You can also create a symbolic link after your compile:

[userid@machine LoopSubdivisionRelease]$ ln -s ./LoopSubdivisionRelease.app/Contents/MacOS/LoopSubdivisionRelease LoopSubdivisionRelease

after which you can invoke the executable as follows:

[userid@machine LoopSubdivisionRelease]$ ./LoopSubdivisionRelease ./diredgenormals/tetrahedron.diredgenormal

================================================
COMPILE INSTRUCTIONS CONTRIBUTED BY RAFAEL

QT CREATOR:
===========

Download and install QT creator + Qt.
https://www.qt.io/download-open-source

- Click "download the QT Online installer" and pick your platform. 
- Create an account and login to proceed, and go forward until "select components" where you should select a version of QT creator +QT.
- Expand the section on "Qt" and choose a version of Qt. If you want, 5.15.2 would give you the same setup as the labs. 
- Proceed normally.

Creating project
- Open QtCreator
- File, New File or Project, Import project, Import as qmake project.
- Name the project "LoopSubdivisionRelease"
- Choose the folder where your source is (same as where this file is)
- Click next twice, and finish.

Configuring project
- After creating, you will see the "configure project" screen. Pick one compiler option that has been installed for you (e.g. windows I recommend MinGW, so it behaves more similar to the university labs compilers).
- Expand the "LoopSubdivisionRelease" project, and open LoopSubdivisionRelease.pro

Paste the necessary flags for compilation from each platform

macosX Sonoma:
QT+=opengl 
QT+=openglwidgets 
QMAKE_CXXFLAGS+=-DGL_SILENCE_DEPRECATION

windows:
QT+=opengl
LIBS += -lopengl32

linux: 
QT+=opengl

Running:
- Click "projects", and your active configuration. 
- Choose "run"
- Add the command line arguments: ./diredgenormals/tetrahedron.diredgenormal
- Click the green play button. 

WINDOWS:
========
Add two folders to your PATH environment variable:

- Press windows button, type "env" and choose from the search results "edit the system environment variables"
- Alternatively, search for "system properties"
- Click "Environment variables.
- Under system variables, find "PATH"
- Select and click Edit.
- Click new, and add the two following folders:

XXs are the numbers for your QT version
C:\Qt\Qt5.XX.X\Tools\mingw730_64\bin
C:\Qt\Qt5.XX.X\5.XX.X\mingw73_64\bin

Press ok, close it. 

Compiling
- Open a terminal at the folder where your source is (shift+right click on explorer): 
- qmake -project QT+=opengl
- qmake
- mingw32-make.exe

Running
- .\release\LoopSubdivisionRelease.exe ./diredgenormals/tetrahedron.diredgenormal
