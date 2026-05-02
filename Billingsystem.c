#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// Constants
#define LIMIT 100
#define MAX_NAME_LEN 50
#define PASSWORD "admin123"

// Reads a line from input safely and removes newline character
void readLine(char *buffer, int size) {
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
}

// Login system with 3 attempts
int login() {
    char password[20];
    int attempts = 3;
    while (attempts--) {
        printf("\n🔐 Enter password: ");
        scanf("%19s", password);
        if (strcmp(password, PASSWORD) == 0) {
            printf("✅ Access granted.\n");
            return 1;
        } else {
            printf("❌ Incorrect password. %d attempt(s) left.\n", attempts);
        }
    }
    printf("🚫 Too many failed attempts. Exiting...\n");
    return 0;
}

// Displays system title
void printTitle() {
    printf("\n=== SIMPLE BILLING SYSTEM ===\n");
}

// Displays current date and time
void printDateTime() {
    time_t t;
    time(&t);
    printf("🕒 %s", ctime(&t));
}

// Adds items to the inventory
void addInventory() {
    FILE *f = fopen("inventory.txt", "a");
    if (!f) {
        printf("❌ Error: Cannot open inventory file\n");
        return;
    }

    int n;
    printf("How many items to add? ");
    scanf("%d", &n);
    getchar(); // Clear newline after number input

    for (int i = 0; i < n; i++) {
        char name[MAX_NAME_LEN];
        int qty;
        float price;

        // Get item details
        printf("\nItem %d name: ", i + 1);
        readLine(name, MAX_NAME_LEN);

        printf("Quantity: ");
        while (scanf("%d", &qty) != 1 || qty < 0) {
            printf("❌ Invalid quantity. Please enter a positive number: ");
            while (getchar() != '\n');
        }

        printf("Price: ");
        while (scanf("%f", &price) != 1 || price < 0) {
            printf("❌ Invalid price. Please enter a positive number: ");
            while (getchar() != '\n');
        }
        getchar();

        // Save to file
        fprintf(f, "%-20s %-10d %-10.2f\n", name, qty, price);
    }

    fclose(f);
    printf("✅ Inventory updated.\n");
}

// Views current inventory from file
void viewInventory() {
    FILE *f = fopen("inventory.txt", "r");
    if (!f) {
        printf("❌ No inventory found.\n");
        return;
    }

    printf("\n📦 Inventory:\n");
    printf("========================================\n");
    printf("%-20s %-10s %-10s\n", "Item Name", "Quantity", "Price");
    printf("========================================\n");

    char name[MAX_NAME_LEN];
    int qty;
    float price;

    // Read each item and display
    while (fscanf(f, "%19s %d %f", name, &qty, &price) == 3) {
        printf("%-20s %-10d %-10.2f\n", name, qty, price);
    }
    printf("========================================\n");
    fclose(f);
}

// Allows editing quantity and price of an item
void editInventory() {
    viewInventory(); // Show existing items first

    char target[MAX_NAME_LEN];
    printf("\nEnter item name to edit: ");
    readLine(target, MAX_NAME_LEN);

    FILE *f = fopen("inventory.txt", "r");
    if (!f) {
        printf("❌ Error: Cannot open inventory file\n");
        return;
    }

    FILE *temp = fopen("temp.txt", "w");
    if (!temp) {
        printf("❌ Error: Cannot create temporary file\n");
        fclose(f);
        return;
    }

    char name[MAX_NAME_LEN];
    int qty, found = 0;
    float price;

    // Copy content to temp file, updating target item
    while (fscanf(f, "%19s %d %f", name, &qty, &price) == 3) {
        if (strcmp(name, target) == 0) {
            found = 1;

            printf("\nCurrent details:\n");
            printf("Name: %s\nQuantity: %d\nPrice: %.2f\n", name, qty, price);

            printf("\nEnter new quantity: ");
            while (scanf("%d", &qty) != 1 || qty < 0) {
                printf("❌ Invalid quantity. Please enter a positive number: ");
                while (getchar() != '\n');
            }

            printf("Enter new price: ");
            while (scanf("%f", &price) != 1 || price < 0) {
                printf("❌ Invalid price. Please enter a positive number: ");
                while (getchar() != '\n');
            }
            getchar();
        }
        fprintf(temp, "%-20s %-10d %-10.2f\n", name, qty, price);
    }

    fclose(f);
    fclose(temp);

    if (found) {
        remove("inventory.txt");
        rename("temp.txt", "inventory.txt");
        printf("\n✅ Item updated successfully\n");
    } else {
        remove("temp.txt");
        printf("❌ Item '%s' not found\n", target);
    }
}

