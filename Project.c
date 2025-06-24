#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PARKING_SPOTS 10 // Maximum number of parking slots
#define FILENAME "parking_records.txt" // File to store parking records
#define HOURLY_RATE 50.0 // Fee charged per hour for parking

// Structure to hold parking record details
typedef struct {
    int id; // Unique ID for the parking record
    char license_plate[15]; // Vehicle's license plate number
    char entry_time[20]; // Entry time of the vehicle
    char exit_time[20]; // Exit time of the vehicle
    float fee; // Fee charged for parking
} ParkingRecord;

// Global variables to track parking slots, revenue, and vehicle counts
int parking_slots[MAX_PARKING_SPOTS] = {0}; // 0 indicates available, 1 indicates occupied
float total_revenue = 0.0; // Total revenue generated from parking fees
int total_entries = 0; // Total number of vehicle entries
int total_exits = 0; // Total number of vehicle exits

// Function prototypes for various functionalities
void addParkingRecord(); // Adds a new parking record
void updateParkingRecord(); // Deletes a parking record based on ID
void deleteParkingRecord(); // Displays all current entries and exits
void trackEntriesAndExits(); // Calculates the fee based on entry and exit times
void calculateFees(ParkingRecord *record);// Displays all parking records from the file
void displayRecords(); // Displays available parking slots
void displayAvailableSlots(); // Generates a report of total revenue and vehicle counts
void generateRevenueReport(); 

