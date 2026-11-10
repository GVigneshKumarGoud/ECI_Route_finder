/* ============================================================
   dijkstra.c
   ECI Route Finder — Adjacency List Builder & Shortest Path
   ============================================================
   This file contains:
     Step B : get_location_index()    — convert "L03" → 2
              build_adjacency_list()  — fill adj[][] from roads[]
              print_adjacency_list()  — display for verification
     Step D : dijkstra()              — shortest path algorithm
     Step E : print_path()            — trace and display the route

   It uses the data structures defined in:
     ads_c/include/route_finder.h
   ============================================================ */

#include "../include/route_finder.h"
#include <limits.h>   /* needed for INT_MAX — represents "infinity" */

/* ============================================================
   FUNCTION: get_location_index
   ============================================================
   PURPOSE : Convert a location ID string into an integer index
             so we can use it to access arrays by position.

   HOW IT WORKS:
     Location IDs are "L01", "L02", ... "L10".
     - location_id + 1  →  skips the 'L', gives "01", "03" etc.
     - atoi(...)        →  converts "01" to integer 1
     - subtract 1       →  converts to 0-based index (0 to 9)

   EXAMPLES:
     "L01" → 0      "L05" → 4      "L10" → 9

   RECEIVES: location_id — a string like "L03"
   RETURNS : integer index 0 to 9, or -1 if the ID is invalid
   ============================================================ */
int get_location_index(const char *location_id) {

    /* Basic safety check: ID must start with 'L'              */
    if (location_id == NULL || location_id[0] != 'L') {
        return -1; /* -1 means "not found" or "invalid"        */
    }

    /* atoi converts a number-string to an integer.
       location_id + 1 moves past the 'L' character.          */
    int number = atoi(location_id + 1);

    /* Valid IDs are L01 to L10, so numbers must be 1 to 10   */
    if (number < 1 || number > MAX_LOCATIONS) {
        return -1;
    }

    return number - 1; /* Convert 1-based to 0-based index     */
}

/* ============================================================
   FUNCTION: build_adjacency_list
   ============================================================
   PURPOSE : Loop through every road in the Graph and fill in
             the adj[][] table so each location knows its
             direct neighbours and the cost to reach them.

   Because every road is BIDIRECTIONAL, each road R from A→B
   creates TWO entries:
     • adj[A] gets a neighbour entry pointing to B
     • adj[B] gets a neighbour entry pointing to A

   RECEIVES: A pointer to the Graph (already loaded with roads)
   RETURNS : nothing (void) — it modifies the Graph in place
   ============================================================ */
void build_adjacency_list(Graph *g) {

    int i;

    /* ── Step 1: Reset all neighbour counts to zero ── */
    for (i = 0; i < MAX_LOCATIONS; i++) {
        g->neighbour_count[i] = 0;
    }

    /* ── Step 2: Loop through every road ── */
    for (i = 0; i < g->road_count; i++) {

        /* Get the integer index for the two ends of this road */
        int from = get_location_index(g->roads[i].from_id);
        int to   = get_location_index(g->roads[i].to_id);

        /* Skip this road if either location ID was invalid    */
        if (from == -1 || to == -1) {
            printf("WARNING: Skipping road %s — invalid location ID.\n",
                   g->roads[i].road_id);
            continue;
        }

        /* Get the two cost values from this road              */
        int dist        = g->roads[i].distance_m;
        int travel_time = g->roads[i].travel_time_min;

        /* ── Direction 1: from → to ── */
        /* Safety check: don't exceed the neighbour array size */
        if (g->neighbour_count[from] < MAX_NEIGHBOURS) {

            int slot = g->neighbour_count[from]; /* next free slot */

            g->adj[from][slot].to_index        = to;
            g->adj[from][slot].distance_m      = dist;
            g->adj[from][slot].travel_time_min = travel_time;

            g->neighbour_count[from]++;          /* one more neighbour */
        }

        /* ── Direction 2: to → from (bidirectional!) ── */
        if (g->neighbour_count[to] < MAX_NEIGHBOURS) {

            int slot = g->neighbour_count[to];

            g->adj[to][slot].to_index        = from;
            g->adj[to][slot].distance_m      = dist;
            g->adj[to][slot].travel_time_min = travel_time;

            g->neighbour_count[to]++;
        }
    }

    printf("Adjacency list built for %d location(s).\n", g->location_count);
}

