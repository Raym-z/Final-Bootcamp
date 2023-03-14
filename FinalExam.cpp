#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>

struct Node
{
    int bf, height;
    double tempe;
    char planetName[100];
    Node *left, *right;
};

int max(int a, int b)
{
    return (a > b) ? a : b;
}

int getHeight(Node *root)
{
    return root ? root->height : 0;
}

int setHeight(Node *root)
{
    return max(getHeight(root->left), getHeight(root->right)) + 1;
}

int setBalanceFactor(Node *root)
{
    return getHeight(root->left) - getHeight(root->right);
}

Node *createNode(double tempe, char *name)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->tempe = tempe;
    strcpy(newNode->planetName, name);
    newNode->height = 1;
    newNode->bf = 0;
    newNode->left = newNode->right = NULL;
    return newNode;
}

Node *updateNode(Node *root)
{
    root->height = setHeight(root);
    root->bf = setBalanceFactor(root);
    return root;
}

Node *leftRotate(Node *root)
{
    Node *pivot = root->right;
    Node *temp = pivot->left;
    pivot->left = root;
    root->right = temp;
    root = updateNode(root);
    pivot = updateNode(pivot);
    return pivot;
}

Node *rightRotate(Node *root)
{
    Node *pivot = root->left;
    Node *temp = pivot->right;
    pivot->right = root;
    root->left = temp;
    root = updateNode(root);
    pivot = updateNode(pivot);
    return pivot;
}

Node *rotation(Node *root)
{
    if (root->bf > 1)
    {
        if (root->left->bf >= 0)
        {
            return rightRotate(root);
        }
        else
        {
            root->left = leftRotate(root->left);
            return rightRotate(root);
        }
    }
    else if (root->bf < -1)
    {
        if (root->right->bf <= 0)
        {
            return leftRotate(root);
        }
        else
        {
            root->right = rightRotate(root->right);
            return leftRotate(root);
        }
    }

    return root;
}
int validateInsertSame(Node *root, double temperature, char *name)
{
    if (!root)
        return 1;

    if (root->tempe == temperature)
    {
        return 0;
    }
    validateInsertSame(root->left, temperature, name);
    validateInsertSame(root->right, temperature, name);
    return 1;
}

Node *insertNode(Node *root, double tempe, char *name)
{
    int valid = validateInsertSame(root, tempe, name);
    if (valid == 1)
    {
        if (!root)
        {
            printf("\n\nAdded %s with temperature of %.2lf\n\n", name, tempe);
            return createNode(tempe, name);
        }
        else if (tempe < root->tempe)
        {
            root->left = insertNode(root->left, tempe, name);
        }
        else if (tempe > root->tempe)
        {
            root->right = insertNode(root->right, tempe, name);
        }
        return rotation(updateNode(root));
    }
    return root;
}

Node *predecessor(Node *root)
{
    Node *curr = root->left;

    while (curr->right)
    {
        curr = curr->right;
    }

    return curr;
}

Node *deleteNode(Node *root, double value)
{
    if (!root)
    {
        return root;
    }

    if (value < root->tempe)
    {
        root->left = deleteNode(root->left, value);
    }
    else if (value > root->tempe)
    {
        root->right = deleteNode(root->right, value);
    }
    else
    {
        if (!root->left || !root->right)
        {
            Node *newRoot = root->left ? root->left : root->right;
            root->left = root->right = NULL;
            free(root);
            root = NULL;
            return newRoot;
        }

        Node *inOrderPredecessor = predecessor(root);
        root->tempe = inOrderPredecessor->tempe;
        root->left = deleteNode(root->left, inOrderPredecessor->tempe);
    }

    return rotation(updateNode(root));
}

int inOrder(Node *root, int index)
{
    if (!root)
        return index;

    index = inOrder(root->left, index);
    printf("| %-3d | %-13.2lf | %-20s |\n", index, root->tempe, root->planetName);
    index++;
    index = inOrder(root->right, index);

    return index;
}

void Menu()
{
    printf("\e[1;1H\e[2J");
    printf("BeeAero Database System\n");
    printf("1. Insert planet and temperature.\n");
    printf("2. Delete planet from database.\n");
    printf("3. Search similar planet.\n");
    printf("4. Show planets from index to index in the database.\n");
    printf("5. View all planet in database.\n");
    printf("6. Exit database\n>> ");
}

int count_higher_temp(Node *root, double temperature)
{
    if (!root)
    {
        return 0;
    }
    int count = 0;

    if (root->tempe > temperature)
        count++;

    count += count_higher_temp(root->right, temperature);
    count += count_higher_temp(root->left, temperature);

    return count;
}

int validateTemp(double temperature)
{
    if (pow(-2, 31) > temperature || temperature > ((pow(2, 31) - 1)))
    {
        return 0;
    }
    return 1;
}
int validateInsert(double temperature, char *name)
{
    int valid = 1;
    valid = validateTemp(temperature);
    if (!valid)
        return 0;

    for (int i = 0; i < strlen(name); i++)
    {
        if (isupper(name[i]) || isspace(name[i]) || strlen(name) > 30)
        {
            printf("\n\nInput a proper format\nThe format should not contain whitespace and\nPlanet name shouldn't be longer than 30 characters\nTemperature should be between -2^31 and 2^31 -1\n\n");
            return 0;
        }
    }
    return 1;
}

