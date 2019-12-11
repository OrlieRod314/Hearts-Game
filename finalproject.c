#define _CRT_SECURE_NO_WARNINGS
#define SPADE '\x06'
#define CLUB '\x05'
#define HEART '\x03'
#define DIAMOND '\x04'
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

//Structure for Card
typedef struct card_s {
	char suit;               //Spade, Diamond, Club, or Heart
	int face;                //Face Value: 1-13
	struct card_s* next;     //Address of the next card
} card;

//Structure for Card
typedef struct player_s {
	char name[25];           //Player’s/Computer’s name
	int points;              //Total penalty points
	int winner_of_turn;      //1 if yes; 0 if no. Keeps track of the turn winners
	int is_player;			 //1 if yes; 0 if no. Keeps track of the player
	card* hand;              //Address to head node of their hands
	struct player_s* next;   //Address to next player
} player;


//Sets up the game. Error checking required
void setup(card** deck, player* playerHN, int* numPlayers);

//Creates a linked list of cards
void create_deck(card* cardHN, int num);

//Initializes the deck of cards
void initialize_deck(card* deckHN);

//Prints cards
void print_cards(card* cardHN);

//Creates a linked list of players
void player_list(player* playerHN, int num);

//Shuffles the deck of cards
void shuffle_deck(card** deckHN);

//Removes a card from a linked list and returns the removed card’s address
card* remove_card(card** deckHN, int position);

//Adds a card into a linked list
void add_card(card** cardHN, card* selected, int position);

//Deals cards to the players
void deal_cards(card* deck, player* HN, int numPlayers);

//Counts cards in a linked list
int count_cards(card* HN);

//Passes the cards to another player. Error checking required
void pass_cards(player* HN, int numPlayers);

//Moves pointer to point at starting player
void player_start(player** HN, int turn, int numPlayers);

//Selects a card to play. Removes it from players hand and adds it to table linked list. Error checking required
void select_card(player* HN, card** table, int numPlayers, int* heart_played, int turn);

//Compares cards on the table. Winner is updated, points are given, and table is cleared. 
void compare_cards(player** HN, card** table, int numPlayers, int* heart_played);

//Sorts the cards in a given linked list
void sort_cards(card** cardHN);

//Swaps cards
void swap_adjacent_cards(card** cardHN, card* card1, card* card2);

//Swaps numbers
void swap_nums(int* num1, int* num2);

//Looks for a specific card in a linked list based on face value and suit
int search_for_card(card* cardHN, int face, char suit);

//Makes sure selected card is valid
int is_valid_selection(card* table, int heart_played, int playerChoice, card* playerHand, int turn, int is_player);


