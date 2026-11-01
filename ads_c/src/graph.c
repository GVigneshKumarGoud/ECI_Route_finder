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
   FUNCTION: main
   ============================================================
   PURPOSE : The entry point of the program.
             Creates a Graph, loads both CSV files into it,
             then prints the data to verify everything loaded.
   ============================================================ */
int main() {

    /* Declare one Graph variable — this is our campus map  */
    Graph campus;

    /* Set the counters to zero before loading anything     */
    campus.location_count = 0;
    campus.road_count     = 0;

    printf("\n=== ECI Route Finder — Data Loader ===\n\n");

    /* ── Step 1: Load locations ── */
    int ok1 = load_locations(&campus, "data/locations.csv");

    /* ── Step 2: Load roads ────── */
    int ok2 = load_roads(&campus, "data/routes.csv");

    /* If either file failed to load, stop the program      */
    if (!ok1 || !ok2) {
        printf("\nERROR: Could not load all data. Exiting.\n");
        return 1; /* Non-zero return = program ended with an error */
    }

    /* ── Step 3: Display what was loaded ── */
    print_locations(&campus);
    print_roads(&campus);

    printf("Data loaded successfully. Graph is ready.\n\n");

    return 0; /* Zero return = program finished successfully   */
}