/* ============================================================
   FUNCTION: print_adjacency_list
   ============================================================
   PURPOSE : Display the adjacency list in a readable format
             so you can verify it was built correctly.

   For each location, it lists:
     - The location's own name and ID
     - Each direct neighbour with distance and travel time

   RECEIVES: A pointer to the Graph (already built)
   ============================================================ */
void print_adjacency_list(const Graph *g) {

    int i, j;

    printf("\n");
    printf("============================================\n");
    printf("  ADJACENCY LIST (Campus Road Network)\n");
    printf("============================================\n");

    for (i = 0; i < g->location_count; i++) {

        /* Print the current location's ID and name           */
        printf("\n[%s] %s (%d neighbour(s)):\n",
               g->locations[i].location_id,
               g->locations[i].location_name,
               g->neighbour_count[i]);

        /* Print each neighbour of this location              */
        for (j = 0; j < g->neighbour_count[i]; j++) {

            int nb_index = g->adj[i][j].to_index;

            printf("   → %-20s | %4d m | %2d min\n",
                   g->locations[nb_index].location_name,
                   g->adj[i][j].distance_m,
                   g->adj[i][j].travel_time_min);
        }
    }

    printf("\n");
}

/* ============================================================
   FUNCTION: print_path
   ============================================================
   PURPOSE : After dijkstra() fills the parent[] array, this
             function traces the path BACKWARDS from destination
             to source, then prints it forwards.

             parent[i] tells us: "to reach location i, we came
             from location parent[i]". We follow that chain
             until we reach the source (where parent = -1).

   RECEIVES: - g         : the Graph (for location names)
             - parent[]  : filled in by dijkstra()
             - src       : source index
             - dest      : destination index
             - total_cost: the final distance or travel time
             - mode      : 1 = distance mode, 2 = time mode
   ============================================================ */
void print_path(const Graph *g, int parent[], int src, int dest,
                int total_cost, int mode) {

    /* We trace the path by following parent[] links.
       We store the path in a temporary array, then print it
       in the correct (forward) order.                        */
    int path[MAX_LOCATIONS];
    int path_len = 0;
    int current  = dest;

    /* Walk backwards: dest → ... → src                      */
    while (current != -1) {
        path[path_len] = current;
        path_len++;
        current = parent[current];
    }

    /* Now path[] is filled backwards, e.g. [dest, ..., src] */
    printf("\n============================================\n");
    if (mode == 1) {
        printf("  SHORTEST ROUTE  (by Distance)\n");
    } else {
        printf("  FASTEST ROUTE   (by Travel Time)\n");
    }
    printf("============================================\n");
    printf("  From : %s\n", g->locations[src].location_name);
    printf("  To   : %s\n", g->locations[dest].location_name);
    printf("--------------------------------------------\n");

    /* Print path forwards: reverse the backwards array      */
    int step;
    for (step = path_len - 1; step >= 0; step--) {
        int idx = path[step];
        if (step == path_len - 1) {
            /* First location in the route — just print name  */
            printf("  START  %-22s\n", g->locations[idx].location_name);
        } else {
            printf("  →      %-22s\n", g->locations[idx].location_name);
        }
    }

    printf("--------------------------------------------\n");
    if (mode == 1) {
        printf("  Total Distance  : %d metres\n", total_cost);
    } else {
        printf("  Total Time      : %d minutes\n", total_cost);
    }
    printf("============================================\n\n");
}