int main(void) {
	player* playerHN, * winner; //PlayerHN holds linked list of players and winner is a temporary player pointer
	card* deck, * table; //Deck stores the cards in the deck and table stores the played cards
	int numPlayers, heart_played = 0, turn; //numPlayers stores the number of players, heart_played stores whether or not a heart was played, turn keeps track of turns
	char cont = '\n'; //Continue character
	//Loop for multiple games
	while (cont != 'q') {
		turn = 1; //Starting at turn 1
		playerHN = (player*)malloc(sizeof(player)); //Initializes first node of player
		deck = (card*)malloc(sizeof(card)); //Initializes first node of deck
		table = NULL; //Initialize table

		setup(&deck, playerHN, &numPlayers); //Sets up the game
		printf("\n");

		pass_cards(playerHN, numPlayers); //Passes 3 cards
		while (playerHN->is_player != 1) //Moves to player
			playerHN = playerHN->next;
		printf("New Hand:\n"); //Prints out the players new hand
		sort_cards(&(playerHN->hand)); //Sorts the cards in the players hand to make it easier to read
		print_cards(playerHN->hand); //Prints the new hand out
		printf("\n\n");

		player_start(&playerHN, turn, numPlayers); //Determines which player has the 2 of clubs

		while (count_cards(playerHN->hand) != 0) { //Loops as long as the players have cards
			for (int i = 0; i < numPlayers; i++) { //Lets each player select a card
				select_card(playerHN, &table, numPlayers, &heart_played, turn); //Select card function
				print_cards(table); //Prints out the played cards after each player plays a card
				playerHN = playerHN->next; //Moves to next player
				turn++; //Increments turn since only turn 1 matters
			}
			compare_cards(&playerHN, &table, numPlayers, &heart_played); //Compares the cards playes
		}
		winner = playerHN; //Sets winner equal to the first player in the player linked list
		for (int i = 0; i < numPlayers; i++) { //Loops through each player
			if (winner->points > playerHN->points) //Least amount of points wins 
				winner = playerHN; //Sets winner to player with the least amount of points
			playerHN = playerHN->next; //Moves to next player
		}
		printf("\n\nTHE WINNER IS %s WITH %d POINTS\n\n", winner->name, winner->points); //Prints winning screen

		for (int i = 0; i < numPlayers; i++) { //Frees player linked list 
			winner = playerHN->next;
			free(playerHN);
			playerHN = winner;
		}
		free(deck); //Frees deck variable
		cont = '\n';
		printf("Do you want to play again (q to quit): "); //Asks user if they want to continue
		while (cont == '\n')
			scanf("%c", &cont);
	}

	return 0;
}


//Done
void setup(card** deck, player* playerHN, int* numPlayers) {
	//Beginning of the Game
	char name[25]; //Stores players name until player linked list is created
	printf("Enter your name: ");
	scanf("%s", name);

	printf("\n\n%s, lets play Hearts!\n\n", name); //Starting text

	do {
		printf("Enter the number of players: ");
		scanf("%d", numPlayers);//Gets number of players
	} while (!(*numPlayers == 3 || *numPlayers == 4));

	create_deck(*deck, 52); //Creates a deck linked list
	initialize_deck(*deck); //Initializes the deck
	if (*numPlayers == 3) //Removes the 2D if there are only 3 players
		remove_card(deck, 41);
	shuffle_deck(deck); //Shuffles the deck

	player_list(playerHN, *numPlayers); //Creates player linked list
	deal_cards(*deck, playerHN, *numPlayers); //Deals cards to players

	for (int i = 0; i < *numPlayers; i++) { //Sets the names of the players
		sort_cards(&(playerHN->hand)); //Sorts player's hand
		if (i == 0) {
			strcpy(playerHN->name, name); //User's name
			playerHN->is_player = 1; //Is player
			printf("%s's Deck\n", playerHN->name);
			print_cards(playerHN->hand); //Prints their hands
		}
		else if (i == 1) {
			strcpy(playerHN->name, "Computer 1");
			playerHN->is_player = 0;
		}
		else if (i == 2) {
			strcpy(playerHN->name, "Computer 2");
			playerHN->is_player = 0;
		}
		else if (i == 3) {
			strcpy(playerHN->name, "Computer 3");
			playerHN->is_player = 0;
		}
		playerHN->points = 0;
		playerHN = playerHN->next; //Moves to next player
	}
}

//Done
void create_deck(card* cardHN, int num) {
	//Creates a linked list of cards
	card* temp;
	temp = cardHN;
	for (int i = 1; i <= num; i++) {
		if (i < num) {
			temp->next = (card*)malloc(sizeof(card)); //Allocates memory
			temp = temp->next; //Moves to next node
		}
		else {
			temp->next = NULL; //Ending node points to NULL
		}
	}
}

