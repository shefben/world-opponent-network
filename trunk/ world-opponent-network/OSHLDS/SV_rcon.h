/* The rcon is, well, a remote password protected console.  Aside from a
// challenge request to verify IPs, the basic setuo is this: we get one
// packet in the form of 0xFFFFFFFF "rcon" <challenge> <password> <command>.
//
// The astute will probably observe that this simple plain text transmission
// of the PASSWORD is very BAD.  However, there's nothing I can do about
// that until the client is ready (though I CAN add a more secure version
// if admins are willing to use a third program).
//
// Unfortunately the output will be to the console, not the user.
// The console will need a bit of an overhaul before we can redirect
// the output.  Command and cvar levels are also of interest.
*/

#ifndef OSHLDS_SV_RCON_HEADER
#define OSHLDS_SV_RCON_HEADER


void SV_RconInit();
void SV_RconShutdown();
void SV_Rcon();

//
int ServerIsPassworded();
int CheckPassword(const char *);

#endif
