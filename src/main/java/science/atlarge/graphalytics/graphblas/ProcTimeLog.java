package science.atlarge.graphalytics.graphblas;

import java.time.Instant;

/**
 * Responsible for logging the processing start and end time.
 */
public final class ProcTimeLog {
    static final String START_PROC_TIME = "Processing starts at";
    static final String END_PROC_TIME = "Processing ends at";

    /**
     * Logs the processing start time.
     */
    public static void start() {
        System.out.println(START_PROC_TIME + " " + Instant.now().toEpochMilli());
    }

    /**
     * Logs the processing end time.
     */
    public static void end() {
        System.out.println(END_PROC_TIME + " " + Instant.now().toEpochMilli());
    }
}