//Done
void initialize_deck(card* deckHN) {
	//Initializes the deck of cards
	for (int i = 0; i < 4; i++) { //Suits
		for (int j = 0; j < 13; j++) { //Face values
			deckHN->face = j + 1;
			switch (i) {
			case 0:
				deckHN->suit = SPADE;   //Suit = SPADE
				break;
			case 1:
				deckHN->suit = HEART;   //Suit = HEART
				break;
			case 2:
				deckHN->suit = CLUB;    //Suit = CLUB
				break;
			case 3:
				deckHN->suit = DIAMOND; //Suit = DIAMOND
				break;
			default: //If something bad happens
				printf("\n**Initialize Deck Function Error**\n");
				break;
			}
			deckHN = deckHN->next;
		}
	}
}

//Done
void print_cards(card* cardHN) {
	//Prints cards. Face values are changed if they are 1, 11, 12, or 13.
	card* temp = cardHN;
	printf("\n");
	//Card number
	for (int i = 0; i < count_cards(cardHN); i++)
		printf("  %2d   ", i + 1);
	printf("\n");
	//Top of the card
	for (int i = 0; i < count_cards(cardHN); i++)
		printf(" ----- ");
	printf("\n");
	//Top left of the card
	for (int j = 0; j < count_cards(cardHN); j++) {
		while (temp != NULL) {
			switch (temp->face) {
			case 1:
				printf("|A%c   |", temp->suit); //Prints ace and suit
				break;
			case 11:
				printf("|J%c   |", temp->suit); //Prints jack and suit
				break;
			case 12:
				printf("|Q%c   |", temp->suit); //Prints queen and suit
				break;
			case 13:
				printf("|K%c   |", temp->suit); //Prints king and suit
				break;
			case 10:
				printf("|10%c  |", temp->suit); //Prints 10 and suit
				break;
			default: //Prints face value and suit
				printf("|%d%c   |", temp->face, temp->suit);
				break;
			}
			temp = temp->next; //Moves to next card
		}
	}
	printf("\n");
	//Prints the two middle lines on the card
	for (int k = 0; k < 2; k++) {
		for (int i = 0; i < count_cards(cardHN); i++)
			printf("|     |");
		printf("\n");
	}
	temp = cardHN; //Resets temp
	//Bottom right of the card
	for (int j = 0; j < count_cards(cardHN); j++) {
		while (temp != NULL) {
			switch (temp->face) {
			case 1:
				printf("|   A%c|", temp->suit); //Prints ace and suit
				break;
			case 11:
				printf("|   J%c|", temp->suit); //Prints jack and suit
				break;
			case 12:
				printf("|   Q%c|", temp->suit); //Prints queen and suit
				break;
			case 13:
				printf("|   K%c|", temp->suit); //Prints king and suit
				break;
			default: //Prints face value and suit
				printf("|  %2d%c|", temp->face, temp->suit);
				break;
			}
			temp = temp->next; //Moves to next card
		}
	}
	printf("\n");
	//End of the card
	for (int i = 0; i < count_cards(cardHN); i++)
		printf(" ----- ");
	printf("\n\n");
}

//Done
void player_list(player* playerHN, int num) {
	//Creates a player linked list
	player* temp = NULL;
	temp = playerHN;
	for (int i = 1; i <= num; i++) {
		if (i < num) {
			temp->next = (player*)malloc(sizeof(player)); //Allocates memory
			temp = temp->next; //Moves to next player
		}
		else {
			temp->next = playerHN; //Makes the list circular
		}
	}
}

//Done
void shuffle_deck(card** deckHN) {
	//Shuffles the given deck
	srand(time(NULL)); //Seed
	int rand1, rand2;
	card* holder;
	for (int i = 0; i < 500; i++) {
		rand1 = (rand() % count_cards(*deckHN)) + 1; //Random number from 1 - 52
		rand2 = (rand() % count_cards(*deckHN)) + 1; //Random number from 1 - 52
		holder = remove_card(deckHN, rand1); //Removes card from a random spot
		add_card(deckHN, holder, rand2); //Adds the removed card to a random spot
	}
}

