#include <stdio.h>
#include <stdlib.h>
#include "takis.h"
int main()
{
	srand(time(NULL));
	int numOfPlayers = welcome(), current = 0, choice, direction = RIGHT, counterArr[15], greenSize = 0, *ptrToGreem = &greenSize;
	Card** organizedGreens;
	Player** list = (Player**)malloc(sizeof(Player*) * numOfPlayers);
	Player* activePlayer;
	Card* activeCard, *twinCard;
	resetArr(counterArr);
	fillList(list, numOfPlayers);
	checkPack(counterArr, list, numOfPlayers);
	//printList(list, numOfPlayers);
	activeCard = startingCard();
	addStat(counterArr, activeCard);
	twinCard = createIdenticalCard(activeCard);
	do
	{
		if (current == numOfPlayers && direction == RIGHT)
			current = 0;
		else if ((current == numOfPlayers + 1) && (direction == RIGHT))
			current = 1;
		else if(current == -1 && direction == LEFT)
			current = numOfPlayers - 1;
		else if (current == -2 && direction == LEFT)
			current = numOfPlayers - 2;
		activePlayer = list[current];
		printf("%s's cards:\n\n", activePlayer->name);
		printPack(activePlayer);
		printf("Upper card:\n");
		printCard(activeCard);
		printf("%s's turn:\n\n", activePlayer->name);
		choice = chooseMove(activePlayer->packSize);
		//migh have problem
		if ((activePlayer->pack[choice - 1] != COLCHANGE) && (choice != 0) && (!checkValidMove(activePlayer->pack[choice - 1], activeCard)))
		{
			printf("Invalid card! Try again.\n");
			continue;
		}
		copyDeatils(twinCard, activeCard);
		activeCard = turn(activePlayer, choice,activeCard,counterArr);
		if (activeCard == NULL)
		{
			activeCard = twinCard;
		}
		else if (activeCard->num == DIRCHANGE)
		{
			if (direction == RIGHT)
				direction = LEFT;
			else
				direction = RIGHT;
		}
		else if (activeCard->num == PLUS)
			continue;
		else if (activeCard->num == STOP)
		{
			if (direction == RIGHT)
				current += 2;
			else
				current -= 2;
			continue;
		}
		if (direction == RIGHT)
			current++;
		else
			current--;
	} while (activePlayer->packSize != 0);
	printf("the winner is... %s! Congratulations!\n\n", activePlayer->name);
	printStats(counterArr);
	return 0;
}