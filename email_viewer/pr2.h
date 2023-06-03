#ifndef PR2_PR2_H
#define PR2_PR2_H
#endif //PR2_PR2_H

#define MAX_LINE 1000
#define SUMMARY 1
#define MESSAGE 2
#define SEARCH 3
#define REPEAT 0

// Organizes email info in memory
typedef struct email_info {
    char address[MAX_LINE]; // Address of email
    char subject[MAX_LINE]; // Subject of email
    char date[MAX_LINE]; // Date and time of email
    int body;
} email;

// Parse database
void parse(FILE *fp, int *total_emails, email *info);

// Option selector
int options(int choice);
void summary(email *info, int total); // Option 1
void message(FILE *fp, email *info, int total); // Option 2
void search(FILE *fp, email *info, int total);// Option 3

// Option 1 components
void email_address(char *add);
void email_subject(char *sub);
void email_date_time(char *dt);
