/****
	Name: Yehonatan Reisinger
	ID: 211398375
****/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

//defines
#define _CRT_SECURE_NO_WARNINGS
#define EMPTY 0
#define CLOSE_TAKI 0
//maximums
#define MAX_NAME_LEN 20
#define PACK_SIZE_IN_START 4
#define NUMBER_OF_COLORS 4
#define NUMBER_OF_TYPES 6
//card types and colors
#define SPECIAL_CARD_TYPE 0
#define RED_COLOR 'R'
#define BLUE_COLOR 'B'
#define YELLOW_COLOR 'Y'
#define GREEN_COLOR 'G'
#define NO_COLOR ' '
//turn related defines
#define RIGHT 1
#define LEFT -1
#define TAKE_CARD_FROM_DECK 0
//enums
typedef enum { numberType, plus, stop, changeDirection, taki, changeColor} Type;

//structs
typedef struct Card {
	char color;
	int num;
	Type type;
}Card;

typedef struct Player {
	char name[MAX_NAME_LEN];
	int logPackSize;
	int phyPackSize;
	Card* pack;
}Player;

typedef struct GameInfo {
	Player* allPlayers;
	int numOfPlayers;
	int direction;
	int currentTurn;
}GameInfo;

//functions
void getGameInfo(GameInfo* gameInfoP);
void freeAll(GameInfo* gameInfoP);
void checkIfAllocationSuccessful(void* pointer);
int getNumOfPlayers();
void getPlayersInfo(GameInfo* game);
void givePlayerNewPack(Player* player);
Card createNewCard();
char getRandomCardColor();
void printPack(Player* player);
void printCard(Card* card);
Card createStartingCard();
void updateCurrentTurn(GameInfo* gameInfoP);
void advanceTurnToNextPlayer(GameInfo* gameInfoP);
void presentActivePlayerAndUpperCard(Player* activePlayer, Card* upperCard);
int getCardChoice(int numOfChoices);
void getCardFromDeck(Player* activePlayer);
bool isMoveValid(Card chosenCard, Card* upperCard);
void makeSpecialCardMove(GameInfo* gameInfoP, Card** upperCard);
void givePlayerAnotherTurn(GameInfo* gameInfoP);
void stopNextPlayer(GameInfo* gameInfoP);
void changeColorOfGame(GameInfo* gameInfoP, Card** upperCard);
void openTaki(GameInfo* gameInfoP, Card** upperCard);
void putChosenCardOnTopOfDeck(Player* activePlayer, int cardChoice, Card** upperCard);
void swapCardsInPack(Card pack[], int index1, int index2);

