## About ##
<p>This project is here to help document old <a href='wonapi.md'>wonapi</a> functions, but also to aid in developing a new WON server architecture. This opens up the possibility to rebuild the old WON Libraries to bypass or create a new authentication system with the help of the released source code. The main scope of this project is to make aware the source code and information about the WON server systems, that lots of old games used, and developers that want to make the games network playable again.<br>
</p>
# We are looking for Dedicated Hosts for Directory/Master servers, Routing server/Game list/chat Server  & DBProxyServer Please Email Me if you are interested! #
## Update 6/16/2014 ##
We have just completed the Homeworld and Auth1 Authentication protocols, Though it is worth mentioning that it is barebones right now.  Login's are all guest at the momment until I complete the database portion of the server.
The following games now authenticate correctly:
ARC
Homeworld
Homeworld Cataclysm
Darkreign2

## Update 7/28/2015 ##
We have changed domains

Out new weebsite/domain is http://Neuwon.com
The official launching of neuwon will commence in 1-2 weeks

so get your sierra/won games ready!!!


The new authentication server revision will be up for beta testing when database portion is finished!

## Update 5/15/2014 ##
The Won network is near complete, most games such as Half-Life, ARC, StarTrek Armada, 3d Cool Pool and most other's (those games listed are the only games that have been tested successfully so far.)

Unfortunatly, The circumstances in which the WONServer code was received, we CANNOT Release the source code until it is rewritten without code that has copyright attached to it.

At the moment, WON Half-life Is 90% functional.  The Authentication, Directory & Chat server work as they would have back in 1999-2003.  We are currently using the WON2 masterserver/mod server network as the server list for testing purposed.

When the Network is bug free, this group will use its own Master and Mod Server's.

WONID's are generated from I.P. Address's which most people know are bound top change.  When out network fully launches it will be based on Serial Key's that cannot be modified, or regenerated as to prevent cheating (Mechanics of this are confidential to prevent such things)