//Done
card* remove_card(card** deckHN, int position) {
	//Removes nodes and returns their address
	card* temp, * temp2;
	temp = *deckHN;
	if (position == 1) { //If removing from position 1. Moves head node
		*deckHN = temp->next; //Makes head node the 2nd node
		temp->next = NULL; //Sets the initial head node to point to NULL
		return temp; //Returns the address of temp
	}
	else if (position <= count_cards(*deckHN)) { //If at another position besides 1
		for (int i = 1; i < position - 1; i++) //Sets temp equal to the address of the node before the node that wants to be removed
			temp = temp->next;
		temp2 = temp->next; //Sets temp2 to the node to be removed
		temp->next = temp2->next; //Makes temp skip temp2
		temp2->next = NULL; //temp2 points to NULL
		return temp2; //Returns the address of temp2
	}
	else { //If something goes wrong
		printf("\n**Remove Card Function Error**\n");
		return NULL;
	}
}

//Done
void add_card(card** cardHN, card* selected, int position) {
	//Adds a card to a linked list
	card* temp, * temp2;
	temp = *cardHN;
	temp2 = selected;

	if (position == 1) { //If desired position is 1
		temp2->next = *cardHN; //Selected card points to head node
		*cardHN = temp2; //Head node updated to selected card
	}
	else if (position <= count_cards(*cardHN)) { //Middle positions
		for (int i = 1; i < position - 1; i++)
			temp = temp->next; //Moves card to card just before the position
		temp2->next = temp->next; //Selected card points to next card
		temp->next = selected; //Current card points to the selected card
	}
	else if (position == count_cards(*cardHN) + 1) {
		while (temp->next != NULL)
			temp = temp->next;
		temp->next = temp2;
		temp2->next = NULL;
	}
	else //If something goes wrong
		printf("\n**Add Card Function Error**\n");
}

//Done
void deal_cards(card* deck, player* HN, int numPlayers) {
	//Sets the head nodes of each players' linked list hand
	for (int i = 0; i < numPlayers; i++) {
		HN->hand = remove_card(&deck, 1);
		HN = HN->next;
	}
	//Adds the other cards to the list
	while (count_cards(deck) != 0) {
		add_card(&(HN->hand), remove_card(&deck, 1), count_cards(HN->hand) + 1); //Adds card to the end of the linked list
		HN = HN->next;
	}
}

//Done
int count_cards(card* HN) {
	//Increments i until HN == NULL to count each node in the card linked list
	int i = 0;
	while (HN != NULL) {
		HN = HN->next;
		i++;
	}
	return i;
}

//Done
void sort_cards(card** cardHN) {
	//Bubble Sorting Algorithm
	card* temp;
	int cont;
	do {
		temp = *cardHN; //Resets temp
		cont = 1; //Resets cont
		while (temp->next != NULL && temp != NULL) { //Swaps adjacent cards if the second is larger than the first
			if (temp->face > (temp->next)->face) {
				swap_adjacent_cards(cardHN, temp, temp->next);
				cont = 0; //Set to zero if a swap occurs
			}
			if (temp->next != NULL) //Makes sure temp != NULL
				temp = temp->next;
		}
	} while (cont != 1); //Only way to exit loop is for no swaps to occur
}

//Done
void swap_adjacent_cards(card** cardHN, card* card1, card* card2) {
	int card1_pos = -1, card2_pos = -1;
	card1_pos = position_of_card(*cardHN, card1); //Stores location of the cards
	card2_pos = position_of_card(*cardHN, card2);
	card* temp1;
	if (card1_pos != -1 && card2_pos != -1) {
		temp1 = remove_card(cardHN, card2_pos); //Temporary variable stores the location of the removed card
		add_card(cardHN, temp1, card1_pos); //Added back in to the deck before card1
	}
}