int main()
{
	srand(time(NULL));
	int cardChoice;
	GameInfo gameInfo;
	Player* activePlayer;
	Card startingCard = createStartingCard();
	Card* upperCard = &startingCard;
	getGameInfo(&gameInfo);
	activePlayer = &(gameInfo.allPlayers[gameInfo.currentTurn]);
	//start the game
	while (activePlayer->logPackSize != 0)
	{
		//
		//maybe get rid of it
		//
		updateCurrentTurn(&gameInfo);
		//give the turn to player that's currently need to be playing
		activePlayer = &(gameInfo.allPlayers[gameInfo.currentTurn]);
		//present the players name, pack, and the upper card
		presentActivePlayerAndUpperCard(activePlayer, upperCard);
		cardChoice = getCardChoice(activePlayer->logPackSize);
		//add a new card to the player if he chose to
		if (cardChoice == TAKE_CARD_FROM_DECK)
		{
			getCardFromDeck(activePlayer);
			advanceTurnToNextPlayer(&gameInfo);
			continue;
		}
		//if the player wanted to put a card against the games rules, he will have to make turns until he will choose a valid card
		if(!isMoveValid(activePlayer->pack[cardChoice - 1],upperCard))
		{
			printf("Invalid card! Try again.\n");
				continue;
		}
		//else, the move is valid
		putChosenCardOnTopOfDeck(activePlayer,cardChoice ,&upperCard);	
		//make the move according to the player's choice
		if (upperCard->type != numberType)
			makeSpecialCardMove(&gameInfo, &upperCard);
		//give the player to the next player in line
		advanceTurnToNextPlayer(&gameInfo);
	}
	printf("the winner is... %s! Congratulations!\n\n", activePlayer->name);
	freeAll(&gameInfo);
	return 0;
}
void putChosenCardOnTopOfDeck(Player* activePlayer, int cardChoice, Card** upperCard)
{
	//put the chosen card in the end of the cards pack
	if(cardChoice != activePlayer->logPackSize)
		swapCardsInPack(activePlayer->pack, cardChoice - 1, activePlayer->logPackSize - 1);
	//make the chosen card by the player the new upper card
	*upperCard = &(activePlayer->pack[activePlayer->logPackSize - 1]);
	activePlayer->logPackSize--;
}
void swapCardsInPack(Card pack[], int index1, int index2)
{
	Card temp;
	temp = pack[index1];
	pack[index1] = pack[index2];
	pack[index2] = temp;
}
void makeSpecialCardMove(GameInfo* gameInfoP ,Card** upperCard)
{
	//make the move according to whice special card was chosen by the player
	switch ((*upperCard)->type)
	{
		case(changeDirection):
			//right = 1, left = -1. if we multiply the current direction by -1 it will change it
			gameInfoP->direction *= -1;
			return;
		case(plus):
			givePlayerAnotherTurn(gameInfoP);
			return;
		case(stop):
			stopNextPlayer(gameInfoP);
			return;
		case(changeColor):
			changeColorOfGame(gameInfoP, upperCard);
			return;
		default:
			openTaki(gameInfoP, upperCard);
	}
}
void openTaki(GameInfo* gameInfoP, Card** upperCard)
{
	char openTakiColor = (*upperCard)->color;
	int takiChoice;
	Player* activePlayer = &gameInfoP->allPlayers[gameInfoP->currentTurn];
	presentActivePlayerAndUpperCard(activePlayer, *upperCard);
	printf("please enter 0 if you want to finish your turn\n\
or 1 - %d if you want to put one of your cards in the middle\n", activePlayer->logPackSize);
	scanf(" %d", &takiChoice);
	//ask the player to put cards on top of the deck until he wants to stop or until a no color card is selected
	while (takiChoice != CLOSE_TAKI && activePlayer->pack[takiChoice - 1].type != changeColor)
	{
		//if the card that the user wanted to put on the table is not of the same color of the taki, or if the user typed a card that is nor in range, repeat the process
		if ((takiChoice < 0 || takiChoice > activePlayer->logPackSize) || (activePlayer->pack[takiChoice - 1].type != changeColor && activePlayer->pack[takiChoice - 1].color != openTakiColor))
		{
			printf("Invalid card! Try again.\n");
			printf("please enter 0 if you want to finish your turn\n\
or 1 - %d if you want to put one of your cards in the middle\n", activePlayer->logPackSize);
			scanf(" %d", &takiChoice);
			continue;
		}
		//else put the card on top of the deck and continue the process until the player wants to finish his turn
		putChosenCardOnTopOfDeck(activePlayer, takiChoice, upperCard);
		//if the player used his last card, stop asking for more cards
		if (activePlayer->logPackSize == EMPTY)
			break;
		presentActivePlayerAndUpperCard(activePlayer, *upperCard);
		printf("please enter 0 if you want to finish your turn\n\
or 1 - %d if you want to put one of your cards in the middle\n", activePlayer->logPackSize);
		scanf(" %d", &takiChoice);
	}
	//if the last card in the "open" taki was a special card, activate the card
	if ((*upperCard)->type != numberType)
		makeSpecialCardMove(gameInfoP, upperCard);
}
void changeColorOfGame(GameInfo* gameInfoP, Card** upperCard)
{
	int colorChoice;
	printf("please enter your color choice:\n1 - Red\n2 - Blue\n3 - Green\n4 - Yellow\n");
	scanf("%d", &colorChoice);
	//change the color of the game according to the players's choice
	switch (colorChoice)
	{
	case(1):
		(*upperCard)->color = RED_COLOR;
		break;
	case(2):
		(*upperCard)->color = BLUE_COLOR;
		break;
	case(3):
		(*upperCard)->color = GREEN_COLOR;
		break;
	case(4):
		(*upperCard)->color = YELLOW_COLOR;
		break;
	default:
		printf("error, try again\n");
		changeColorOfGame(gameInfoP, upperCard);
		return;
	}
	(*upperCard)->type = changeColor;
	(*upperCard)->num = SPECIAL_CARD_TYPE;
}
void givePlayerAnotherTurn(GameInfo* gameInfoP)
{
	//move the turn back by 1, before moving to the next player the program will advance the turn again and therfore the same player will get another turn
	if (gameInfoP->direction == RIGHT)
		gameInfoP->currentTurn--;
	else
		gameInfoP->currentTurn++;
	//if the player used the stop card as his last card
	if(gameInfoP->allPlayers[gameInfoP->currentTurn].logPackSize == EMPTY)
		getCardFromDeck(&gameInfoP->allPlayers[gameInfoP->currentTurn]);
}
void stopNextPlayer(GameInfo* gameInfoP)
{
	//advance the turn by 1, before moving to the next player the program will advance the turn again and therfore "stop" the next player
	if (gameInfoP->direction == RIGHT)
		gameInfoP->currentTurn++;
	else
		gameInfoP->currentTurn--;
	//if the stop card that the player used is his last card + there are only 2 players in game, give him another card
	if (gameInfoP->numOfPlayers == 2 && gameInfoP->allPlayers[gameInfoP->currentTurn].logPackSize == EMPTY)
		getCardFromDeck(&gameInfoP->allPlayers[gameInfoP->currentTurn]);
}
void getCardFromDeck(Player* activePlayer)
{
	//make space for another card
	if (activePlayer->logPackSize == activePlayer->phyPackSize)
	{
		Card* temp;
		activePlayer->phyPackSize *= 2;
		temp = (Card*)realloc(activePlayer->pack,activePlayer->phyPackSize * sizeof(Card));
		checkIfAllocationSuccessful((void*)temp);
		activePlayer->pack = temp;
	}
	//take another card
	activePlayer->pack[activePlayer->logPackSize] = createNewCard();
	activePlayer->logPackSize++;
}
bool isMoveValid(Card chosenCard, Card* upperCard)
{
	//if the card that the player chose is the same color of the upper card or no color card is always valid
	if ((chosenCard.color == NO_COLOR) || (upperCard->color == chosenCard.color))
		return true;
	//else, the color is not the same
	if (upperCard->type == chosenCard.type)
	{
		if (chosenCard.type == numberType)
			return upperCard->num == chosenCard.num;
		else
			return true;
	}
	//else, the chosen card is not of the same color and not of the same type, therefore the move is not legal
	return false;
}
int getCardChoice(int numOfChoices)
{
	int choice;
	printf("please enter %d if you want to take a card from the deck\n\
or 1 - %d if you want to put one of your cards in the middle:\n",TAKE_CARD_FROM_DECK ,numOfChoices);
	scanf("%d", &choice);
	//if the user cohse a card choice that is not within his pack range
	if ((choice < 0) || (choice > numOfChoices))
	{
		printf("Invalid card! try again!!!!!!!!!!\n");
		return getCardChoice(numOfChoices);
	}
	else
		return choice;
}
void presentActivePlayerAndUpperCard(Player* activePlayer, Card* upperCard)
{
	printf("\nUpper card:\n");
	printCard(upperCard);
	printf("%s's turn:\n\n", activePlayer->name);
	printPack(activePlayer);
}
void advanceTurnToNextPlayer(GameInfo* gameInfoP)
{
	//advance the turn to the next player
	if (gameInfoP->direction == RIGHT)
		gameInfoP->currentTurn++;
	else
		gameInfoP->currentTurn--;
}
void updateCurrentTurn(GameInfo* gameInfoP)
{
	//if the turn is of someone within the valid range
	if (0 <= gameInfoP->currentTurn && gameInfoP->currentTurn < gameInfoP->numOfPlayers)
		return;
	if (gameInfoP->direction == RIGHT)
	{
		//if the last player have played his turn and the direction is right, the first player is next
		if (gameInfoP->currentTurn == gameInfoP->numOfPlayers)
			gameInfoP->currentTurn = 0;
		//if the turn of the last player was skipped and the direction is right, the second player is next
		else if(gameInfoP->currentTurn == gameInfoP->numOfPlayers + 1)
			gameInfoP->currentTurn = 1;
	}
	else
	{
		//if the first player have played his turn and the direction is left, the last player is next
		if (gameInfoP->currentTurn == -1)
			gameInfoP->currentTurn = gameInfoP->numOfPlayers - 1;
		//if the turn of the first player was skipped and the direction is left, the second from last player is next
		else if (gameInfoP->currentTurn == -2)
			gameInfoP->currentTurn = gameInfoP->numOfPlayers - 2;
	}
}
Card createStartingCard()
{
	Card startingCard;
	startingCard = createNewCard();
	//starting card has to be a number card
	while (startingCard.type != numberType)
		startingCard = createNewCard();
	return startingCard;
}
void printCard(Card* card)
{
	printf("*********\n*       *\n*");
	switch (card->type)
	{
	case(numberType):
		printf("   %d   ", card->num);
		break;
	case(plus):
		printf("   +   ");
		break;
	case(stop):
		printf("  STOP ");
		break;
	case(changeDirection):
		printf("  <->  ");
		break;
	case(taki):
		printf("  TAKI ");
		break;
	default:
		printf(" COLOR ");
		break;
	}
	printf("*\n*   %c   *\n*       *\n*********\n\n", card->color);
}
void printPack(Player* player)
{
	int i;
	for (i = 0; i < player->logPackSize; i++)
	{
		printf("Card #%d:\n", i + 1);
		printCard(&player->pack[i]);
	}
}
char getRandomCardColor()
{
	//return a number between 0 and 3 which represent the card color
	switch (rand() % NUMBER_OF_COLORS)
	{
		case(0):
			return RED_COLOR;
		case(1):
			return BLUE_COLOR;
		case(2):
			return YELLOW_COLOR;
		default:
			return GREEN_COLOR;
	}
}
Card createNewCard()
{
	Card newCard;
	//get type of card
	newCard.type = rand() % NUMBER_OF_TYPES;
	//get color of card
	if (newCard.type == changeColor)
		newCard.color = NO_COLOR;
	else
		newCard.color = getRandomCardColor();
	//get number of card
	if (newCard.type == numberType)
		newCard.num = (rand() % 9) + 1;
	else
		newCard.num = SPECIAL_CARD_TYPE;
	return newCard;
}
void givePlayerNewPack(Player* player)
{
	int i;
	//allocate a new array of cards
	player->pack = (Card*)malloc(PACK_SIZE_IN_START * sizeof(Card));
	checkIfAllocationSuccessful((void*)player->pack);
	player->logPackSize = PACK_SIZE_IN_START;
	player->phyPackSize = PACK_SIZE_IN_START;
	//fill the pack with new random cards
	for (i = 0; i < player->logPackSize; i++)
		player->pack[i] = createNewCard();
}
void getPlayersInfo(GameInfo* game)
{
	int i;
	for (i = 0; i < game->numOfPlayers; i++)
	{
		printf("please enter the first name of player #%d:\n", i+1);
		scanf(" %s", game->allPlayers[i].name);
		givePlayerNewPack(&(game->allPlayers[i]));
	}
}

