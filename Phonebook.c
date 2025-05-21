#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME 100
#define MAX_PHONE 20

typedef struct Contact 
{
    char name[MAX_NAME];
    char phone[MAX_PHONE];
    struct Contact* left;
    struct Contact* right;
} Contact;

void capitalizeEachWord(char* str) 
{
    int inWord = 0;
    for (int i = 0; str[i]; i++) 
    {
        if (isspace(str[i])) 
        {
            inWord = 0;
        } 
        else 
        {
            if (!inWord) 
            {
                str[i] = toupper(str[i]);
                inWord = 1;
            } 
            else 
            {
                str[i] = tolower(str[i]);
            }
        }
    }
}

Contact* createNode(char* name, char* phone) 
{
    Contact* newNode = (Contact*)malloc(sizeof(Contact));
    strcpy(newNode->name, name);
    strcpy(newNode->phone, phone);
    newNode->left = newNode->right = NULL;
    return newNode;
}

Contact* insert(Contact* root, char* name, char* phone) 
{
    if (root == NULL)
        return createNode(name, phone);
    int cmp = strcmp(name, root->name);
    if (cmp < 0)
        root->left = insert(root->left, name, phone);
    else if (cmp > 0)
        root->right = insert(root->right, name, phone);
    else
        printf("Contact '%s' already exists.\n", name);
    return root;
}

Contact* search(Contact* root, char* name) 
{
    if (!root || strcmp(name, root->name) == 0)
        return root;
    if (strcmp(name, root->name) < 0)
        return search(root->left, name);
    else
        return search(root->right, name);
}

Contact* searchByPhone(Contact* root, const char* phone) 
{
    if (!root)
        return NULL;
    if (strcmp(root->phone, phone) == 0)
        return root;
    Contact* left = searchByPhone(root->left, phone);
    if (left) return left;
    return searchByPhone(root->right, phone);
}

void suggestMatches(Contact* root, const char* prefix) 
{
    if (!root)
        return;
    if (strncmp(root->name, prefix, strlen(prefix)) == 0 || strncmp(root->phone, prefix, strlen(prefix)) == 0) 
    {
        printf("%s: %s\n", root->name, root->phone);
    }
    suggestMatches(root->left, prefix);
    suggestMatches(root->right, prefix);
}

Contact* minValueNode(Contact* node) 
{
    Contact* current = node;
    while (current && current->left)
        current = current->left;
    return current;
}

Contact* delete(Contact* root, char* name) 
{
    if (!root) return root;
    int cmp = strcmp(name, root->name);
    if (cmp < 0)
        root->left = delete(root->left, name);
    else if (cmp > 0)
        root->right = delete(root->right, name);
    else {
        if (!root->left) 
        {
            Contact* temp = root->right;
            free(root);
            return temp;
        } 
        else if (!root->right) 
        {
            Contact* temp = root->left;
            free(root);
            return temp;
        }
        Contact* temp = minValueNode(root->right);
        strcpy(root->name, temp->name);
        strcpy(root->phone, temp->phone);
        root->right = delete(root->right, temp->name);
    }
    return root;
}

