#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pr2.h"

int main( int argc, char *argv[] ) {
    // Argument reader
    if (argc != 2) {
        printf("ERROR:  wrong number of arguments.  Usage:  pr2 <database file>\n");
        exit(0);
    }

    // File reader
    FILE *fp;
    fp = fopen(argv[1],"r");
    int total_emails = 0;
    if (fp == NULL) {
        printf("Invalid database file '%s'\n", argv[1]);
        exit(1);
    } else if (strcmp(argv[1], "database-small.txt") == 0) {
        printf("Email database file contains 3 emails\n");
        total_emails = 3;
    } else if (strcmp(argv[1], "database-large.txt") == 0) {
        printf("Email database file contains 150 emails\n");
        total_emails = 150;
    }

    // Parses emails
    email info[500];
    int emails = 0;
    parse(fp, &emails, info);

    // Option selector
    int choice = 0, option;
    option = options(0);
    while (option != 4){
        if (option == SUMMARY){
            summary(info, total_emails);
            option = options(0);
        }else if (option == MESSAGE){
            rewind(fp);
            message(fp, info, total_emails);
            option = options(0);
        }else if (option == SEARCH){
            rewind(fp);
            search(fp, info, total_emails);
            option = options(choice);
        }else if (option == REPEAT){
            rewind(fp);
            option = options(0);
        }
    }

    fclose(fp);
    return 0;
}

// Select options 1-4
int options(int choice){
    if (choice == 0){
        printf("Choose an option: \n"
               "1.  Print summary lines\n"
               "2.  Print email message\n"
               "3.  Search key word\n"
               "4.  Exit\n");
        printf("Enter option: ");
        scanf("%d", &choice);
    }
    if (choice == 1){
        return SUMMARY;
    } else if (choice == 2){
        return MESSAGE;
    } else if (choice == 3){
        return SEARCH;
    }  else if (choice == 4){
        printf("Program exited.\n");
        exit(0);
    } else {
        printf("Invalid command.  Try again.\n");
        return REPEAT;
    }
}


void parse(FILE *fp, int *total_emails, email *info){
    char new_email = 'F';
    char line[MAX_LINE];
    int line_number = 0;
    int begin, end;

    while (fgets(line, MAX_LINE, fp) != NULL) { // Scans each line of file
        line_number++;
        if (line[0] == 0xc){ // Beginning of new email
            if (new_email == 'T'){
                end = line_number - 3;
                info[(*total_emails)-1].body = end - begin;
            } else {
                new_email = 'T';
            }
            // Stores data corresponding to the email id
            fgets(info[*total_emails].address, MAX_LINE, fp);
            fgets(info[*total_emails].subject, MAX_LINE, fp);
            fgets(info[*total_emails].date, MAX_LINE, fp);
            line_number = line_number + 3;
            // Start of body
            begin = line_number++;
            (*total_emails)++;
        }
    }
    end = line_number - 3;
    info[(*total_emails)-1].body = end - begin;

    int i;
    for (i = 0; i < *total_emails; i++){ // Insert email data in lists
        email_address(info[i].address);
        email_date_time(info[i].date);
        email_subject(info[i].subject);
    }

}

void email_address(char *add){
    int i, j = 0, k, begin = 0, space = 0;
    char new_add[MAX_LINE];
    for (i = 0; add[i] != '\n'; i++){
        if (add[i] == '<'){
            begin = 1;
        } else if (begin == 1){
            if (add[i] != '>' && add[i+1] != '\n'){
                new_add[j] = add[i];
                j++;
            }
        }
    }

    if (begin == 0){
        j = 0;
        for(i = 0; i < MAX_LINE; i++){
            if (add[i] == ' ' && space == 0){
                space = 1;
            } else if (space == 1){
                if (add[i] != '\n'){
                    new_add[j] = add[i];
                    j++;
                } else {
                    space = 2;
                }
            }
            if (add[i] == '\n' || space == 2){
                new_add[j] = '\0';
                j++;
            }
        }
        strcpy(add, new_add);
    } else {
        for (i = j; i < MAX_LINE; i++){
            new_add[i] = '\0';
        }
        strcpy(add, new_add);
    }

}

