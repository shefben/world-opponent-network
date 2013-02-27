/* Links.  If you've never used a linked list before, you're in the WRONG place.
// I'm not fond of using a generic linking structure, but that's how it's done.
//
// The main reason it's a separate FILE is to cut down on header hell.
*/

#ifndef OSHLDS_LINKS_HEADER
#define OSHLDS_LINKS_HEADER

// structs
typedef struct link_s {
   struct link_s *prev, *next;
} link_t;

// functions
void ClearLink(link_t *);
void RemoveLink(link_t *);
void InsertLinkBefore(link_t *, link_t *);
void InsertLinkAfter(link_t *, link_t *);
#endif