int getNumOfPlayers()
{
	int num;
	printf("********** Welcome to TAKI game !!! **********\nplease enter the number of players:\n");
	scanf("%d", &num);
	//ask the user for number of players until a positive number is typed
	while (num <= 0)
	{
		printf("error! try again\n");
		scanf("%d", &num);
	}
	return num;
}
void checkIfAllocationSuccessful(void* pointer)
{
	if (pointer == NULL)
	{
		printf("memory allocation failed");
		exit(1);
	}
}
void freeAll(GameInfo* gameInfoP)
{
	int i;
	for (i = 0; i < gameInfoP->numOfPlayers; i++)
		free(gameInfoP->allPlayers[i].pack);
	free(gameInfoP->allPlayers);
}
void getGameInfo(GameInfo* gameInfoP)
{
	gameInfoP->numOfPlayers = getNumOfPlayers();
	//make a dynamic array of players
	gameInfoP->allPlayers = (Player*)malloc(gameInfoP->numOfPlayers * sizeof(Player));
	checkIfAllocationSuccessful((void*)gameInfoP->allPlayers);
	//get the info of all the players
	getPlayersInfo(gameInfoP);
	//start the game with the first player and with the game going in clock wise direction
	gameInfoP->currentTurn = 0;
	gameInfoP->direction = RIGHT;
}
