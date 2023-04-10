#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "libpq-fe.h"

/* Maximum length of string used to submit a connection */
#define MAXCONNECTIONSTRINGSIZE 501
/* Maximum length of string used to submit a SQL statement */
#define MAXSQLSTATEMENTSTRINGSIZE 2001
/* Maximum length of string version of integer */
#define  MAXNUMBERSTRINGSIZE        20


/* Exit with success after closing connection to the server
 *  and freeing memory that was used by the PGconn object.
 */
static void good_exit(PGconn *conn)
{
    PQfinish(conn);
    exit(EXIT_SUCCESS);
}

/* Exit with failure after closing connection to the server
 *  and freeing memory that was used by the PGconn object.
 */
static void bad_exit(PGconn *conn)
{
    PQfinish(conn);
    exit(EXIT_FAILURE);
}

/* The three C functions appears below. */

/* Function: printCameraPhotoCount:
 * -------------------------------------
 * Parameters:  connection, and theCameraID, which should be the ID of a camera.
 * Prints the cameraID, the highwayNum and mileMarker of that camera, and the
 * number of number of photos for that camera, if camera exists for thecameraID.
 * Return 0 if normal execution, -1 if no such camera.
 * bad_exit if SQL statement execution fails.
 */


int printCameraPhotoCount(PGconn *conn, int theCameraID)
{
    // Beginning Serializable Transaction
    PGresult *res_begin = PQexec(conn, "BEGIN TRANSACTION ISOLATION LEVEL SERIALIZABLE");
    if (PQresultStatus(res_begin) != PGRES_COMMAND_OK) {
        fprintf(stderr, "BEGIN TRANSACTION failed: %s\n", PQerrorMessage(conn));
        PQclear(res_begin);
        bad_exit(conn);
        return(-1);
    }
    PQclear(res_begin);

    // Getting the highwayNum and mileMarker of the camera whose cameraID is theCameraID
    char stringCameraID[MAXNUMBERSTRINGSIZE];
    sprintf(stringCameraID, "%d", theCameraID);

    char camera_sql[MAXSQLSTATEMENTSTRINGSIZE] = "SELECT c.highwayNum, c.mileMarker FROM Cameras c WHERE c.cameraID = ";
    strcat(camera_sql, stringCameraID);

    PGresult *res_camera = PQexec(conn, camera_sql);

    // Checking that the SQL statement worked 
    if (PQresultStatus(res_camera) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT for CameraID failed: %s, %s\n", camera_sql, PQerrorMessage(conn));
        PQclear(res_camera);
        bad_exit(conn);
        return(-1);
    }

    // If the number of tuples returned is 0 (no cameraID is theCameraID), return -1
    int t = PQntuples(res_camera);
    if (t == 0) {
        PQclear(res_camera);
        PGresult *res_rollback = PQexec(conn, "ROLLBACK TRANSACTION");
        if (PQresultStatus(res_rollback) != PGRES_COMMAND_OK) {
            fprintf(stderr, "ROLLBACK TRANSACTION failed: %s\n", PQerrorMessage(conn));
            PQclear(res_rollback);
            bad_exit(conn);
            return(-1);
        }
        PQclear(res_rollback);

        return(-1);
    }

    // Extracting the highwayNum and mileMarker of the camera
    char *theHighwayNum;
    theHighwayNum = PQgetvalue(res_camera, 0, 0);
    
    char *theMileMarker;
    theMileMarker = PQgetvalue(res_camera, 0, 1);

    PQclear(res_camera);

    // Getting the number of photos taken by the camera
    char photosCount_sql[MAXSQLSTATEMENTSTRINGSIZE] = "SELECT COUNT(*) FROM Photos p WHERE p.cameraID = ";
    strcat(photosCount_sql, stringCameraID);

    PGresult *res_photos = PQexec(conn, photosCount_sql);

    // Checking that the SQL statement worked
    if (PQresultStatus(res_photos) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT for photos count failed: %s, %s\n", photosCount_sql, PQerrorMessage(conn));
        PQclear(res_photos);
        bad_exit(conn);
        return(-1);
    }

    // Printing the output for this function
    int photosCount = atoi(PQgetvalue(res_photos, 0, 0));
    PQclear(res_photos);

    printf("Camera %d, on %s at %s has taken %d photos\n", theCameraID, theHighwayNum, theMileMarker, photosCount);

    // Committing Transaction
    PGresult *res_commit = PQexec(conn, "COMMIT TRANSACTION");
    if (PQresultStatus(res_commit) != PGRES_COMMAND_OK) {
        fprintf(stderr, "COMMIT TRANSACTION failed: %s\n", PQerrorMessage(conn));
        PQclear(res_commit);
        bad_exit(conn);
        return(-1);
    }
    PQclear(res_commit);

    return(0);
}