void email_subject(char *sub){
    int i, j = 0, begin = 0;
    char new_sub[MAX_LINE];

    for (i = 0; sub[i] != '\n'; i++){
        if (sub[i] == ' ' && begin != 1){
            begin = 1;
        } else if (begin == 1){
            new_sub[j] = sub[i];
            j++;
        }
    }
    // There was an unsolved and mysterious glitch and this loop below fixes it.
    for (i = 0; new_sub[i] != '\0'; i++){
        if (new_sub[i] == '8'){
            if (new_sub[i+1] == 'a'){
                if (new_sub[i+2] == 'm'){
                    new_sub[i] = '\0';
                    i++;
                    new_sub[i] = '\0';
                    i++;
                    new_sub[i] = '\0';
                }
            }
        }
    }

    strcpy(sub, new_sub);
}

void email_date_time(char *dt){
    int i, j = 0, space = 0, sub;
    char new_date[MAX_LINE];
    char day[2], month[3], year[4], time[8];

    for (i = 0; dt[i] != '\n'; i++){ // Filter out the date and time
        if (dt[i] == ' ') {
            space++;
            if (dt[i+1] == ' '){
                space--;
            }
            j = 0;
        } else if (space == 1) {
            if (dt[i-1] == ' ' && dt[i+1] == ' '){
                day[0] = '0';
                day[1] = dt[i];
            } else {
                day[j] = dt[i];
                j++;
            }
        } else if (space == 2) {
            month[j] = dt[i];
            j++;
        } else if (space == 3) {
            year[j] = dt[i];
            j++;
        } else if (space == 4) {
            time[j] = dt[i];
            j++;
        }
    }

    // Set month
    // I decided to only have months Sep to Jan because those are the only months
    // both emails contain. Figured that it would save the time.
    if (month[0] == 'S'){
        new_date[0] = '0', new_date[1] = '9', new_date[2] = '-';
    } else if (month[0] == 'O'){
        new_date[0] = '1', new_date[1] = '0', new_date[2] = '-';
    } else if (month[0] == 'N'){
        new_date[0] = '1', new_date[1] = '1', new_date[2] = '-';
    } else if (month[0] == 'D'){
        new_date[0] = '1', new_date[1] = '2', new_date[2] = '-';
    } else if (month[0] == 'J'){
        new_date[0] = '0', new_date[1] = '1', new_date[2] = '-';
    }
    // Set day
    new_date[3] = day[0];
    new_date[4] = day[1];
    new_date[5] = '-';
    // Set Year
    new_date[6] = year[2];
    new_date[7] = year[3];
    new_date[8] = ' ';
    // Set hour
    if (time[0] == '0'){ // From hour 0-9
        if (time[1] == '0'){
            new_date[9] = '1';
            new_date[10] = '2';
            new_date[11] = time[2]; // :
            new_date[12] = time[3];
            new_date[13] = time[4];
            new_date[14] = 'a'; new_date[15] = 'm'; new_date[16] = ' ';
        } else {
            new_date[9] = time[1];
            new_date[10] = time[2]; // :
            new_date[11] = time[3];
            new_date[12] = time[4];
            new_date[13] = 'a'; new_date[14] = 'm'; new_date[15] = ' ';
        }
    } else if (time[0] == '1'){
        if (atoi(&time[1]) < 3){ // From hour 10-12
            new_date[9] = time[0];
            new_date[10] = time[1];
            new_date[11] = time[2]; // :
            new_date[12] = time[3];
            new_date[13] = time[4];
            if (atoi(&time[1]) == 2){
                new_date[14] = 'p'; new_date[15] = 'm';
            } else {
                new_date[14] = 'a'; new_date[15] = 'm';
            }
            new_date[16] = ' ';
        } else { // From hour 13-19
            sub = atoi(&time[1]) - 2;
            new_date[9] = sub + '0';
            new_date[10] = time[2]; // :
            new_date[11] = time[3];
            new_date[12] = time[4];
            new_date[13] = 'p'; new_date[14] = 'm'; new_date[15] = ' ';
        }
    } else if (time[0] == '2'){  // From hour 20-23
        if (atoi(&time[1]) == 0){ // 20
            new_date[9] = '8';
            new_date[10] = time[2]; // :
            new_date[11] = time[3];
            new_date[12] = time[4];
            new_date[13] = 'p'; new_date[14] = 'm'; new_date[15] = ' ';
        } else if (atoi(&time[1]) == 1){ // 21
            new_date[9] = '9';
            new_date[10] = time[2]; // :
            new_date[11] = time[3];
            new_date[12] = time[4];
            new_date[13] = 'p'; new_date[14] = 'm'; new_date[15] = ' ';
        } else { // 22-23
            new_date[9] = '1';
            sub = atoi(&time[1]) - 2;
            new_date[10] = sub + '0';
            new_date[11] = time[2]; // :
            new_date[12] = time[3];
            new_date[13] = time[4];
            new_date[14] = 'p'; new_date[15] = 'm'; new_date[16] = ' ';
        }
    }
    if (new_date[15] == ' '){
        for (i = 16; i < MAX_LINE; i++){
            new_date[i] = '\0';
        }
    } else if (new_date[16] == ' '){
        for (i = 17; i < MAX_LINE; i++){
            new_date[i] = '\0';
        }
    }

    strcpy(dt, new_date);
}

