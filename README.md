# <p align="right">![Feather Logo](https://github.com/0xBaca/Feather/blob/Developement_Beta/feather-1.0/logo/128x128/apps/feather.png)</p><p align="center">Feather</p>
<p align="center">https://www.feathereditor.com</p>

## What is Feather? ##

The only terminal based text editor designed to work with BIG files.
It doesn't matter if you will edit a 100MB or 100GB file. Feather will do.

This text editor is written from scratch by its author and does not copy any existing text editor
or idea how text editor should be implemented.

## Portability ##

You can compile Feather on Linux or MacOs. Windows port is on the way.

## Compiling ##

Run one of the build scripts from source directory:<br />

`./build_deb_1.0-1.arm_64.sh`

You will need the following packages:<br />
| Deb based linux | Rpm based linux |
|--- | --- |
| debhelper-compat |  rpmbuild |
| libncurses-dev | ncurses-devel |
| libncursesw | cmake |
| libncurses | clang |
| cmake | |
| clang | |

Feather is written with C++17 so make sure you have the right libraries.

If you find missing C++ headers, provide those with -I directive. [Makefile](https://github.com/0xBaca/FeatherEditor/blob/master/feather-1.0/makefile) has some default paths but it can vary between systems.

## Running tests ##

You will need the following packages:<br />
- boost
- cmake

Execute: [`run_test.sh`](https://github.com/0xBaca/FeatherEditor/blob/master/feather-1.0/scripts/run_tests.sh)

## Documentation ##

Official webpage [feathereditor.com](https://feathereditor.com) is the best source of documentation for Feather so far.

## Copying ##

Feather is free but any copied versions must include the [License](https://github.com/0xBaca/FeatherEditor/blob/master/LICENSE) with the author name and its email address.

Summary of the [License](https://github.com/0xBaca/FeatherEditor/blob/master/LICENSE): There are no restrictions on using or distributing an
unmodified copy of Feather but it must include the [License](https://github.com/0xBaca/FeatherEditor/blob/master/LICENSE) text with author name and email.


## Showing your appreciation ##

I will be working on improving Feather. If you find it useful or you just would like to help any contribution will be appreciated.<br />


## Helping to improve Feather ##

If you would like to contribute or you have any questions feel free to drop me a message: 0xbaca@gmail.com.


## Author ##

Mateusz Wojtczak <br />
0xbaca@gmail.com <br />