To test Half-Life please put replace your valve/woncomm.lst with the following file:
[Woncomm.lst](https://drive.google.com/file/d/0B36Itj6rgoAwOFlhZ1JzbzdYVWc/edit?usp=sharing)

## Update ##
Empire Earth and AOC is now 100% functional through NEOEE net
the link it: [Neoee](http://www.neoee.net)
# Please Review Wiki for detailed & Extremely Helpful information #
## Information ##
So how are we going to approach this to make the Authentication and Directory servers? <br> The <a href='wonapi.md'>wonapi</a> from AVP2 and from Darkreign2 are complete.  It includes everything from creating <a href='encrypt.md'>pub/priv key pairs</a> & signing, it also include packet packing, encrypting and unpacking, receiving, processing and replying to ALL known won packets.  The only real coding that needs to be done is integrating <a href='wonapi#titan.md'>titanapi.lib</a> into a server template and use conditional statements to route the information to the right  parts of the library.<br>
<h2>Ways to approach</h2>
<ol><li>Reverse engineer the game and also capture packets (which is imposssible since won is shut down)<br>
</li><li>Rewrite the various won dll's to remove or remake the authentication process without modifying the game. (<a href='directory#wondll.md'>wondll.dll</a> , <a href='directory#wonauth.md'>wonauth(_95).dll</a> <a href='directory#woncrypt.md'>woncrypt(_95).dll</a> )<br>
</li><li>Use the API and os games like darkreign2 avp2 or homeworld, so logging can be  enabled to show the information sent and recieved for logging in/authentication.(current goal)<br>
<h2>SVN contents</h2>
(under /trunk)<br>
</li></ol><ul><li>Complete WONAPI and Titan Api now available from repo!<br>
<br>
<br>
<old><br>
<br>
<br>
<ul><li>partial header only wonapi sdk from <a href='hw1.md'>Homeworld</a>
</li><li>Full WONAPI sdk from <a href='dr2.md'>DarkReign 2</a>
</li><li>Partial WONAPI sdk from <a href='avp2.md'>Alien vs Predator</a>
<h2>Games that use WON</h2>
<ul>
</li></ul><blockquote><li><a href='hw1.md'>HomeWorld</a> - Use HomeWorld SDL for multiplayability (Cant find windows binary)</li>
<li><a href='hl1.md'>Half-life</a> - multiplayer available via <a href='http://won2.net'>masterserver woncomm.lst</a></li>
<li><a href='o2.md'>Outpost 2</a> - <a href='http://www.outpostuniverse.net/network.php'>Netfix client for server list</a></li>
<li><a href='sta.md'>Star Trek: Armada</a> - no known multiplayer fix / bypass</li>
<li><a href='sof.md'>Soldier of Fortune</a> - no known multiplayer fix / bypass</li>
<li><a href='darkreign2.md'>Dark Reign 2</a> - Multiplayer Patch comming soon from this project</li>
<li><a href='Silencer.md'>Silencer</a> - Multiplayer available through: <a href='http://rgb9000.net/silencer/'>server fix</a> or <a href='http://rgb9000.net/cypher'>o2 clone</a></li>
<li><a href='ARC.md'>Attack Retrieve Capture</a> - Find <a href='http://forums.arc-hq.net/'>ARC communities</a> for servers</li>
<li><a href='ultra.md'>3D Ultra Cool Pool</a> - Multiplayer works with Room server <a href='http://Pauth.com'>pauth</a></li>
<li><a href='ultra.md'>Maximum Pool</a> - Same ass cool pool <a href='http://Pauth.com'>http://Pauth.com</a> </li>
<li><a href='rb3d.md'>Redbaron 3D</a> - Multiplayer works with <a href='http://www.gunjones.com/RBservers.txt'>fixed meta-server list</a></li>
<li><a href='groundcontrol.md'>Ground Control</a> - no known multiplayer fix / bypass</li>
<li><a href='swat.md'>SWAT 1 2 &amp; 3</a> - no known multiplayer fix / bypass Use Swat 4</li>
<li><a href='tribes.md'>Tribes 1 &amp; 2</a> - Multiplayer is available through <a href='http://tribesone.com/forum/showthread.php?19177-New-Tribes-Master-Server-Instructions'>tribes 1 masterserver patch</a> or <a href='http://t2.plugh.us/#howget'>tribes 2 masterserver fix</a></li>
<li><a href='avp.md'>Alien vs Predator</a> - Multiplayer works with <a href='http://master.lithfaq.com/'>masterserver patch</a></li>
<li><a href='ydkj.md'>You dont know jack series</a> - no known multiplayer fix / bypass</li>
</ul></blockquote></li></ul>

<h2>Older games that use WON or Sierra network</h2>
<ul><li>Lords of Magic<br>
</li><li>Red Baron II - Change to RB3D Via Super patch<br>
</li><li>Civil War Generals 2<br>
</li><li>Lords of the Realm II<br>
</li><li>MissionForce: Cyberstorm<br>
</li><li>3D Ultra Minigolf<br>
</li><li>The Time Warp of Dr. Brain<br>
</li><li>Sierra Sports: Football '97 & '98<br>
</li><li>Sierra Sports: Golf<br>
</li><li>Trophy Bass 2 and Trophy Rivers<br>
</li><li>Shivers 2, Birthright<br>
</li><li>Hoyle Black Jack<br>
</li><li>Hoyle Poker<br>
</li><li>Hoyle Board Games (Backgammon, Battling ships, Checkers, Chinese Checkers, Chess, Yacht)<br>
</li><li>Hoyle Cards (includes Spades, Hearts, Gin and Poker)<br>
</li><li>Hoyle Casino (includes four versions of Poker: 5-card draw, 5-card draw low-ball, 7-card stud and Texas Hold'em)<br>
</li><li>Power Chess and Power Chess '98</li></ul>