int main() {
    int choice; // Variable to store user choice
    while (1) {
        // Display available parking slots before showing the menu
        displayAvailableSlots();

        printf("\n--- Parking Management System ---\n");
        printf("1. Add Parking Record\n");
        printf("2. Update Parking Record\n");
        printf("3. Delete Parking Record\n");
        printf("4. Track Entries and Exits\n");
        printf("5. Display All Records\n");
        printf("6. Generate Revenue Report\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        
        scanf("%d", &choice); // Get user choice

        switch (choice) {
            case 1: 
            addParkingRecord(); 
            break; 
            case 2:
            updateParkingRecord(); 
            break;
            case 3: 
            deleteParkingRecord(); 
            break; 
            case 4: 
            trackEntriesAndExits(); 
            break; 
            case 5: 
            displayRecords(); 
            break;
            case 6: 
            generateRevenueReport(); 
            break; 
            case 7: 
                printf("Thank you! Visit again!\n"); // Exit message before exiting program
                exit(0); // Exit the program
            default: printf("Invalid choice! Please try again.\n"); // Handle invalid input
        }
    }
    return 0;
}

void addParkingRecord() {
    FILE *file = fopen(FILENAME, "a"); // Open file in append mode
    if (!file) {
        perror("Unable to open file!"); // Error handling if file can't be opened
        return;
    }

    ParkingRecord record;

    // Find an available slot for parking
    int slot_found = -1;
    for (int i = 0; i < MAX_PARKING_SPOTS; i++) {
        if (parking_slots[i] == 0) { // Slot is available
            slot_found = i;
            parking_slots[i] = 1; // Mark as occupied
            break;
        }
    }

    if (slot_found == -1) { 
        printf("No available parking slots!\n"); // Inform user if no slots are available
        fclose(file);
        return;
    }

    printf("Enter License Plate: ");
    scanf("%s", record.license_plate); // Get license plate from user
    
    printf("Enter Entry Time (HH:MM): ");
    scanf("%s", record.entry_time); // Get entry time from user
    
    record.id = slot_found + 1; // Use slot index + 1 as ID for the record
    record.exit_time[0] = '\0'; // Initially no exit time recorded
    record.fee = 0.0; // Initial fee is zero

    fprintf(file, "%d %s %s %s %.2f\n", record.id, record.license_plate, record.entry_time, record.exit_time, record.fee);
    
    total_entries++; // Increment entry count when a new vehicle is added
    fclose(file);
    
    printf("Parking Record Added Successfully! Assigned Slot ID: %d\n", record.id); 
}

void updateParkingRecord() {
    FILE *file = fopen(FILENAME, "r+");
    if (!file) {
        perror("Unable to open file!");
        return;
    }

    int id;
    printf("Enter ID of the parking record to update: ");
    scanf("%d", &id);
    
    ParkingRecord record;
    int found = 0;
    long pos;

    while ((pos = ftell(file)) >= 0 && fscanf(file, "%d %s %s %s %f", 
           &record.id, record.license_plate, record.entry_time, 
           record.exit_time, &record.fee) == 5) {
        if (record.id == id) {
            found = 1;
            printf("Enter Exit Time (HH:MM): ");
            scanf("%s", record.exit_time);
            calculateFees(&record);
            fseek(file, pos, SEEK_SET); // Move back to where we read from
            fprintf(file, "%d %s %s %s %.2f\n", 
                    record.id, record.license_plate,
                    record.entry_time, record.exit_time,
                    record.fee);
            total_revenue += record.fee;
            total_exits++;
            parking_slots[id - 1] = 0; // Mark slot as available
            break;
        }
    }

    if (!found) {
        printf("Record with ID %d not found!\n", id);
    } else {
        printf("Parking Record Updated Successfully! Fee: %.2f\n", record.fee);
        printf("Slot ID %d is now available.\n", id);
    }

    fclose(file);
}

void calculateFees(ParkingRecord *record) {
   int entry_hour, entry_minute, exit_hour, exit_minute;

   sscanf(record->entry_time, "%d:%d", &entry_hour, &entry_minute); 
   sscanf(record->exit_time, "%d:%d", &exit_hour, &exit_minute);

   int entry_total_minutes = entry_hour * 60 + entry_minute; 
   int exit_total_minutes = exit_hour * 60 + exit_minute;

   int duration_hours = (exit_total_minutes - entry_total_minutes + 59) / 60; 

   if (duration_hours > 0) {
       record->fee = duration_hours * HOURLY_RATE; 
   } else {
       record->fee = 0; 
   }
}

void deleteParkingRecord() {
    FILE *file = fopen(FILENAME, "r"); 
    FILE *tempFile = fopen("temp.txt", "w"); 

    if (!file || !tempFile) {
        perror("Unable to open file!");
        return;
    }

    int id;
    printf("Enter ID of the parking record to delete: ");
    scanf("%d", &id);

    ParkingRecord record;
    int found = 0;

    while (fscanf(file, "%d %s %s %s %f", &record.id, record.license_plate, record.entry_time, record.exit_time, &record.fee) != EOF) {
        if (record.id == id) { 
            found = 1; 
            parking_slots[id - 1] = 0; 
            continue; 
        }
        fprintf(tempFile, "%d %s %s %s %.2f\n", record.id, record.license_plate, record.entry_time, record.exit_time, record.fee);
    }

    fclose(file);
    fclose(tempFile);

    remove(FILENAME); 
    rename("temp.txt", FILENAME); 

    if (found) {
        printf("Parking Record Deleted Successfully!\n");
    } else {
        printf("Record with ID %d not found!\n", id);
    }
}

void trackEntriesAndExits() {
   displayRecords(); 
}
 
void displayRecords() {
   FILE *file = fopen(FILENAME, "r"); 
   if (!file) {
       perror("Unable to open file!");
       return;
   }

   ParkingRecord record;
   printf("\n--- Parking Records ---\n");
   while (fscanf(file, "%d %s %s %s %f", &record.id,
                 record.license_plate,
                 record.entry_time,
                 record.exit_time,
                 &record.fee) != EOF) {
       printf("ID: %d | License Plate: %s | Entry Time: %s | Exit Time: %s | Fee: %.2f\n",
              record.id,
              record.license_plate,
              record.entry_time,
              strlen(record.exit_time) > 0 ? record.exit_time : "Not Exited",
              record.fee);
   }
   fclose(file);
}

void displayAvailableSlots() {
   printf("\n--- Available Parking Slots ---\n");
   for (int i = 0; i < MAX_PARKING_SPOTS; i++) {
       if (parking_slots[i] == 0) { 
           printf("Slot ID: %d is available.\n", i + 1); 
       }
   }
}

void generateRevenueReport() {
   printf("\n--- Revenue Report ---\n");
   printf("Total Revenue Generated: %.2f\n", total_revenue);
   printf("Total Vehicle Entries: %d\n", total_entries);
   printf("Total Vehicle Exits: %d\n", total_exits);
}
