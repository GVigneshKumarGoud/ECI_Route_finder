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

/* Maximum number of locations (nodes) in the campus map */
#define MAX_LOCATIONS 10

/* Maximum number of roads (edges) in the campus map */
#define MAX_ROADS 17

/* Maximum number of characters allowed in a text field */
#define MAX_NAME_LEN 50

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

} Graph;

/* ── End of Include Guard ───────────────────────────────────
   Matches the #ifndef at the top. Everything between
   #ifndef and #endif is protected from double-inclusion.  */
#endif /* ROUTE_FINDER_H */

