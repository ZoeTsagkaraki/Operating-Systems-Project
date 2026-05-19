# Airline Reservation System - Operating Systems Project

Αυτή η εφαρμογή αποτελεί την εργασία για το μάθημα **Λειτουργικά Συστήματα**. Πρόκειται για ένα κατανεμημένο σύστημα κρατήσεων πτήσεων που βασίζεται στην αρχιτεκτονική Client-Server.

## Μέλη Ομάδας
* **Ζωή Τσαγκαράκη** (ΑΜ: 02507) - ztsagkaraki@uth.gr
* **Μαρία Ντάγκινη** (ΑΜ: 02505) - mntagkini@uth.gr

## Χαρακτηριστικά (Features)
- **Client-Server Architecture:** Επικοινωνία μέσω Sockets (TCP).
- **Multithreading:** Ο Server διαχειρίζεται πολλαπλούς πελάτες ταυτόχρονα χρησιμοποιώντας POSIX threads (`pthreads`).
- **Thread Safety:** Χρήση Mutex (`pthread_mutex_t`) για την ασφαλή ανάγνωση και εγγραφή στα αρχεία δεδομένων.
- **Flight Management:**
    - Αναζήτηση απευθείας πτήσεων.
    - Αναζήτηση πτήσεων με ανταπόκριση (connecting flights).
    - Πραγματοποίηση κρατήσεων με αυτόματη ενημέρωση (μείωση) των διαθέσιμων θέσεων.

## Δομή Δεδομένων
- `flights.txt`: Αποθηκεύει τις διαθέσιμες πτήσεις (ID, αναχώρηση, άφιξη, ώρες, χωρητικότητα).
- `bookings.txt`: Αποθηκεύει το αρχείο των κρατήσεων των επιβατών.

## Οδηγίες Εκτέλεσης
1. Μεταγλώττιση (Compile):
   ```bash
   g++ server.cpp -o server -pthread
   g++ client.cpp -o client