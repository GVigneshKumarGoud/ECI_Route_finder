/* ============================================================
   graph.c
   ECI Route Finder — Graph Data Loading & Display
   ============================================================
   This file contains functions to:
     1. Load locations from  data/locations.csv  into a Graph
     2. Load roads     from  data/routes.csv     into a Graph
     3. Print the loaded data so you can verify it is correct

   It uses the data structures defined in:
     ads_c/include/route_finder.h
   ============================================================ */

#include "../include/route_finder.h"

/* ============================================================
   FORWARD DECLARATIONS
   ============================================================
   These tell the compiler that the functions below exist in
   dijkstra.c. Without these lines, the compiler would see
   calls to dijkstra() etc. in main() and complain that it
   does not know what they are.
   ============================================================ */
int  get_location_index   (const char *location_id);
void build_adjacency_list (Graph *g);
void print_adjacency_list (const Graph *g);
void print_path           (const Graph *g, int parent[], int src,
                           int dest, int total_cost, int mode);
int  dijkstra             (const Graph *g, int src, int dest, int mode);

/* ============================================================
   FUNCTION: load_locations
   ============================================================
   PURPOSE : Open locations.csv and read every row into the
             Graph's locations[] array.
   RECEIVES: - A pointer to a Graph variable  (to fill in)
             - The file path to locations.csv (as text)
   RETURNS : 1 if loading succeeded, 0 if something went wrong
   ============================================================ */
int load_locations(Graph *g, const char *filepath) {

    /* fopen opens a file for reading ("r" = read mode).
       If the file is not found, it returns NULL.          */
    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) {
        printf("ERROR: Could not open file: %s\n", filepath);
        return 0; /* 0 means failure */
    }

    /* Buffer = a temporary holding area for one line of text.
       256 characters is more than enough for our CSV rows.  */
    char line[256];

    /* Skip the first line — it is the header row:
       "location_id,location_name,location_type"            */
    fgets(line, sizeof(line), fp);

    /* Reset the counter before we start loading            */
    g->location_count = 0;

    /* Read each remaining line until the file ends         */
    while (fgets(line, sizeof(line), fp)) {
if (line[0] == '\n' || line[0] == '\r')
    continue;
        /* Safety check: stop if the array is already full  */
        if (g->location_count >= MAX_LOCATIONS) {
            printf("WARNING: More locations in CSV than MAX_LOCATIONS.\n");
            break;
        }

        /* We will fill this shortcut variable for clarity  */
        Location *loc = &g->locations[g->location_count];

        /* sscanf reads formatted data from a string.
           %[^,] means "read characters until you hit a comma".
           This lets us split the line by commas.           */
        sscanf(line, "%9[^,],%49[^,],%49[^\n]",
               loc->location_id,
               loc->location_name,
               loc->location_type);

        g->location_count++; /* Move to the next slot       */
    }

    fclose(fp); /* Always close the file when done          */

    printf("Loaded %d location(s) from: %s\n", g->location_count, filepath);
    return 1; /* 1 means success                            */
}

/* ============================================================
   FUNCTION: load_roads
   ============================================================
   PURPOSE : Open routes.csv and read every row into the
             Graph's roads[] array.
   RECEIVES: - A pointer to a Graph variable  (to fill in)
             - The file path to routes.csv    (as text)
   RETURNS : 1 if loading succeeded, 0 if something went wrong
   ============================================================ */
int load_roads(Graph *g, const char *filepath) {

    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) {
        printf("ERROR: Could not open file: %s\n", filepath);
        return 0;
    }

    char line[256];

    /* Skip the header row:
       "road_id,from_location_id,to_location_id,distance_m,travel_time_min" */
    fgets(line, sizeof(line), fp);

    /* Reset the counter before we start loading            */
    g->road_count = 0;

    while (fgets(line, sizeof(line), fp)) {

        if (g->road_count >= MAX_ROADS) {
            printf("WARNING: More roads in CSV than MAX_ROADS.\n");
            break;
        }

        Road *road = &g->roads[g->road_count];

        /* %d reads an integer value from the string        */
        sscanf(line, "%9[^,],%9[^,],%9[^,],%d,%d",
               road->road_id,
               road->from_id,
               road->to_id,
               &road->distance_m,
               &road->travel_time_min);

        g->road_count++;
    }

    fclose(fp);

    printf("Loaded %d road(s) from: %s\n", g->road_count, filepath);
    return 1;
}

/* ============================================================
   FUNCTION: print_locations
   ============================================================
   PURPOSE : Display all loaded locations in a readable table.
   RECEIVES: A pointer to a Graph variable (to read from)
   ============================================================ */
void print_locations(const Graph *g) {

    printf("\n");
    printf("============================================\n");
    printf("  CAMPUS LOCATIONS (%d total)\n", g->location_count);
    printf("============================================\n");
    printf("%-6s  %-22s  %-20s\n", "ID", "Name", "Type");
    printf("------  ----------------------  --------------------\n");

    int i;
    for (i = 0; i < g->location_count; i++) {
        printf("%-6s  %-22s  %-20s\n",
               g->locations[i].location_id,
               g->locations[i].location_name,
               g->locations[i].location_type);
    }
    printf("\n");
}