/* ============================================================
   FUNCTION: dijkstra
   ============================================================
   PURPOSE : Find the shortest path from src to dest in the
             campus road network using Dijkstra's algorithm.

   HOW IT WORKS (plain English):
     1. Mark every location's distance as "infinity" (INT_MAX).
     2. Set the source location's distance to 0.
     3. Repeat until all locations are visited:
          a. Pick the UNVISITED location with the SMALLEST
             known distance so far — call it "current".
          b. For each neighbour of current:
               new_cost = dist[current] + road_cost
               If new_cost < dist[neighbour]:
                   Update dist[neighbour] = new_cost
                   Record parent[neighbour] = current
          c. Mark current as visited.
     4. When done, dist[dest] holds the shortest cost and
        parent[] lets us trace back the full path.

   RECEIVES:
     - g    : pointer to the loaded Graph
     - src  : source      location index (0–9)
     - dest : destination location index (0–9)
     - mode : 1 = use distance_m,  2 = use travel_time_min

   RETURNS : 1 if a route was found, 0 if no route exists
   ============================================================ */
int dijkstra(const Graph *g, int src, int dest, int mode) {

    /* ── Validate inputs ──────────────────────────────────── */
    if (src < 0 || src >= g->location_count) {
        printf("ERROR: Invalid source index %d.\n", src);
        return 0;
    }
    if (dest < 0 || dest >= g->location_count) {
        printf("ERROR: Invalid destination index %d.\n", dest);
        return 0;
    }
    if (src == dest) {
        printf("Source and destination are the same location.\n");
        return 0;
    }

    /* ── Working arrays ───────────────────────────────────── */

    /* dist[i]    = shortest known cost to reach location i   */
    int dist[MAX_LOCATIONS];

    /* visited[i] = 1 if location i has been finalised        */
    int visited[MAX_LOCATIONS];

    /* parent[i]  = which location we came FROM to reach i.
                    -1 means "no parent yet" (unvisited).     */
    int parent[MAX_LOCATIONS];

    int i;

    /* Initialise all arrays                                  */
    for (i = 0; i < g->location_count; i++) {
        dist[i]    = INT_MAX; /* "infinity" — not yet reached */
        visited[i] = 0;       /* nobody visited yet           */
        parent[i]  = -1;      /* no parent yet                */
    }

    /* Cost to reach source from itself is always 0           */
    dist[src] = 0;

    /* ── Main Dijkstra loop ───────────────────────────────── */
    /* We iterate location_count times — once per location    */
    int iter;
    for (iter = 0; iter < g->location_count; iter++) {

        /* ── Step A: Pick the unvisited location with the
                      smallest known distance ── */
        int current   = -1;
        int min_cost  = INT_MAX;

        for (i = 0; i < g->location_count; i++) {
            if (!visited[i] && dist[i] < min_cost) {
                min_cost = dist[i];
                current  = i;
            }
        }

        /* If current is still -1, all remaining locations are
           unreachable — stop early                           */
        if (current == -1) {
            break;
        }

        /* Mark this location as finalised                    */
        visited[current] = 1;

        /* If we just finalised the destination, we are done  */
        if (current == dest) {
            break;
        }

        /* ── Step B: Relax all neighbours of current ── */
        int j;
        for (j = 0; j < g->neighbour_count[current]; j++) {

            int nb   = g->adj[current][j].to_index;

            /* Choose the cost field based on the mode        */
            int cost;
            if (mode == 1) {
                cost = g->adj[current][j].distance_m;
            } else {
                cost = g->adj[current][j].travel_time_min;
            }

            /* Only update if we found a shorter path AND
               the current location was actually reachable    */
            if (!visited[nb] && dist[current] != INT_MAX) {

                int new_cost = dist[current] + cost;

                if (new_cost < dist[nb]) {
                    dist[nb]   = new_cost;  /* better route found */
                    parent[nb] = current;   /* record how we got here */
                }
            }
        }
    }

    /* ── Check result and print ───────────────────────────── */
    if (dist[dest] == INT_MAX) {
        /* Destination was never reached                      */
        printf("\nNo route found between %s and %s.\n",
               g->locations[src].location_name,
               g->locations[dest].location_name);
        return 0;
    }

    /* Route found — print it using print_path()              */
    print_path(g, parent, src, dest, dist[dest], mode);
    return 1;
}