// Deletes an item from inventory
void deleteInventory() {
    viewInventory(); // Show current items

    char target[MAX_NAME_LEN];
    printf("\nEnter item name to delete: ");
    readLine(target, MAX_NAME_LEN);

    printf("Are you sure you want to delete '%s'? (y/n): ", target);
    char confirm;
    scanf(" %c", &confirm);
    getchar();

    if (tolower(confirm) != 'y') {
        printf("❌ Deletion cancelled\n");
        return;
    }

    FILE *f = fopen("inventory.txt", "r");
    if (!f) {
        printf("❌ Error: Cannot open inventory file\n");
        return;
    }

    FILE *temp = fopen("temp.txt", "w");
    if (!temp) {
        printf("❌ Error: Cannot create temporary file\n");
        fclose(f);
        return;
    }

    char name[MAX_NAME_LEN];
    int qty, found = 0;
    float price;

    // Copy all but target item to temp file
    while (fscanf(f, "%19s %d %f", name, &qty, &price) == 3) {
        if (strcmp(name, target) == 0) {
            found = 1;
            continue;
        }
        fprintf(temp, "%-20s %-10d %-10.2f\n", name, qty, price);
    }

    fclose(f);
    fclose(temp);

    if (found) {
        remove("inventory.txt");
        rename("temp.txt", "inventory.txt");
        printf("✅ Item '%s' deleted successfully\n", target);
    } else {
        remove("temp.txt");
        printf("❌ Item '%s' not found\n", target);
    }
}

// Applies discount to total if needed
float applyDiscount(float total) {
    char ch;
    printf("Apply discount? (y/n): ");
    scanf(" %c", &ch);
    if (tolower(ch) == 'y') {
        float d;
        do {
            printf("Enter discount %% (0-100): ");
            while (scanf("%f", &d) != 1) {
                printf("❌ Invalid input. Please enter a number: ");
                while (getchar() != '\n');
            }
            if (d < 0 || d > 100) {
                printf("❌ Discount must be between 0 and 100\n");
            }
        } while (d < 0 || d > 100);
        total -= total * d / 100;
        printf("💸 Discounted Total: %.2f\n", total);
    }
    return total;
}