/* ============================================================
   FUNCTION: print_roads
   ============================================================
   PURPOSE : Display all loaded roads in a readable table.
   RECEIVES: A pointer to a Graph variable (to read from)
   ============================================================ */
void print_roads(const Graph *g) {

    printf("============================================\n");
    printf("  CAMPUS ROADS (%d total)\n", g->road_count);
    printf("============================================\n");
    printf("%-6s  %-6s  %-6s  %-12s  %-16s\n",
           "RoadID", "From", "To", "Distance(m)", "TravelTime(min)");
    printf("------  ------  ------  ------------  ----------------\n");

    int i;
    for (i = 0; i < g->road_count; i++) {
        printf("%-6s  %-6s  %-6s  %-12d  %-16d\n",
               g->roads[i].road_id,
               g->roads[i].from_id,
               g->roads[i].to_id,
               g->roads[i].distance_m,
               g->roads[i].travel_time_min);
    }
    printf("\n");
}

/* ============================================================
   FUNCTION: show_location_menu
   ============================================================
   PURPOSE : Print a numbered list of all loaded locations so
             the user can pick one by typing a number.
   ============================================================ */
void show_location_menu(const Graph *g) {
    int i;
    printf("\n  %-4s  %-6s  %-24s  %-20s\n",
           "No.", "ID", "Name", "Type");
    printf("  ----  ------  ------------------------  --------------------\n");
    for (i = 0; i < g->location_count; i++) {
        printf("  %-4d  %-6s  %-24s  %-20s\n",
               i + 1,                              /* 1-based number      */
               g->locations[i].location_id,
               g->locations[i].location_name,
               g->locations[i].location_type);
    }
    printf("\n");
}

/* ============================================================
   FUNCTION: main
   ============================================================
   PURPOSE : Entry point of the ECI Route Finder program.
             Loads data, builds the graph, then runs an
             interactive menu so the user can find routes.
   ============================================================ */
int main() {

    /* ── 1. Create and initialise the campus graph ── */
    Graph campus;
    campus.location_count = 0;
    campus.road_count     = 0;

    printf("\n");
    printf("============================================\n");
    printf("   ECI ROUTE FINDER — Campus Navigator    \n");
    printf("============================================\n\n");

    /* ── 2. Load data from CSV files ── */
    int ok1 = load_locations(&campus, "data/locations.csv");
    int ok2 = load_roads    (&campus, "data/routes.csv");

    if (!ok1 || !ok2) {
        printf("\nERROR: Could not load data files. Exiting.\n");
        printf("Make sure you run this program from the ECI_Route_Finder folder.\n\n");
        return 1;
    }

    /* ── 3. Build the adjacency list from loaded roads ── */
    build_adjacency_list(&campus);

    /* ── 4. Interactive route-finder loop ── */
    char again = 'y';

    while (again == 'y' || again == 'Y') {

        printf("\n--------------------------------------------\n");
        printf("  FIND A ROUTE\n");
        printf("--------------------------------------------\n");

        /* Show the numbered location list */
        show_location_menu(&campus);

        /* ── Ask for source location ── */
        int src_num = 0;
        while (src_num < 1 || src_num > campus.location_count) {
            printf("  Enter START location number (1 to %d): ",
                   campus.location_count);
            scanf("%d", &src_num);
            if (src_num < 1 || src_num > campus.location_count) {
                printf("  Invalid choice. Please try again.\n");
            }
        }

        /* ── Ask for destination location ── */
        int dest_num = 0;
        while (dest_num < 1 || dest_num > campus.location_count
               || dest_num == src_num) {
            printf("  Enter END   location number (1 to %d): ",
                   campus.location_count);
            scanf("%d", &dest_num);
            if (dest_num < 1 || dest_num > campus.location_count) {
                printf("  Invalid choice. Please try again.\n");
            } else if (dest_num == src_num) {
                printf("  Start and end cannot be the same. Try again.\n");
            }
        }

        /* ── Ask for search mode ── */
        int mode = 0;
        while (mode != 1 && mode != 2) {
            printf("\n  Search by:\n");
            printf("    1 = Shortest Distance (metres)\n");
            printf("    2 = Fastest Time     (minutes)\n");
            printf("  Enter choice (1 or 2): ");
            scanf("%d", &mode);
            if (mode != 1 && mode != 2) {
                printf("  Invalid choice. Enter 1 or 2.\n");
            }
        }

        /* ── Convert 1-based user numbers to 0-based indices ── */
        int src  = src_num  - 1;
        int dest = dest_num - 1;

        /* ── Run Dijkstra's algorithm ── */
        dijkstra(&campus, src, dest, mode);

        /* ── Ask if the user wants to search again ── */
        printf("  Search again? (y / n): ");
        scanf(" %c", &again);   /* the space before %c skips whitespace */
    }

    printf("\nThank you for using ECI Route Finder. Goodbye!\n\n");
    return 0;
}

