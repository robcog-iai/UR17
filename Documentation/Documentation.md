# Realistic Walking  
* The realistic walking for the character is implemented in [GMovment.h](https://github.com/wzeitler/UR17/blob/ur17-p4/Source/UGame/Private/Character/Components/GMovement.h) 
and [GMovment.cpp](https://github.com/wzeitler/UR17/blob/ur17-p4/Source/UGame/Private/Character/Components/GMovement.cpp). GMovment is the movment component which handles the movment with the keyboard button W, A, S and D.

* For the realisitc movment the curved function [PT1-Glied](https://de.wikipedia.org/wiki/PT1-Glied) was used, 
which increasses the movment speed until the maximum threshold is reached.  