/* Function: openAllExits:
 * ----------------------------
 * Parameters:  connection, and theHighwayNum, the number of a highway.
 
 * Opens all the exit on that highway that weren't already open.
 * Returns the number of exits on the highway that weren't open,
 * or -1 if there is no highway whose highwayNum is theHighwayNum.
 */

int openAllExits(PGconn *conn, int theHighwayNum)
{
    // Beginning Serializable Transaction
    PGresult *res_begin = PQexec(conn, "BEGIN TRANSACTION ISOLATION LEVEL SERIALIZABLE");
    if (PQresultStatus(res_begin) != PGRES_COMMAND_OK) {
        fprintf(stderr, "BEGIN TRANSACTION failed: %s\n", PQerrorMessage(conn));
        PQclear(res_begin);
        bad_exit(conn);
        return(-1);
    }
    PQclear(res_begin);

    // Checking to see if there is a highway in Highways whose highwayNum is theHighwayNum
    char stringHighwayNum[MAXNUMBERSTRINGSIZE];
    sprintf(stringHighwayNum, "%d", theHighwayNum);

    char check_sql[MAXSQLSTATEMENTSTRINGSIZE] = "SELECT h.highwayNum FROM Highways h WHERE h.highwayNum = ";
    strcat(check_sql, stringHighwayNum);

    PGresult *res_check = PQexec(conn, check_sql);
    
    // Checking that the SQL statement worked 
    if (PQresultStatus(res_check) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT for highwayNum failed: %s, %s\n", check_sql, PQerrorMessage(conn));
        PQclear(res_check);
        bad_exit(conn);
        return(-1);
    }

    // If the number of tuples returned is 0 (no highwayNum is theHighwayNum), return -1
    int t = PQntuples(res_check);
    if (t == 0) {
        PQclear(res_check);
        PGresult *res_rollback = PQexec(conn, "ROLLBACK TRANSACTION");
        if (PQresultStatus(res_rollback) != PGRES_COMMAND_OK) {
            fprintf(stderr, "ROLLBACK TRANSACTION failed: %s\n", PQerrorMessage(conn));
            PQclear(res_rollback);
            bad_exit(conn);
            return(-1);
        }
        PQclear(res_rollback);

        return(-1);
    }
    PQclear(res_check);

    // Updating all exits on theHighwayNum which aren't open to have their isExitOpen value to TRUE (exit is open)
    char exits_sql[MAXSQLSTATEMENTSTRINGSIZE] = "UPDATE Exits e SET isExitOpen = TRUE WHERE e.isExitOpen <> TRUE AND e.highwayNum = ";
    strcat(exits_sql, stringHighwayNum);

    PGresult *res_exits = PQexec(conn, exits_sql);

    // Checking that the SQL statement worked
    if (PQresultStatus(res_exits) != PGRES_COMMAND_OK) {
        fprintf(stderr, "UPDATE failed: %s, %s\n", exits_sql, PQerrorMessage(conn));
        PQclear(res_exits);
        bad_exit(conn);
        return(-1);
    }

    // Getting the number of exits whose isExitOpen value was changed
    int result = atoi(PQcmdTuples(res_exits));
    PQclear(res_exits); 

    // Committing Transaction
    PGresult *res_commit = PQexec(conn, "COMMIT TRANSACTION");
    if (PQresultStatus(res_commit) != PGRES_COMMAND_OK) {
        fprintf(stderr, "COMMIT TRANSACTION failed: %s\n", PQerrorMessage(conn));
        PQclear(res_commit);
        bad_exit(conn);
        return(-1);
    }
    PQclear(res_commit);

    return(result);
}

