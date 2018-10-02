# UR17 - UGame Tutorial
Download the plugin in and put it inside the plugin folder of your project.  
The Plugin Folder (UR17/) contains the following file Input Backup 2018-10-02 221505.ini. Put this inside YourProject\Saved\Config\Windows to have the correct input mapping. Instead of moving the file into the correct folder it is also possible to import it inside the procject. To do so start your project, open the project settings and move to input, there you will finde the buttong import. Push this button and pick the .ini file form the plugin folder, to import the input settings.

## Character Setup
To use the character he needs to be put inside the level where he should be used. By including the plugin to your project you can access the Game Controller, which is the character. By navigating to UGame C++ Classes / UGame / Private / Character you can pull the Game Controller inside the game world.  
After the character is placed inside the world he has to be set to Player 0. This happens in the editor under Pawn, there you can set Auto Posses Player to Player 0.  
![](CharacterPawn.JPG "Character Setup")  

## Tutorial
The character can be moved with W,A,S and D and the mouse. To pick objects up or rotate them you can press the right mouse button to show the mouse cursor and by right clicking on the oject a menu comes up with the possible interactions.  
Before being able to interact with an object it needs the following Tags: UGame;Interactable,True;Pickup,True;. The picture shows where the correct Tags field is found.    
![](ActorTags.JPG "Actor Tags")  

The last thing is to set the object to moveable, which is shown in the following picture.  
![](ActorMoveable.JPG "Actor Tags")  

