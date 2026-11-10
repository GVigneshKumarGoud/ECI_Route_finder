/* ============================================================
   route_finder.h
   ECI Route Finder — Data Structure Definitions
   ============================================================
   This file defines the building blocks (data structures)
   used to represent a college campus road network as a graph.

   A "graph" is just a collection of:
     - Locations (called "nodes" or "vertices" in graph theory)
     - Roads connecting them (called "edges" in graph theory)
   ============================================================ */

/* ── Include Guard ──────────────────────────────────────────
   These three lines prevent this file from being read more
   than once by the compiler, even if it is included in
   multiple places. This is standard practice in C.        */
#ifndef ROUTE_FINDER_H
#define ROUTE_FINDER_H

/* ── Standard Headers ───────────────────────────────────────
   stdio.h  → needed for printing output (printf, etc.)
   stdlib.h → needed for general utilities (memory, etc.)
   string.h → needed for working with text (strings)      */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================
   CONSTANTS
   These are fixed values that will not change throughout
   the program. Using constants makes the code easier to
   update later — change the number in one place only.
   ============================================================ */

/* Maximum number of locations (nodes) in the road network */
#define MAX_LOCATIONS 20

/* Maximum number of roads (edges) in the road network */
#define MAX_ROADS 30

/* Maximum number of characters allowed in a text field */
#define MAX_NAME_LEN 50

/* Maximum number of direct neighbours any one location can have.
   In our campus map no location connects to more than 10 others. */
#define MAX_NEIGHBOURS 10

/* ============================================================
   STRUCTURE: Location
   Represents a single place on the campus map.
   Think of it as one row from your locations.csv file.
   ============================================================ */
typedef struct {

    char location_id[10];        /* Unique ID, e.g. "L01", "L02"        */
    char location_name[MAX_NAME_LEN]; /* Full name, e.g. "Main Gate"    */
    char location_type[MAX_NAME_LEN]; /* Category, e.g. "Academic"      */

} Location;

/* ============================================================
   STRUCTURE: Road
   Represents a single road (connection) between two places.
   Think of it as one row from your routes.csv file.
   Every road is bidirectional — you can travel either way.
   ============================================================ */
typedef struct {

    char road_id[10];            /* Unique ID, e.g. "R01", "R02"        */
    char from_id[10];            /* Starting location ID, e.g. "L01"    */
    char to_id[10];              /* Ending   location ID, e.g. "L02"    */
    int  distance_m;             /* Road length in metres  (e.g. 200)   */
    int  travel_time_min;        /* Travel time in minutes (e.g.   3)   */

} Road;

/* ============================================================
   STRUCTURE: Neighbour
   Represents one direct connection seen from a location's
   point of view inside the adjacency list.

   Example: from L01, one neighbour entry would be:
     to_index       = 1      (meaning L02, index 1)
     distance_m     = 200    (road is 200 metres long)
     travel_time_min= 3      (takes 3 minutes to travel)
   ============================================================ */
typedef struct {

    int to_index;           /* Index (0–9) of the neighbouring location */
    int distance_m;         /* Cost by distance  (metres)               */
    int travel_time_min;    /* Cost by time      (minutes)              */

} Neighbour;

/* ============================================================
   STRUCTURE: Graph
   The complete campus road network.
   It holds an array of all locations and all roads together.
   This is the main "container" your program will work with.
   ============================================================ */
typedef struct {

    Location locations[MAX_LOCATIONS]; /* All campus locations           */
    int      location_count;           /* How many locations are loaded  */

    Road     roads[MAX_ROADS];         /* All roads between locations    */
    int      road_count;               /* How many roads are loaded      */

    /* ── Adjacency List ────────────────────────────────────────────
       adj[i] holds all direct neighbours of location at index i.
       neighbour_count[i] says how many neighbours location i has.

       Example after building:
         adj[0][0] → { to_index=1, distance_m=200, travel_time_min=3 }
         adj[0][1] → { to_index=9, distance_m=150, travel_time_min=2 }
         neighbour_count[0] = 3   (L01 connects to 3 locations)
       ────────────────────────────────────────────────────────────── */
    Neighbour adj[MAX_LOCATIONS][MAX_NEIGHBOURS]; /* Adjacency list     */
    int       neighbour_count[MAX_LOCATIONS];     /* Neighbours per loc */

} Graph;

/* ── End of Include Guard ───────────────────────────────────
   Matches the #ifndef at the top. Everything between
   #ifndef and #endif is protected from double-inclusion.  */
  #endif /* ROUTE_FINDER_H */