//Done
int position_of_card(card* HN, card* find) {
	//Returns the position of the card in the linked list
	int i = 1;
	while (HN != NULL) {
		if ((find->face == HN->face) && (find->suit == HN->suit)) //Checks if face value and suit match
			return i; //Returns position if they do
		i++;
		HN = HN->next;
	}
	return -1; //Returns if no cards match
}

//Done
void pass_cards(player* HN, int numPlayers) {
	int pos_card[3]; //Array that holds the position of cards
	for (int i = 0; i < numPlayers; i++) { //Loops through the players
		if (HN->is_player == 1) { //For the player
			do {
				printf("Enter three cards you wish to pass (1-%d): ", 52 / numPlayers); //Prompts user to pick three cards
				scanf("%d %d %d", &pos_card[0], &pos_card[1], &pos_card[2]);
			} while (pos_card[0] == pos_card[1] || pos_card[1] == pos_card[2] || pos_card[0] == pos_card[2] || pos_card[0] < 1 || pos_card[1] < 1 || pos_card[2] < 1 || pos_card[0] > 52 / numPlayers || pos_card[1] > 52 / numPlayers || pos_card[2] > 52 / numPlayers); //Makes sure the choices are valid
			//Sorts the cards from largest to smallest
			if (pos_card[0] < pos_card[1])
				swap_nums(&pos_card[0], &pos_card[1]);
			if (pos_card[0] < pos_card[2])
				swap_nums(&pos_card[0], &pos_card[2]);
			if (pos_card[1] < pos_card[2])
				swap_nums(&pos_card[1], &pos_card[2]);
			//Adds a card to the next players hand
			for (int j = 0; j < 3; j++)
				add_card(&((HN->next)->hand), remove_card(&(HN->hand), pos_card[j]), j + 1);
		}
		else { //For the computers
			//Computers choose 3 random cards
			for (int k = 0; k < 3; k++)
				add_card(&((HN->next)->hand), remove_card(&(HN->hand), (rand() % (count_cards(HN->hand) - 3)) + 4), k + 1);
		}
		HN = HN->next; //Moves to next player
	}
}

//Done
void swap_nums(int* num1, int* num2) {
	//Switches the number values fed in
	int temp;
	temp = *num1;
	*num1 = *num2;
	*num2 = temp;
}

//Done
void player_start(player** HN, int turn, int numPlayers) {
	if (turn == 1) { //For starting turn
		while (search_for_card((*HN)->hand, 2, CLUB) != 1) //Looks for 2C in each of the players hands
			*HN = (*HN)->next; //Updates head node until card is found
	}
}

//Done
int search_for_card(card* cardHN, int face, char suit) {
	int found = 0; //Assumes no card
	for (int i = 0; i < count_cards(cardHN); i++) { //Looks through card linked list
		if (face == cardHN->face && suit == cardHN->suit) //If face and suit match sets found equal to 1
			found = 1;
		cardHN = cardHN->next; //Moves to the next card
	}
	return found; //Returns the value of found
}

//Done
void select_card(player* HN, card** table, int numPlayers, int* heart_played, int turn) {
	//Puts the player's and computers' selected cards into a separate linked list
	int playerChoice, index = 1;
	card* playersCard;
	printf("%s: ", HN->name); //Prints name so player can determine who played which card
	if (HN->is_player == 1) {
		printf("Current Hand:\n"); //Prints out the player's current hand 
		print_cards(HN->hand);
		do {
			printf("Select a card(1 - %d): ", count_cards(HN->hand)); //Player selects a card
			scanf("%d", &playerChoice);
		} while (playerChoice < 1 || playerChoice > count_cards(HN->hand) || !is_valid_selection(*table, *heart_played, playerChoice, HN->hand, turn, HN->is_player)); //Makes sure player hand stays within bounds
		if (*table == NULL)
			*table = remove_card(&(HN->hand), playerChoice); //Sets the head node of the table to the player-removed card
		else
			add_card(table, remove_card(&(HN->hand), playerChoice), count_cards(*table) + 1); //Adds card to the end of the table linked list
	}
	else {
		//AI Code
		while (!is_valid_selection(*table, *heart_played, index, HN->hand, turn, HN->is_player)) //Picks first valid card in hand
			index++; //Increments index if not valid
		if (*table == NULL)
			*table = remove_card(&(HN->hand), index); //Sets the head node of the table to the computer-removed card
		else
			add_card(table, remove_card(&(HN->hand), index), count_cards(*table) + 1); //Adds card to the end of the table linked list
	}
}

