#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "takis.h"
Card* turn(Player* player, int choice, Card* upperCard, int arr[])
{
	Card* chosenCard = player->pack[choice - 1], * lastCard = upperCard;
	if(choice == 0)
	{ 
		(player->packSize)++;
		player->pack = (Card**)realloc(player->pack, sizeof(Card*) * player->packSize);
		player->pack[(player->packSize) - 1] = createCard();
		addStat(arr, player->pack[(player->packSize) - 1]);
		return NULL;
	}
	else if (chosenCard->num == COLCHANGE)
	{
		int colorChoice;
		printf("please enter your color choice:\n1 - Red\n2 - Blue\n3 - Green\n4 - Yellow\n");
		scanf("%d", &colorChoice);
		chosenCard->color = colorChoice;
	}
	else if (chosenCard->num == TAKI)
	{
		organizePack(player, choice - 1);
		chosenCard = createIdenticalCard(chosenCard);
		(player->packSize)--;
		player->pack = (Card**)realloc(player->pack, sizeof(Card*) * player->packSize);
		int takiChoice;
		do 
		{
			printf("please enter 0 if you want to finish your turn\n\
or 1 - %d if you want to put one of your cards in the middle\n", player->packSize);
			scanf("%d", &takiChoice);
			if (!takiChoice)
				break;
			chosenCard = player->pack[takiChoice - 1];
			if (!checkValidMove(chosenCard, lastCard))
			{
				printf("Invalid card! try again\n");
				continue;
			}
			lastCard = chosenCard;
			organizePack(player, takiChoice - 1);
			(player->packSize)--;
		} while (takiChoice != 0);
		if (lastCard == upperCard)
		{
			free(upperCard);
			return chosenCard;
		}
		lastCard = createIdenticalCard(lastCard);
		player->pack = (Card**)realloc(player->pack, sizeof(Card*) * player->packSize);
		free(upperCard);
		return lastCard;
	}
	organizePack(player, choice - 1);
	(player->packSize)--;
	chosenCard = createIdenticalCard(chosenCard);
	if(player->packSize != 0)
		player->pack = (Card**)realloc(player->pack, sizeof(Card*) * player->packSize);
	free(upperCard);
	return chosenCard;
}
Card** getCardsByColor(Player** list, int numOfPlayers, Color wantedColor, int* pSize)
{
	int i, j, counter = 0;
	Card** organizedCards = (Card**)malloc(sizeof(Card*)), *current;
	for (i = 0; i < numOfPlayers; i++)
	{
		for (j = 0; j < list[i]->packSize; j++)
		{
			current = list[i]->pack[j];
			if (current->color == wantedColor)
			{
				organizedCards[counter] = createIdenticalCard(current);
				counter++;
				organizedCards = (Card**)realloc(organizedCards, (sizeof(Card*) * counter) + 1);
			}
		}
	}
	(*pSize) = counter;
	return organizedCards;
}
void printStats(int arr[])
{
	int index, i;
	printf("******** Game Stats ***********\nCard # | Frequency\n__________________\n");
	for (i = 1; i < 15; i++)
	{
		index = maxOfArray(arr);
		switch (index)
		{
		case(PLUS):
			printf("   +  |   %d    \n", arr[index]);
			break;
		case(STOP):
			printf(" STOP |   %d    \n", arr[index]);
			break;
		case(DIRCHANGE):
			printf("  <-> |   %d    \n", arr[index]);
			break;
		case(TAKI):
			printf(" TAKI |   %d    \n", arr[index]);
			break;
		case(COLCHANGE):
			printf(" COLOR |   %d    \n", arr[index]);
			break;
		default:
			printf("   %d  |   %d    \n", index, arr[index]);
			break;
		}
		arr[index] = -100;
	}
}
int maxOfArray(int arr[])
{
	int i, index = 1;
	for (i = 1; i < 15; i++)
	{
		if (arr[i] >= arr[index])
			index = i;
	}
	return index;
}
void checkPack(int arr[], Player** list, int numOfPlayers)
{
	int i,j;
	for (i = 0; i < numOfPlayers; i++)
		for (j = 0; j < list[i]->packSize; j++)
			addStat(arr,list[i]->pack[j]);
}
void addStat(int arr[], Card* card)
{
	(arr[card->num])++;
}
void copyDeatils(Card* dest, Card* src)
{
	dest->color = src->color;
	dest->num = src->num;
}
Card* createIdenticalCard(Card* card)
{
	Card* identicalCard = (Card*)malloc(sizeof(Card));
	identicalCard->color = card->color;
	identicalCard->num = card->num;
	return identicalCard;
}
void organizePack(Player* player, int index)
{
	int i; Card* temp;
	for (i = index; i < player->packSize; i++)
	{
		temp = player->pack[i];
		player->pack[i] = player->pack[i+1];
		player->pack[i + 1] = temp;
	}
	//myabe free
}
int checkValidMove(Card* chosenCard, Card* upperCard)
{
	if (chosenCard->color == none)
		return VALID;
	if ((upperCard->color != chosenCard->color) && (upperCard->num != chosenCard->num))
		return NO_VALID;
	return VALID;
}
int chooseMove(int numOfChoices)
{
	int choice;
	printf("please enter 0 if you want to take a card from the deck\n\
or 1 - %d if tou want to put one of your cards in the middle:\n", numOfChoices);
	scanf("%d", &choice);
	if ((choice < 0) || (choice > numOfChoices))
	{
		printf("Invalid card! try again!!!!!!!!!!\n");
		return chooseMove(numOfChoices);
	}
	else
		return choice;
}
void printPack(Player* player)
{
	int i;
	for (i = 0; i < player->packSize; i++)
	{
		printf("Card #%d:\n", i + 1);
		printCard(player->pack[i]);
	}
}
Card* startingCard()
{
	Card* startCard = createCard();
	if (startCard->num > 9)
	{
		free(startCard);
		return startingCard();
	}
	else
		return startCard;
}
void fillList(Player** list, int size)
{
	int i;
	for (i = 0; i < size; i++)
	{
		list[i] = (Player*)malloc(sizeof(Player));
		setUpPlayer(list[i],i + 1);
	}
}
void setUpPlayer(Player* player,int serialNum)
{
	int i;
	player->pack = (Card**)malloc(sizeof(Card*) * START);
	player->packSize = START;
	srand(time(NULL));
	for (i = 0; i < START; i++)
		player->pack[i] = createCard();
	printf("please enter the first name of player #%d:\n", serialNum);
	scanf(" %s", player->name);
}
Card* createCard()
{
	Card* newCard = (Card*)malloc(sizeof(Card));
	newCard->num = (rand() % 14) + 1;
	if (newCard->num == COLCHANGE)
		newCard->color = none;
	else
		newCard->color = (rand() % 4) + 1;
	return newCard;
}
int welcome()
{
	int num;
	printf("********** Welcome to TAKI game !!! **********\nplease enter the number of players:\n");
	do
	{
		scanf("%d", &num);
		if (num < 0)
			printf("error! try again\n");
	} while (num < 0);
	return num;
}
void printList(Player** list, int size)
{
	int i, j;
	for (i = 0; i < size; i++)
	{
		printf("player #%d is %s and his cards are:\n", i + 1, list[i]->name);
		for (j = 0; j < list[i]->packSize; j++)
			printCard(list[i]->pack[j]);
	}
}
void printCard(Card* card)
{
	printf("*********\n*       *\n*");
	switch (card->num)
	{
	case(PLUS):
		printf("   +   ");
		break;
	case(STOP):
		printf("  STOP ");
		break;
	case(DIRCHANGE):
		printf("  <->  ");
		break;
	case(TAKI):
		printf("  TAKI ");
		break;
	case(COLCHANGE):
		printf(" COLOR ");
		break;
	default:
		printf("   %d   ", card->num);
		break;
	}
	printf("*\n*");
	switch (card->color)
	{
	case(none):
		printf("       *\n*       *\n*********\n");
		return;
	case(red):
		printf("   R   ");
		break;
	case(blue):
		printf("   B   ");
		break;
	case(green):
		printf("   G   ");
		break;
	case(yellow):
		printf("   Y   ");
		break;
	}
	printf("*\n*       *\n*********\n");
}
void resetArr(int arr[])
{
	int i;
	arr[0] = -100;
	for (i = 1; i < 15; i++)
		arr[i] = 0;
}
int game(Player** list, int numOfPlayers)
{
	Player* activePlayer;
	Card* activeCard;
	activeCard = createCard();

}