/* Function: determineSpeedingViolationsAndFines:
 * -------------------------------
 * Parameters:  connection, and an integer maxFineTotal, the maximum total
 * of the fines that should be assessed to owners whose vehicles were speeding.
 *
 * Counts the number of speeding violations by vehicles that each owner
 * owns, and sets the speedingViolations field of Owners accordingly.
 *
 * Assess fines to some owners based on the number of speeding
 * violations they have.
 *
 * Executes by invoking a Stored Function,
 * determineSpeedingViolationsAndFinesFunction, which does all of the work.
 *
 * Returns a negative value if there is an error, and otherwise returns the total
 * fines that were assessed by the Stored Function.
 */

int determineSpeedingViolationsAndFines(PGconn *conn, int maxFineTotal)
{
    // Calling the Stored Function "determineSpeedingViolationsAndFinesFunction"
    char stringMaxFineTotal[MAXNUMBERSTRINGSIZE];
    sprintf(stringMaxFineTotal, "%d", maxFineTotal);

    char function_sql[MAXSQLSTATEMENTSTRINGSIZE] = "SELECT determineSpeedingViolationsAndFinesFunction(";
    strcat(function_sql, stringMaxFineTotal);
    strcat(function_sql, ")");

    PGresult *total_fines = PQexec(conn, function_sql);

    // Checking that the function call worked
    if (PQresultStatus(total_fines) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Function determineSpeedingViolationsAndFinesFunction failed on %d: %s\n", maxFineTotal, PQerrorMessage(conn));
        PQclear(total_fines);
        bad_exit(conn);
    }

    // Returning the total fines that were assessed by the Stored Function
    int result = atoi(PQgetvalue(total_fines, 0, 0));

    PQclear(total_fines);
    
    return(result);
}