void edit(Contact** root, char* name) 
{
    Contact* node = search(*root, name);
    if (node) 
    {
        int choice;
        char newName[MAX_NAME], newPhone[MAX_PHONE];
        printf("Edit Options:\n1. Name\n2. Phone\n3. Both\nEnter choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) 
        {
            case 1:
                printf("Enter new name: ");
                fgets(newName, MAX_NAME, stdin);
                newName[strcspn(newName, "\n")] = 0;
                capitalizeEachWord(newName);
                strcpy(newPhone, node->phone);
                *root = delete(*root, node->name);
                *root = insert(*root, newName, newPhone);
                printf("Name updated.\n");
                break;

            case 2:
                printf("Enter new phone: ");
                fgets(newPhone, MAX_PHONE, stdin);
                newPhone[strcspn(newPhone, "\n")] = 0;
                strcpy(node->phone, newPhone);
                printf("Phone updated.\n");
                break;

            case 3:
                printf("Enter new name: ");
                fgets(newName, MAX_NAME, stdin);
                newName[strcspn(newName, "\n")] = 0;
                capitalizeEachWord(newName);

                printf("Enter new phone: ");
                fgets(newPhone, MAX_PHONE, stdin);
                newPhone[strcspn(newPhone, "\n")] = 0;

                *root = delete(*root, node->name);
                *root = insert(*root, newName, newPhone);
                printf("Name and phone updated.\n");
                break;

            default:
                printf("Invalid option.\n");
        }
    } 
    else 
    {
        printf("Contact '%s' not found.\n", name);
    }
}

void display(Contact* root) 
{
    if (root) 
    {
        display(root->left);
        printf("%s: %s\n", root->name, root->phone);
        display(root->right);
    }
}

Contact* loadFromFile(const char* filename, Contact* root) 
{
    FILE* file = fopen(filename, "r");
    if (!file) return root;
    char line[200], name[MAX_NAME], phone[MAX_PHONE];
    while (fgets(line, sizeof(line), file)) 
    {
        if (sscanf(line, "%[^,],%s", name, phone) == 2) 
        {
            capitalizeEachWord(name);
            root = insert(root, name, phone);
        }
    }
    fclose(file);
    return root;
}

void saveToFile(FILE* file, Contact* root) 
{
    if (root) 
    {
        saveToFile(file, root->left);
        fprintf(file, "%s,%s\n", root->name, root->phone);
        saveToFile(file, root->right);
    }
}

int main() 
{
    Contact* root = NULL;
    char name[MAX_NAME], phone[MAX_PHONE];
    int choice;

    root = loadFromFile("contacts.txt", root);

    while (1) 
    {
        printf("-----Phonebook Manager-----");
        printf("\n1. Add Contact\n2. Edit Contact\n3. Delete Contact\n4. Search Contact\n5. Display All\n6. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) 
        {
            case 1:
                printf("Enter name: ");
                fgets(name, MAX_NAME, stdin);
                name[strcspn(name, "\n")] = 0;
                capitalizeEachWord(name);
                printf("Enter phone: ");
                fgets(phone, MAX_PHONE, stdin);
                phone[strcspn(phone, "\n")] = 0;
                root = insert(root, name, phone);
                printf("\n");
                break;

            case 2:
                printf("Enter name to edit: ");
                fgets(name, MAX_NAME, stdin);
                name[strcspn(name, "\n")] = 0;
                capitalizeEachWord(name);
                edit(&root, name);
                printf("\n");
                break;

            case 3:
                printf("Enter name to delete: ");
                fgets(name, MAX_NAME, stdin);
                name[strcspn(name, "\n")] = 0;
                capitalizeEachWord(name);
                root = delete(root, name);
                printf("Contact deleted\n");
                printf("\n");
                break;

            case 4:
                printf("Enter name or phone to search: ");
                fgets(name, MAX_NAME, stdin);
                name[strcspn(name, "\n")] = 0;
                capitalizeEachWord(name);

                Contact* found = search(root, name);
                if (found) 
                {
                    printf("Found: %s - %s\n", found->name, found->phone);
                    printf("1. Edit\n2. Delete\n3. Do Nothing\nEnter choice: ");
                    int subchoice;
                    scanf("%d", &subchoice);
                    getchar();
                    switch (subchoice)
                    {
                        case 1:
                        edit(&root, found->name);
                        break;
                        
                        case 2:
                        root = delete(root, found->name);
                        printf("Contact deleted\n");
                        break;
                        
                        case 3:
                        break;
                        
                        default:
                        printf("Invalid choice\n");
                    }
                } 
                else 
                {
                    Contact* phoneMatch = searchByPhone(root, name);
                    if (phoneMatch) 
                    {
                        printf("Found by phone: %s - %s\n", phoneMatch->name, phoneMatch->phone);
                    } 
                    else 
                    {
                        printf("Contact not found. Suggestions starting with '%s':\n", name);
                        suggestMatches(root, name);
                        printf("Do you want to add this contact? (Y/N): ");
                        char ans;
                        scanf("%c", &ans);
                        getchar();
                        ans = toupper(ans);
                        if (ans == 'Y') 
                        {
                            strcpy(name, name);
                            capitalizeEachWord(name);
                            printf("Enter phone: ");
                            fgets(phone, MAX_PHONE, stdin);
                            phone[strcspn(phone, "\n")] = 0;
                            root = insert(root, name, phone);
                            printf("\n");
                        }
                        else if(ans == 'N')
                        {
                            printf("Contact not added.\n");
                        }
                        else
                        {
                            printf("Invalid Input! Contact not added.\n");
                        }
                    }
                }
                break;

            case 5:
                printf("All Contacts:\n");
                display(root);
                break;

            case 6:
                printf("Saving and exiting...\n");
                FILE* file = fopen("contacts.txt", "w");
                if (file) 
                {
                    saveToFile(file, root);
                    fclose(file);
                    printf("Contacts saved to contacts.txt\n");
                } 
                else 
                {
                    printf("Error saving file.\n");
                }
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }
    return 0;
}