void find_similar_temperature(Node *root, double temperature)
{

    double min_diff = abs(root->tempe - temperature);
    double similar_temp = root->tempe;
    char planetname_temp[100];
    strcpy(planetname_temp, root->planetName);
    Node *current = root;

    while (current)
    {
        double diff = fabs(current->tempe - temperature);

        if (diff < min_diff)
        {
            min_diff = diff;
            similar_temp = current->tempe;
            strcpy(planetname_temp, current->planetName);
        }
        else if (diff == min_diff && current->tempe > similar_temp)
        {
            similar_temp = current->tempe;
            strcpy(planetname_temp, current->planetName);
        }

        if (current->tempe == temperature)
        {
            printf("\nFound the planet\n%s - %.2lf", current->planetName, current->tempe);
            return;
        }
        else if (current->tempe < temperature)
        {
            current = current->right;
        }
        else
        {
            current = current->left;
        }
    }
    printf("Planet with smallest Temperature difference : %s - %.2lf\n", planetname_temp, similar_temp);
}

void inOrderRange(Node *root, int startNode, int endNode, int &index)
{
    if (root == NULL)
    {
        return;
    }

    inOrderRange(root->left, startNode, endNode, index);

    if (index >= startNode && index <= endNode)
    {
        printf("| %-3d | %-13.2lf | %-20s |\n", index, root->tempe, root->planetName);
    }
    index++;

    inOrderRange(root->right, startNode, endNode, index);
}

void inOrderRangeStart(Node *root, int startNode, int endNode)
{
    int index = 1; // initialize index to 1
    inOrderRange(root, startNode, endNode, index);
}

int validateFromTo(int start, int to)
{
    if (start > to)
    {
        return 0;
    }
    else if (start < 0 || to > pow(2, 31) - 1)
    {
        printf("\nThe range of index possible is 1 to 2^31 -1.\n");
        return 0;
    }
    else if ((to - start) > 10)
    {
        printf("\nCan only view up to 10 planets at once.\n");
        return 0;
    }
    return 1;
}

int main()
{
    Node *baseRoot = NULL;
    int options;
    do
    {
        Menu();
        scanf("%d", &options);
        getchar();

        double temperature, deleteTarget, searchTarget;
        char name[100];
        int counter, valid = 1, random = 0, start, end;

        switch (options)
        {
        case 1:
            printf("\e[1;1H\e[2J");
            printf("Input Temperature and Planet Name [Temperature - PlanetName]\n");
            printf("For Example 20 pluto\n\n>> ");
            scanf("%lf %[^\n]", &temperature, name);
            getchar();

            valid = validateInsert(temperature, name);
            if (!valid)
            {
                printf("Press Enter to Continue\n\n");
                getchar();
                break;
            }

            baseRoot = insertNode(baseRoot, temperature, name);
            counter = count_higher_temp(baseRoot, temperature);

            printf("Amount of planets with a bigger temperature : %d\n", counter);
            printf("Press Enter to Continue\n\n");
            getchar();

            break;

        case 2:
            printf("\e[1;1H\e[2J");
            printf("Input the temperature of the planet you want to delete\n");
            scanf("%lf", &deleteTarget);
            getchar();

            baseRoot = deleteNode(baseRoot, deleteTarget);
            valid = random = 0;
            random = inOrder(baseRoot, valid);

            printf("\e[1;1H\e[2J");
            printf("Amount of Planets left in the database : %d\n", random);
            printf("Press Enter to Continue\n\n");
            getchar();
            break;

        case 3:
            printf("\e[1;1H\e[2J");
            if (!baseRoot)
            {
                printf("Database is empty\n");
                printf("\nPress Enter to Continue\n");
                getchar();
                break;
            }
            printf("Input a temperature\n>> ");
            scanf(" %lf", &searchTarget);
            random = validateTemp(searchTarget);

            if (random == 0)
            {
                printf("Input a proper Temperature\nTemperature should be between -2^31 and 2^31 -1\n");
                break;
            }
            getchar();
            find_similar_temperature(baseRoot, searchTarget);
            printf("\nPress Enter to Continue\n");
            getchar();
            break;

        case 5:
            printf("\n| %s | %-13s | %-20s |\n", "NO.", "Temperature", "Planet Name");
            valid = 0;
            random = inOrder(baseRoot, valid + 1);
            printf("\nPress Enter to Continue\n");
            getchar();
            break;

        case 6:
            printf("\nSayonara~\n\n\nExiting....\n\n\n");
            exit(0);
            break;

        case 4:
            printf("\e[1;1H\e[2J");
            printf("This function prints planet names, according to the index you want, (from - to )\nFor example : 1 2 which means planet 1 to 2\n");
            scanf(" %d %d", &start, &end);
            getchar();

            valid = validateFromTo(start, end);
            if (valid == 0)
            {
                printf("ERROR\n");
                printf("\nPress Enter to Continue\n");
                getchar();
                break;
            }

            printf("\n| %s | %-13s | %-20s |\n", "NO.", "Temperature", "Planet Name");
            inOrderRangeStart(baseRoot, start, end);
            printf("\nPress Enter to Continue\n");
            getchar();
            break;

        default:

            printf("\nInput a proper Option\n");
            printf("\nPress Enter to Continue\n");
            getchar();
            break;
        }

    } while (1);

    return 0;
}