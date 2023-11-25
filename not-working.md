List of unsupported games and programs, just because they are in this list does not mean you can’t use these programs, it just means you need to use EUAE instead.

Terminology for newbies

	DSI - Data storage Interrupt (software bugs)

		Most common causes.

		* NewMode is not running!!
			AmigaOS3.1 will try to open screens without modeid into hires mode.
			AmigaOS4.1 will try to open screens without modeid into a true color mode.
   			As result AmigaOS4.1 crashes, many old programs do not set mode id.
			download NewMode from Aminet, 
   			(AmigaOS 4.1 Screens prefs can only promote screens that has
			successfully been opened once. So you can't use it!)

		* Failed to allocate memory
			(make sure to have chip ram, see SetPatch docs.)
     
		* Assuming items are load that’s not (data files, fonts, libraries)
			(Check Snoopy logs)
       
		* Reads outside of allocated memory
			(Try press ignore DSI, and cross your fingers.)
    
		* Accessing part of OS that’s not allowed.
   			(Sorry nothing can be done, use EUAE)

		(If possible report problem to game or tool developer, if they are around.)

	ISI - Instruction interrrupt

		Most common causes.
 
 		* Missing JMP tables or trying to access code, that is not there.
   
   		* Broken function pointers, not set or overwritten with bad value.

     		(If possible report problem to game or tool developer, if they are around.)
     
	Black or gray Screen

		Most common causes.

		* Uses copper to setup screen, can't be displayed in RTG modes.
  
  		* Stuck in forbid state, program is not able to get out of busy loop.
    			(Nalle Puh, needs multitasking)

		Probably won’t be fixed unless someone has the source code.
 
## Games

The games that are tested are HD installable games, and workbench games.

	BloodNet AGA - crashes after a window is displayed. 
		they have some kind antipiracy code in there, that might cause problems.
		code is encrypted.

	Breathless - Game refuse to start, don't know why.
		(I believe the source code was released if someone feels the urge to fix it)

	Deluxe Galaga AGA – shows trainer, it asks for fire button on joystick to be pressed.
		if press joystick with Nalle Puh 1.2, games continue and does something… 
		you can't see anything on the screen, but after while there is some sound,
		then it crashes. (so not really working)
    
	SpaceQuest 1 – Not working, refuse to start (too old)

	SpaceQuest 4 – Not working, refuse to start (too old)

	SimCity 2000 – it does start it has sound, make sure to read install script,
		or install under EUAE first, problems: no font and game crashes.

	Simon the Sorcerer - starts DSI error, and ends up at a back screen.

	The Shadow of time - starts and ends up on a black screen.

	The Settlers - Intro crashes, and the games crashes, 

	T-zerO-Demo - black screen.

## Music players and editors

	303TrackerV0.12 - Black Screen.

	AHX - DSI error crashes.

	Beatblox - Black Screen

	Delitracker 2.32 - do not work, freeze the system.
 
	Deluxe Music 2.0 - unstable, crashes in audio.device with DSI error

	Eagle Player - do not work, freeze the system.
    
	FutureComposer - DSI error crashes.

	MusicBugs - System freeze (copper screen i think.)

	Music-X - System freeze.

	OctamedPLayer - DSI error crashes.
    
	THX Sound System 2.3d, crashes badly, no idea why.

	SoundFX (loads 14%, and crashes with DSI error)

 	SuperJam (plays notes, but crashes badly)

	Octamed Soundstudio 1.03c - DSI error in the playback timer interrupt routine, 
		bad pointer value, is sent to NallePuh (use Matrix instead.)

 	Aegis Sonix - does not start, probably is not using AvailMem, 
		and can't find chipmem in execbase.

## Demos

	Boogietown - locks up at "Titanics-Cruncher decrunches while loading..." message.
 		(not possible to get into shell using AUX:)

 	RNO - Fourskin - locksup quicky. (NOT TESTED WITH NEWMODE)

	RNO - 4tft - locksup quickly, even when using newmode.

 	RNO - Boozembly08 - locksup at "Titanics-Cruncher decrunches while loading..." message.

  	DW-RPH-KVG-RNO - loads, but get stuck on gray screen, no music.
   		Intuition is probably locked up / maybe a cooper screen.
		(multitasking is working, you can get into shell over AUX:)
  
	RNO - Experiment2007 - locksup at "Titanics-Cruncher decrunches while loading..." message.

 
