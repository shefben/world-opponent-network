Building instructions:

(note, dates are important, as newer changes can break compatibility)
These files have been BUILT SUCCESSFULLY as of 15 JUNE 2008 on the following:
 * gcc (GCC) 3.4.5 (mingw special) (0 errors, 60 warnings)

These files have been BUILT SUCCESSFULLY as of 10 MAY 2008 on the following:
 * gcc (GCC) 4.1.1 20061011 (Red Hat 4.1.1-30)

These files have FAILED TO BUILD as of 10 MARCH 2008 on the following:
 * Visual Studio 6 sp 6  (32 errors, 367 warnings) (failed file was FS.cpp)
 * Visual Studio 8 (fails.  Compiler was not capable of recognizing and correcting errors though)

These files were COMPILED but not LINKED 10 MARCH on:
 * Borland 5.5.1 (many many warnings, couldn't find manual for linker)




Interesting mod issues:
 * eXistence 1.5, though long dead, seems to have been compiled renectly enough to NOT choke on the Windows function pointers.  Ran into interesting problms with the maxplayers and Existence (must be below 14, was not being set correctly).  Fixed, or at least hacked around, so no more crashes.  Version 1.5 doesn't seem all that stable though on the regular HLDS.  It doesn't crash on mine (too much).  Instead it overflows on transmitted ents and the world isn't navigatable.
 * Ben says a version of BuzzyBots works.  Need to find out which one though, as mine does not.
 * Counter-Strike seems to load (7.1, 1.3).  Hopefully 1.5 will work too, as let's face it, nobody really cares about anything else.  Testing is now being done with this.

Recent changes (6.15):
  This haitus brought to you by lethargy.  I have NOT investigated any bugs; rather I converted
  the cvar functions to an AVL tree.  Seems to work, though I may have to revert to the linked
  list if mods end up giving me grief.
Recent changes (5.24):
  Finished up some StudioModel code that CS wanted; think it's to make sure models have the same dimensions.
  Fixed a few memory leaks, including a rather nasty one that led to a rapidly shrinking hunk
Recent changes (5.16):
  cvar overhaul, removing all traces of that O(n) search from the main engine.
  *might not have finished the job* Fixed bug that made bullets, you know, NOT hit people.  Just involved passing the wrong thing to functions.
Recent changes (5.13):
  Tinkering around, trying to understand how this beast functions, and tallying which mods work.  Attempting to understand enough of the player movement to fix accuracy and lag.
  Broke apart world.c and sv_hlds.c into smaller files.  Started removing excess calls to cvar_variable*; it averages N/2 runtime, where N is the time taken to iterate over all of the cvars in the linked list.  Naturally, being called several million times each frame does NOT help speed.  The difference is already very noticeable.


Code files will usually have descriptions of their contents and intents.


Threading logic:

The engine gets one thread, the console gets another.  Since the console commands themselves are executed in the console thread, and can therefore seriously screw the engine up, only one is allowed to actually play with the engine at a time (a very comprehensive lock, but the alternative of many many locks for many many areas of the engine is doomed to fail).  An alternative to the locks would be making the engine thread actually execute the console commands (with the cbuf).  But that's basically more work for the same effect.
I'd like to block on calls to recvfrom in future, but since that too can muck about with the whole engine, I'm not quite certain what'd be the best way to balance everything.


Observed issues:

Rockets go backwards a bit in one section of Undertow.  Specifically, if you point the launcher down stream in the area you pick it up, it'll travel backwards, possibly hitting a wall or whatever and kill you (it won't collide with you though).  This doesn't appear to be my fault--it doesn't seem to do it on a Windows server, but it did so even without hijacking on unix.
You get a host_error message (client does) when the server tells you you timed out.  Of course, by then...
Custom decals are probably still broken.  Though to be fair, I could never get them working with the real server.
Timing issues.  There were some precision issues, and this may now be resolved.  If problems still exist, they likely cause the next problem:
Lag:  Speed issues resulted in unplayable ammounts of lag.  This may be resolved, but it's likely there are lesser factors which'll abuse players.
Windows: Mods won't work if they're compiled w/o frame pointers (a default opti on VS6).  That's most mods, unfortunately.  If this program compiles under newer VS versions, that might just fix it.
Accuracy:  Bullets are hitting maps where they should be, but not other entities.  Usually works in regular Half-Life.  Never seems to work against, say, civvies in Existence.
Memory:  Reports indicate there are two calls to new with unmatched deletes, leaking a constnt 3000 or so bytes.  There are only four news in my program, so there aren't too many possibilities here.  God only knows how the MODs are doing; frankly, Windows debugging options suck.  I might just make an actual non-VMware linux partition just so I can enjoy its debuggers.
Speed:  While we can run over 1000 frames per second, there are a large number of places that need to be altered to speed things up.  The major bottlenecks are gone, but there are more around.  This WAS related to various lag issues.  There might still be some networking issues left though, but this boosts playability.

Compiling notes:
The C files cannot be compiled as c++ files.  Since most compilers are smart enough to check the file extension, it's hard to do that accidentally.
Must link with Boost and pthreads.  On windows, winsock and winmm are also required.
libcprops will probably require configuring.  What I have now works for my system, but as the library installation hasn't been ported to mingw, you may find it easier to only compile the required five files "avl.c", "collection.c", "mempool.c", "rg.c", "vector.c".  Non-mingw users should probably install it from scratch.





If someone is able to write BETTER instructions or make the process easier, I'd appreciate it.  It's easy to me, but that's because I wrote it, and I've had my compiler set up the way I like it for a long time.


Since PAK files aren't supported you'll need to unpack them all.
Only the base Half-Life mod is supported.  On unix, all mods SHOULD run.  On Windows, it's very much hit and miss, depending on what the mod was compiled with.  I'm out of ideas on how to fix it too, but if you're running a mod you have the code to, you're in the clear.
Do not log, or run fancy scripts, or use addons without testing the basic server first.  Addons *should* work.  Logging won't.


Legal crap: GPL.  Large chunks come from the Quakeworld code (or FTEQW), and that's GPL'd, and since you can't easily separate out what's 100% mine from that, the whole thing gets a blanket GPLing.  Not that I mind too much if you use the code, since you'd be hard pressed to gross dollar one in doing so, but if you use it, you're now obligated to keep it open.