// Generates a customer invoice and updates inventory
void generateInvoice() {
    char custName[50], phone[15], itemName[30];
    int qty, numItems;
    float price, total = 0;

    char invoiceItems[LIMIT][30];
    int itemQty[LIMIT];
    float itemPrice[LIMIT];

    // Get customer details
    printf("Enter customer name: ");
    fgets(custName, sizeof(custName), stdin);
    custName[strcspn(custName, "\n")] = '\0';

    int valid = 0;
    while (!valid) {
        printf("Enter phone number (7 to 14 digits): ");
        scanf("%s", phone);
        getchar();
        int len = strlen(phone);
        valid = len >= 7 && len <= 14;
        for (int i = 0; i < len && valid; i++) {
            if (!isdigit(phone[i])) valid = 0;
        }
        if (!valid) printf("❌ Invalid phone number! Try again.\n");
    }

    // Get items to bill
    printf("How many items to bill? ");
    scanf("%d", &numItems);
    getchar();

    FILE *invFile = fopen("inventory.txt", "r+");
    FILE *outFile = fopen("invoices.txt", "a");
    if (!invFile || !outFile) {
        printf("File error.\n");
        return;
    }

    for (int i = 0; i < numItems; i++) {
        int found = 0;
        printf("\nItem %d name: ", i + 1);
        fgets(itemName, sizeof(itemName), stdin);
        itemName[strcspn(itemName, "\n")] = '\0';

        do {
            printf("Enter quantity: ");
            scanf("%d", &qty);
            if (qty <= 0) printf("❌ Quantity must be greater than 0.\n");
        } while (qty <= 0);
        getchar();

        FILE *temp = fopen("temp.txt", "w");
        char name[30];
        int stock;
        float p;

        // Search for item in inventory and update stock
        while (fscanf(invFile, "%s %d %f", name, &stock, &p) == 3) {
            if (strcmp(name, itemName) == 0 && stock >= qty) {
                found = 1;
                strcpy(invoiceItems[i], name);
                itemQty[i] = qty;
                itemPrice[i] = p;
                stock -= qty;
                total += qty * p;
            }
            fprintf(temp, "%s %d %.2f\n", name, stock, p);
        }

        fclose(invFile);
        fclose(temp);

        if (!found) {
            printf("Item not found or not enough stock.\n");
            i--;
            invFile = fopen("inventory.txt", "r+");
            continue;
        }

        remove("inventory.txt");
        rename("temp.txt", "inventory.txt");
        invFile = fopen("inventory.txt", "r+");
    }

    // Display invoice
    printf("\n----- Invoice -----\n");
    printf("Customer: %s\nPhone: %s\n", custName, phone);
    printf("+-----------------+------------+------------+\n");
    printf("| Item            | Qty        | Price      |\n");
    printf("+-----------------+------------+------------+\n");

    fprintf(outFile, "\nCustomer: %s\nPhone: %s\n", custName, phone);
    fprintf(outFile, "+-----------------+------------+------------+\n");
    fprintf(outFile, "| Item            | Qty        | Price      |\n");
    fprintf(outFile, "+-----------------+------------+------------+\n");

    for (int i = 0; i < numItems; i++) {
        printf("| %-15s | %-10d | %-10.2f |\n", invoiceItems[i], itemQty[i], itemPrice[i]);
        fprintf(outFile, "| %-15s | %-10d | %-10.2f |\n", invoiceItems[i], itemQty[i], itemPrice[i]);
    }

    printf("+-----------------+------------+------------+\n");

    float finalTotal = applyDiscount(total);

    printf("Final Total: %.2f\n", finalTotal);
    fprintf(outFile, "+-----------------+------------+------------+\n");
    fprintf(outFile, "Final Total: %.2f\n", finalTotal);
    fprintf(outFile, "------------------------------\n");

    fclose(invFile);
    fclose(outFile);
}

// Displays all saved invoices
void viewInvoices() {
    FILE *f = fopen("invoices.txt", "r");
    if (!f) {
        printf("❌ No invoices found.\n");
        return;
    }

    printf("\n🧾 INVOICE HISTORY 🧾\n");
    printf("========================================\n");
    char line[200];
    while (fgets(line, sizeof(line), f)) {
        printf("%s", line);
    }
    printf("========================================\n");
    fclose(f);
}

// Clears screen (works on both Windows and Linux)
void clearScreen() {
    system("cls || clear");
}

// Main program with menu
int main() {
    if (!login()) return 0;

    int ch;
    do {
        clearScreen();
        printTitle();
        printDateTime();
        printf("\nMAIN MENU\n");
        printf("1. Add Inventory\n2. View Inventory\n3. Edit Inventory\n4. Delete Inventory\n");
        printf("5. Generate Invoice\n6. View Invoices\n7. Exit\n");
        printf("\nChoose option: ");

        while (scanf("%d", &ch) != 1 || ch < 1 || ch > 7) {
            printf("❌ Invalid choice. Please enter 1-7: ");
            while (getchar() != '\n');
        }
        getchar();

        clearScreen();
        switch (ch) {
            case 1: addInventory(); break;
            case 2: viewInventory(); break;
            case 3: editInventory(); break;
            case 4: deleteInventory(); break;
            case 5: generateInvoice(); break;
            case 6: viewInvoices(); break;
            case 7: printf("👋 Exiting...\n"); break;
        }

        if (ch != 7) {
            printf("\nPress Enter to return to menu...");
            while (getchar() != '\n');
        }
    } while (ch != 7);

    return 0;
}