//Done
void compare_cards(player** HN, card** table, int numPlayers, int* heart_played) {
	card* first = *table, * others = *table, * temp = *table;
	player* players = *HN, * winner = *HN;
	int pen_points = 0;
	do {
		if ((first->suit == others->suit) && (first->face < others->face || others->face == 1)) { //Checks if card suits match, if so checks face value
			winner = players; //Sets winner of hand to the player with the highest face value card of suit
			first = others; //Updates largest card
			if (first->face == 1) //Breaks if ace since nothing can beat ace
				break;
		}
		others = others->next; //Moves to next card
		players = players->next; //Moves to next player
	} while (others != NULL);
	*HN = winner; //Sets player headnode to winner so they start the next hand

	for (int i = 0; i < numPlayers; i++) { //Loops through table removing cards
		temp = remove_card(table, 1); //temp stores the removed cards
		if (temp->suit == HEART) {
			pen_points++; //Increments penalty points if a heart was played
			*heart_played = 1; //Hearts have been broken
		}
		else if ((temp->suit == SPADE) && (temp->face == 12))
			pen_points += 13; //Increments penalty points by 13 since its the Queen of Spades
		free(temp); //Frees the card from memory
	}
	(*HN)->points += pen_points; //Adds to players' points
	printf("\n*%s won the trick and gained %d points*\n\n", (*HN)->name, pen_points); //States who won trick and how many points they gained
	*table = NULL; //Resets table to NULL
}

//Done
int is_valid_selection(card* table, int heart_played, int playerChoice, card* playerHand, int turn, int is_player) {
	int has_suit = 0, valid = 0;
	card* selected_card = playerHand, * temp = playerHand;
	for (int i = 0; i < playerChoice - 1; i++)
		selected_card = selected_card->next; //Sets selected_card to the address of the player selected card
	if (turn == 1) { //Makes sure the first card played is the 2 of clubs
		if (selected_card->face == 2 && selected_card->suit == CLUB) //If so, valid move
			valid = 1;
		else {
			if (is_player) //If not, invalid
				printf("Card must be the 2%c\n", CLUB); //Prints message for player, not computer
			valid = 0;
		}
	}
	else if (table == NULL) { //First move per trick
		if (heart_played) //Checks if hearts have been broken
			valid = 1; //If so, valid move
		else if (selected_card->suit != HEART) { //If not, but heart wasn't chosen, valid move
			valid = 1;
		}
		else {
			if (is_player) //If heart was played without hearts being broken, invalid move
				printf("%c cannot be played until hearts have been broken\n", HEART); //Message for player
			valid = 0;
		}
	}
	else if (table != NULL) { //Other moves besides first
		while (temp != NULL) {
			if (temp->suit == table->suit) //Checks if hand has a card of matching suit
				has_suit = 1; //If so, has_suit = 1
			temp = temp->next; //Moves through cards
		}
		if (has_suit) { //If has suit, checks if selected card is matching
			if (selected_card->suit == table->suit) //Valid if suit exists and is matching the first card
				valid = 1;
			else {
				if (is_player) //Invalid if suit exists but is not played
					printf("Card must match leading suit\n"); //Message for player
				valid = 0;
			}
		}
		else
			valid = 1; //If the player does not have a card of matching suit, any move is valid
	}
	else
		return -1; //Error return value

	return valid; //Returns whether or not a move was valid
}