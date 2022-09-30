#ifndef TAKIS_H
#define TAKIS_H
//enums and defines
typedef enum {none = 0, red, blue, green, yellow } Color;
#define RIGHT 1
#define LEFT -1
#define NO_VALID 0
#define VALID 1
#define START 4
#define NoWinner -1
#define PLUS 10
#define STOP 11
#define DIRCHANGE 12
#define TAKI 13
#define COLCHANGE 14
//structs
typedef struct Card {
	Color color;
	int num;
}Card;

typedef struct Player {
	char name[20];
	int packSize;
	Card** pack;
}Player;
//functions
int game(Player** list, int numOfPlayers);
int welcome();
void fillList(Player** list, int size);
void printList(Player** list, int size);
void printCard(Card* card);
void printPack(Player* player);
void setUpPlayer(Player* player, int serialNum);
Card* createCard();
Card* startingCard();
int chooseMove(int numOfChoices);
Card* turn(Player* player, int choice, Card* upperCard, int arr[]);
int checkValidMove(Card* chosenCard, Card* upperCard);
void organizePack(Player* player, int index);
Card* createIdenticalCard(Card* card);
void copyDeatils(Card* dest, Card* src);
void resetArr(int arr[]);
void addStat(int arr[], Card* card);
void checkPack(int arr[], Player**list ,int numOfPlayers);
void printStats(int arr[]);
int maxOfArray(int arr[]);
Card** getCardsByColor(Player** list, int numOfPlayers, Color wantedColor, int* pSize);
#endif
