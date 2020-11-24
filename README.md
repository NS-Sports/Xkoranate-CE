# xkoranate-CE
This README is a work in progress and should not be considered final.
## Overview

xkoranate-CE (xkoranate, **C**ommunity **E**dition) is a fork of ThirdGeek/Commerce Heights' **xkoranate** program, used for generating scores for sporting events held in NS Sports, the sports roleplay forum for the nation simulation game, NationStates.

**From Commerce Heights' (original creator) Web page on xkoranate:**

"xkoranate is a program that can be used to simulate results of a wide variety of individual and team sports, including Olympic sports. It is capable of scorinating events in one hundred disciplines of over sixty sports."

## Compiling/Installation
### Linux

**You will need:**
* A C++ compiler, such as `gcc`.

### Windows
Coming soon!

### macOS
Coming soon!

## Sport files

Each available event has parameters which are stored in an editable xml file. These files are placed in the “sports” directory, which can be found in xkoranate.app/Contents/Resources/sports/ in the Mac os version. In version 0.3.1 and later (including xkoranate-CE), the application should automatically find this directory.

## Contribution guidelines
When making a change, please follow these steps to maintain code integrity:
* Create a new branch from master to your own fork. DO NOT create new branches on the main repo. 
* Compile and test your changes on your fork where possible.
* If successful, merge to the test (or dev) branch, with a comment explaining your results, and request a review by another member of the organisation (anyone will do!)
* Once verified by a second developer, they can merge changes to the master branch.

Code changes should never be commmitted directly to the master branch without testing and review.
Full guidelines will be added here later, with more detail for those who are less experienced with Git.