int main(int argc, char **argv)
{
    PGconn *conn;

    if (argc != 3)
    {
        fprintf(stderr, "Usage: ./runHighwayApplication <username> <password>\n");
        exit(EXIT_FAILURE);
    }

    char *userID = argv[1];
    char *pwd = argv[2];

    char conninfo[MAXCONNECTIONSTRINGSIZE] = "host=cse180-db.lt.ucsc.edu user=";
    strcat(conninfo, userID);
    strcat(conninfo, " password=");
    strcat(conninfo, pwd);

    /* Make a connection to the database */
    conn = PQconnectdb(conninfo);

    /* Check to see if the database connection was successfully made. */
    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connection to database failed: %s\n",
                PQerrorMessage(conn));
        bad_exit(conn);
    }

    int result;
    
    /* Performs calls to printCameraPhotoCount,
     * printing an error message if there's an error.
     */
    int theCameraID;

    theCameraID = 951;
    result = printCameraPhotoCount(conn, theCameraID);
    if (result == -1) {
        printf("No camera exists whose id is %d\n", theCameraID);
    } else if (result != 0) {
        printf("Error: Value returned by printCameraPhotoCount for theCameraID %d is %d\n", theCameraID, result);
        bad_exit(conn);
    }

    theCameraID = 960;
    result = printCameraPhotoCount(conn, theCameraID);
    if (result == -1) {
        printf("No camera exists whose id is %d\n", theCameraID);
    } else if (result != 0) {
        printf("Error: Value returned by printCameraPhotoCount for theCameraID %d is %d\n", theCameraID, result);
        bad_exit(conn);
    }

    theCameraID = 856;
    result = printCameraPhotoCount(conn, theCameraID);
    if (result == -1) {
        printf("No camera exists whose id is %d\n", theCameraID);
    } else if (result != 0) {
        printf("Error: Value returned by printCameraPhotoCount for theCameraID %d is %d\n", theCameraID, result);
        bad_exit(conn);
    }

    theCameraID = 904;
    result = printCameraPhotoCount(conn, theCameraID);
    if (result == -1) {
        printf("No camera exists whose id is %d\n", theCameraID);
    } else if (result != 0) {
        printf("Error: Value returned by printCameraPhotoCount for theCameraID %d is %d\n", theCameraID, result);
        bad_exit(conn);
    }

    /* Extra newline for readability */
    printf("\n");

    
    /* Performs calls to openAllExits,
     * and prints certain messages according to the results.
     */
    int theHighwayNum;

    theHighwayNum = 101;
    result = openAllExits(conn, theHighwayNum);
    if (result >= 0) {
        printf("%d exits were opened by openAllExits\n", result);
    } else if (result == -1) {
        printf("There is no highway whose number is %d\n", theHighwayNum);
    } else {
        printf("Error: Value returned by openAllExits for theHighwayNum %d is %d\n", theHighwayNum, result);
        bad_exit(conn);
    }

    theHighwayNum = 13;
    result = openAllExits(conn, theHighwayNum);
    if (result >= 0) {
        printf("%d exits were opened by openAllExits\n", result);
    } else if (result == -1) {
        printf("There is no highway whose number is %d\n", theHighwayNum);
    } else {
        printf("Error: Value returned by openAllExits for theHighwayNum %d is %d\n", theHighwayNum, result);
        bad_exit(conn);
    }

    theHighwayNum = 280;
    result = openAllExits(conn, theHighwayNum);
    if (result >= 0) {
        printf("%d exits were opened by openAllExits\n", result);
    } else if (result == -1) {
        printf("There is no highway whose number is %d\n", theHighwayNum);
    } else {
        printf("Error: Value returned by openAllExits for theHighwayNum %d is %d\n", theHighwayNum, result);
        bad_exit(conn);
    }

    theHighwayNum = 17;
    result = openAllExits(conn, theHighwayNum);
    if (result >= 0) {
        printf("%d exits were opened by openAllExits\n", result);
    } else if (result == -1) {
        printf("There is no highway whose number is %d\n", theHighwayNum);
    } else {
        printf("Error: Value returned by openAllExits for theHighwayNum %d is %d\n", theHighwayNum, result);
        bad_exit(conn);
    }

    /* Extra newline for readability */
    printf("\n");

    
    /* Performs calls to determineSpeedingViolationsAndFines, 
     * and prints certain messages according to the results.
     */
    int maxFineTotal;

    maxFineTotal = 300;
    result = determineSpeedingViolationsAndFines(conn, maxFineTotal);
    if (result >= 0) {
        printf("Total fines for maxFineTotal %d is %d\n", maxFineTotal, result);
    } else {
        printf("Error: Value returned by determineSpeedingViolationsAndFines for maxFineTotal %d is %d\n", maxFineTotal, result);
        bad_exit(conn);
    }

    maxFineTotal = 240;
    result = determineSpeedingViolationsAndFines(conn, maxFineTotal);
    if (result >= 0) {
        printf("Total fines for maxFineTotal %d is %d\n", maxFineTotal, result);
    } else {
        printf("Error: Value returned by determineSpeedingViolationsAndFines for maxFineTotal %d is %d\n", maxFineTotal, result);
        bad_exit(conn);
    }

    maxFineTotal = 210;
    result = determineSpeedingViolationsAndFines(conn, maxFineTotal);
    if (result >= 0) {
        printf("Total fines for maxFineTotal %d is %d\n", maxFineTotal, result);
    } else {
        printf("Error: Value returned by determineSpeedingViolationsAndFines for maxFineTotal %d is %d\n", maxFineTotal, result);
        bad_exit(conn);
    }

    maxFineTotal = 165;
    result = determineSpeedingViolationsAndFines(conn, maxFineTotal);
    if (result >= 0) {
        printf("Total fines for maxFineTotal %d is %d\n", maxFineTotal, result);
    } else {
        printf("Error: Value returned by determineSpeedingViolationsAndFines for maxFineTotal %d is %d\n", maxFineTotal, result);
        bad_exit(conn);
    }

    good_exit(conn);
    return 0;
}
