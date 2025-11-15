# asyncEn
asyncEn is a minimalistic event based simulation for testing AI algorithms.  
This is the simplest environment I could write.  Everything is a circle or a line (wall) to simplify collision detection.  
Agents driving objects' behavior send momentary actions and receive events from the environment.  
Actions are left/right/forward kicks to propell itself.  Events include collisions and wave interactions.  
Simulation takes place on the surface that generates waves during collisions and kicks. Waves dissipate as they propagate. This should allow agents to determine distances.  

# Techical details
Simulation is written in C++.  
There are no library dependencies other than SDL2.  
Agents will run on their own threads.  
I might implement stdio-style agents so that they can be stand alone executables.  Alternatively I might implement this environment to be a stdio-style stand alone executable.  This way it will take actions on stdin and output events on stdout.  The problem with this implementation is that IO has to be multiplexed on stdin/stdout among multiple agents.

# Notes
* It seems ALL objects can calculate their own locations, velocity and acceleration till a collision occurs
* When a collision occurs, location, velocity and acceleration have to be calculated for them because at least 2 objects were involved

# TODO
* Should in-game time be expressed in ticks or std::chrono::high_resolution_clock?  
Hi-res clock can be used in the future to convey the time of the collision more precisely.
With tick count asyncEn does not have to guess when to relay an action to an object in the environment.