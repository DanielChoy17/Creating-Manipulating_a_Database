CREATE OR REPLACE FUNCTION
determineSpeedingViolationsAndFinesFunction(maxFineTotal INTEGER)
RETURNS INTEGER AS $$

    DECLARE
        fineTotal INTEGER;                       /* Total fines assessed, the return value */
        currentOwnerState CHAR(2);               /* State for current owner */
        currentOwnerLicenseID CHAR(8);           /* LicenseID for current owner */
        currentViolationCount INTEGER;           /* Violation count for current owner */
        currentFineIncreasePerViolation INTEGER; /* Potential fine increase per violation for current owner */
    
    -- Cursor that finds the number of violations for each owner
    DECLARE ownerViolationCursor CURSOR FOR
        SELECT v.ownerState, v.ownerLicenseID, COUNT(*)
        FROM Highways h, Vehicles v, DistancesAndIntervalsForPhotos p
        WHERE h.highwayNum = p.highwayNum
            AND h.speedLimit < (p.distBetweenCameraMileMarkers / p.photoIntervalInHours)
            AND v.vehicleState = p.vehicleState
            AND v.vehicleLicensePlate = p.vehicleLicensePlate
        GROUP BY v.ownerState, v.ownerLicenseID
        HAVING COUNT(*) > 0
        ORDER BY COUNT(*) DESC;

    BEGIN

    -- Input Validation
    IF maxFineTotal <= 0
        THEN RETURN -1;
        END IF;

    fineTotal := 0;

    -- Updating the speedingViolations field for the owners and reseting their fine to 0
    OPEN ownerViolationCursor;

    LOOP
        FETCH ownerViolationCursor INTO currentOwnerState, currentOwnerLicenseID, currentViolationCount;
            -- EXIT loop if no results
            EXIT WHEN NOT FOUND;

            -- Updating the speedingViolations field for the current owner and reseting their fine to 0
            UPDATE Owners o
            SET speedingViolations = currentViolationCount, fine = 0
            WHERE o.ownerState = currentOwnerState
                AND o.ownerLicenseID = currentOwnerLicenseID;
    END LOOP;
    CLOSE ownerViolationCursor;

    -- Updating the fine field for the owners where necessary
    OPEN ownerViolationCursor;

    LOOP
        FETCH ownerViolationCursor INTO currentOwnerState, currentOwnerLicenseID, currentViolationCount;
            -- EXIT loop if no results
            EXIT WHEN NOT FOUND;
            
            -- Set the value of currentFineIncreasePerViolation based on currentViolationCount
            IF currentViolationCount >= 3
                THEN currentFineIncreasePerViolation := 50;
            ELSEIF currentViolationCount = 2
                THEN currentFineIncreasePerViolation := 20;
            ELSEIF currentViolationCount = 1
                THEN currentFineIncreasePerViolation := 10;
            ELSE currentFineIncreasePerViolation := 0; /* Should be impossible to reach here */
            END IF;
            
            -- Skip current iteration of loop (current owner) if fine increase would go beyond maxFineTotal
            CONTINUE WHEN fineTotal + (currentFineIncreasePerViolation * currentViolationCount) > maxFineTotal;

            -- Updating the fine field for the current owner
            UPDATE Owners o
            SET fine = currentFineIncreasePerViolation * currentViolationCount
            WHERE o.ownerState = currentOwnerState
                AND o.ownerLicenseID = currentOwnerLicenseID;

            -- Updating fineTotal
            fineTotal = fineTotal + (currentFineIncreasePerViolation * currentViolationCount);
    END LOOP;
    CLOSE ownerViolationCursor;

    RETURN fineTotal;

    END
$$ LANGUAGE plpgsql;