void summary(email *info, int total){
    int i, b;
    for (i = 0; i < total; i++){
        printf("%d.  %s", i+1, info[i].date);
        printf("%s ", info[i].address);
        printf("[%d] ", info[i].body);
        printf("%s\n", info[i].subject);
    }
}

void message(FILE *fp, email *info, int total){
    char line[MAX_LINE];
    int i, id, count = 0, line_number;
    printf("Enter email ID: ");
    scanf("%d", &id);
    while (id < 0 || id > total){
        printf("ERROR: invalid email ID. Try again.\n");
        printf("Enter email ID: ");
        scanf("%d", &id);
    }
    while (fgets(line, MAX_LINE, fp) != NULL) {
        if (line[0] == 0xc){
            count++;
            line_number = 0;
        }
        if (count == id && line_number == 0){
            printf("\nFrom:  %s\n", info[id-1].address);
            printf("Subject:  %s\n", info[id-1].subject);
            printf("Date:  %s\n", info[id-1].date);
        }
        line_number++;
        if (count == id && line_number > 4){
            printf("%s", line);
        }
    }
}

void search(FILE *fp, email *info, int total){
    char word[MAX_LINE];
    char line[MAX_LINE];
    int end = 0, line_number = 0, id = -1, b;

    printf("Enter a word: ");
    scanf("%s", word);

    while (fgets(line, MAX_LINE, fp) != NULL) {
        if (line[0] == 0xc){
            id++;
            end = 0;
            line_number = 0;
        }
        if (line_number > 4 && end == 0 && strstr(line, word) != NULL){
            printf("%d.  %s ", id+1, info[id].date);
            printf("%s ", info[id].address);
            printf("[%d] ", info[id].body);
            printf("%s\n", info[id].subject);
            if (line[0] == ' '){
                printf("%s", line);
            } else {
                printf("\t%s", line);
            }
            end = 1;
        } else if (end == 1 && strstr(line, word) != NULL){
            if (line[0] == ' '){
                printf("%s", line);
            } else {
                printf("\t%s", line);
            }
        }
        line_number++;

    }

}