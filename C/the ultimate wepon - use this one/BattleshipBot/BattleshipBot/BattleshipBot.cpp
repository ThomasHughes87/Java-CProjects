

#include "stdafx.h"
#include <winsock2.h>
#include <math.h>
#pragma comment(lib, "wsock32.lib")


#define STUDENT_NUMBER		"I_DONT_LIKE_EXAMS"
#define STUDENT_FIRSTNAME	"Thomas"
#define STUDENT_FAMILYNAME	"Hughes"
//#define IP_ADDRESS_SERVER	"127.0.0.1"
#define IP_ADDRESS_SERVER	"164.11.157.162"
// Ally IP and server addresses
#define IP_ADDRESS_FRIEND0  "164.11.80.42" // friend ip
#define IP_ADDRESS_FRIEND1  "164.11.80.12" // friend ip
#define PORT_SEND	 1924 // We define a port that we are going to use.
#define PORT_RECEIVE 1925 // We define a port that we are going to use.

#define MAX_BUFFER_SIZE	4096
#define MAX_SHIPS		30

#define FIRING_RANGE	500

#define MOVE_LEFT		-1
#define MOVE_RIGHT		 1
#define MOVE_UP			 1
#define MOVE_DOWN		-1
#define MOVE_FAST		 2
#define MOVE_SLOW		 1

SOCKADDR_IN sendto_addr;
SOCKADDR_IN receive_addr;
SOCKADDR_IN sendto_addr_friend0;
SOCKADDR_IN sendto_addr_friend1;

SOCKET sock_send;  // This is our socket, it is the handle to the IO address to read/write packets
SOCKET sock_recv;  // This is our socket, it is the handle to the IO address to read/write packets

WSADATA data;

char InputBuffer [MAX_BUFFER_SIZE];

int myX;
int myY;
int myHealth;
int myFlag;
int number_of_ships;

bool fire = false;
int fireX;
int fireY;

bool enemyInRange, friendInRange ;
bool moveShip = false;

int moveX;
int moveY;

int sendX;
int sendY;

bool setFlag = true;
int new_flag = rand() % 1000000;

void fire_at_ship(int X, int Y);
void move_in_direction(int left_right, int up_down);
void set_new_flag(int newFlag);

/*************************************************************/
/*************** Your tactics code starts here ***************/
/*************************************************************/


int up_down = MOVE_LEFT*MOVE_FAST;
int left_right = MOVE_UP*MOVE_FAST;
int shipDistance[MAX_SHIPS];


/********************** SHIP STRUCTURES **********************/


struct Ship
{
	int x;
	int y;
	int health;
	int flag;
	int distance;
};

Ship ship[MAX_SHIPS];		// All ships.
Ship enemy[MAX_SHIPS];		// Enemy ships.
Ship ally[MAX_SHIPS];		// Ally ships.
Ship tempShip;				// Temp ship for sorting ships.

struct Pack
{
	Ship ship[4];
	int packSize;
	bool filled;
	//int Distance;
};

Pack enemyPack[MAX_SHIPS];	// Array of enemy packs.
Pack myPack;				// Ally pack.



/******************** Tactics Functions *********************/

int number_of_friends;
int number_of_enemies;

/*
Name: receiveShipData.
Input: int recvX ,int recvY ,int recvHealth, int recvFlag.
Output: void.
Description: Handles the received data from the server. This prevents the array overflowing.
Notice: To increase maximum data saved into the arrray you need to increase MAX_SHIPS.
*/

void receiveShipData(int recvX, int recvY, int recvHealth, int recvFlag)
{
	if (number_of_ships < MAX_SHIPS)
	{
		ship[number_of_ships].x = recvX;
		ship[number_of_ships].y = recvY;
		ship[number_of_ships].health = recvHealth;
		ship[number_of_ships].flag = recvFlag;
	}
}

/*
Name: initialiseAllPacks.
Input: void.
Output: void.
Description: Initialises all the packs.
*/

void initialiseAllPacks(void)
{
	Pack emptyPack;
	emptyPack.filled = false;
	emptyPack.packSize = 0;

	for (int j = 0; j < 4; j++)
	{
		emptyPack.ship[j].health = 0;
		emptyPack.ship[j].distance = 0;
		emptyPack.ship[j].flag = 0;
		emptyPack.ship[j].x = 0;
		emptyPack.ship[j].y = 0;
	}

	for (int i = 0; i < MAX_SHIPS; i++)
	{
		enemyPack[i] = emptyPack;
	}

	myPack = emptyPack;
}
///*
//Name: quadrantTactics
//Input: void
//Output: void
//Description: Uses the quadrant tactics to specify where to move next
//*/
void quadrantTactics(void)
{
	int upper_right = 0;
	int	lower_left = 0;
	int	upper_left = 0;
	int	lower_right = 0;

	 //scans through 4 quadrants and determines how many enemys is there in each
	for (int i = 0; i<number_of_ships; i++)
	{
		if ((myX <= enemy[i].x) && (myY <= enemy[i].y))
			upper_right++;
		else if ((myX >= enemy[i].x) && (myY >= enemy[i].y))
			lower_left++;
		else if ((myX >= enemy[i].x) && (myY <= enemy[i].y))//////////////////////////
			upper_left++;
		else if ((myX >= enemy[i].x) && (myY >= enemy[i].y))
			lower_right++;
	}

	if ((upper_right >= 2) && (upper_right <4))
	{
		up_down = MOVE_UP*MOVE_FAST;
		left_right = MOVE_RIGHT*MOVE_FAST;
	}
	if ((lower_left >= 2) && (lower_left <4))
	{
		up_down = MOVE_DOWN*MOVE_FAST;
		left_right = MOVE_LEFT*MOVE_FAST;
	}
	if ((upper_left >= 2) && (upper_left <4))
	{
		up_down = MOVE_UP*MOVE_FAST;
		left_right = MOVE_LEFT*MOVE_FAST;
	}
	if ((lower_right >= 2) && (lower_right <4))
	{
		up_down = MOVE_DOWN*MOVE_FAST;
		left_right = MOVE_RIGHT*MOVE_FAST;
	}
}
/*
Name: move
Input: int x, int y
Output: void
Description: Moves the ship towards the coordinates entered.
*/
void move(int x, int y)
{
	// move up
	if (myY < y)
	{
		up_down = MOVE_UP*MOVE_FAST;

		if (ship[0].health > 7)
		{
			sendY = ship[0].y + 2;
		}
		else
		{
			sendY = ship[0].y + 1;
		}
	}

	// move down
	if (myY > y)
	{
		up_down = MOVE_DOWN*MOVE_FAST;

		if (ship[0].health > 7)
		{
			sendY = ship[0].y - 2;
		}
		else
		{
			sendY = ship[0].y - 1;
		}
	}

	//move left
	if (myX > x)
	{
		left_right = MOVE_LEFT*MOVE_FAST;

		if (ship[0].health > 7)
		{
			sendX = ship[0].x - 2;
		}
		else
		{
			sendX = ship[0].x - 1;
		}
	}

	// move right
	if (myX < x)
	{
		left_right = MOVE_RIGHT*MOVE_FAST;

		if (ship[0].health > 7)
		{
			sendX = ship[0].x + 2;
		}
		else
		{
			sendX = ship[0].x + 1;
		}
	}
}

/*
Name: retreat
Input: int x, int y
Output: void
Description: Moves the ship away from the coordinates entered.
*/

void retreat(int x, int y)
{
	if (myY > y)	// Move up
	{
		up_down = MOVE_UP*MOVE_FAST;

		if (ship[0].health > 7)
		{
			sendY = ship[0].y + 2;
		}
		else
		{
			sendY = ship[0].y + 1;
		}
	}

	if (myY < y)	// Move down
	{
		up_down = MOVE_DOWN*MOVE_FAST;

		if (ship[0].health > 7)
		{
			sendY = ship[0].y - 2;
		}
		else
		{
			sendY = ship[0].y - 1;
		}
	}

	if (myX < x)	// Move left
	{
		left_right = MOVE_LEFT*MOVE_FAST;

		if (ship[0].health > 7)
		{
			sendX = ship[0].x - 2;
		}
		else
		{
			sendX = ship[0].x - 1;
		}
	}

	if (myX > x)	// Move right
	{
		left_right = MOVE_RIGHT*MOVE_FAST;

		if (ship[0].health > 7)
		{
			sendX = ship[0].x + 2;
		}
		else
		{
			sendX = ship[0].x + 1;
		}
	}
}

/*
Name: groupUp
Input: int xZero, int yZero, int xOne, int yOne,  int xTwo, int yTwo
Output: void
Description: Enter the coordinates of each ally (0, 1 & 2) to determine a central meeting point.
Notice: This function will only work for an ally pack of three ships.
*/

void groupUp(int xZero, int yZero, int xOne, int yOne, int xTwo, int yTwo)
{
	int midPointX, midPointY;

	midPointX = (xZero + xOne) / 2;			// Calculates the middle location between two ships
	midPointY = (yZero + yOne) / 2;			// within the ally pack

	midPointX = (xTwo + midPointX) / 2;		// Calculates the middle location between the previous midpoint
	midPointY = (yTwo + midPointY) / 2;		// and the third ship in the ally pack

	move(midPointX, midPointY);
}

/*
Name: sortEnemiesIntoPacks
Input: Ship enemyShip
Output: void
Description: Detects if ship is close enought to another ship in order to be a pack
*/

void sortEnemiesIntoPacks(Ship enemyShip)
{
	int numberOfEnemyPack;
	int distance;
	bool shipNotSorted;
	shipNotSorted = true;
	// runs through all the enemies packs
	for (int i = 0; i < MAX_SHIPS; i++)
	{
		if ((enemyPack[i].filled) && (shipNotSorted))
		{
			//calculate the distance between enemy and check if its less than the pre defined diagonal distance
			for (int j = 0; j < 4; j++)
			{
				distance = 0;
				distance = (int)sqrt((pow((double)(enemyShip.x - enemyPack[i].ship[j].x), 2)) + (pow((double)(enemyShip.y - enemyPack[i].ship[j].y), 2)));

				// adds ships to the pack array which match the diagonal distance criteria
				if ((distance <= 10) && (distance != 0) && (shipNotSorted))
				{
					enemyPack[i].ship[enemyPack[i].packSize] = enemyShip;
					enemyPack[i].packSize++;
					shipNotSorted = false;

				}
			}
		}
	}
	if (shipNotSorted)
	{
		// Adds an enemy to the next empty pack if not close enough to another to be a pack
		numberOfEnemyPack = 0;
		while (enemyPack[numberOfEnemyPack].filled)
		{
			numberOfEnemyPack++;
		}
		enemyPack[numberOfEnemyPack].ship[enemyPack[numberOfEnemyPack].packSize] = enemyShip;
		enemyPack[numberOfEnemyPack].packSize++;
		shipNotSorted = false;
		enemyPack[numberOfEnemyPack].filled = true;
	}
}

/*
Name: calculatePackHealth
Input: Ship pack[MAX_SHIPS]
Output: int pack health
Description: Calculates the health of a given pack
*/

int calculatePackHealth(Pack pack)
{
	int packHealth;

	packHealth = 0;

	for (int i = 0; i < 4; i++)
	{
		packHealth += pack.ship[i].health;
	}

	return packHealth;
}

/*
Name: selectTactics.
Input: void.
Output: int.
Description: Performs the logical operations that decide what tactics to implement.
*/

int selectTactics(void)
{

	// When two packs are in range
	if ((enemyInRange) && (enemyPack[0].ship[0].distance <= MAX_SHIPS) && (enemyPack[1].ship[0].distance <= MAX_SHIPS) && (ship[0].health > 7))
	{
		return 4;
	}
	else if ((enemyInRange) && (enemyPack[0].ship[0].distance <= MAX_SHIPS) && (enemyPack[1].ship[0].distance <= MAX_SHIPS) && (ship[0].health < 8))
	{
		return 2;
	}


	// When one pack is in range
	// Attack if enemy is weak
	if ((enemyInRange) && ((calculatePackHealth(enemyPack[0]) - 2) <= (calculatePackHealth(myPack))))
	{
		return 2;
	}

	// Retreat when enemy is strong
	if ((enemyInRange) && ((calculatePackHealth(enemyPack[0]) - 2) > (calculatePackHealth(myPack)) && (ship[0].health > 7)))
	{
		return 3;
	}
	else if ((enemyInRange) && ((calculatePackHealth(enemyPack[0]) - 2) > (calculatePackHealth(myPack)) && (ship[0].health < 8)))
	{
		return 2;
	}

	// Roam the map
	else
	{
		return 0;
	}
}

/*
Name: IsaFriend
Input: int Index of the ship in the Ship array
Output: bool
Description: Checks if the ship is allied
*/

bool IsaFriend(int index)
{
	bool friendly;

	friendly = false;

	for (int i = 0; i < 3; i++)
	{
		if ((ship[index].x == ally[i].x) && (ship[index].y == ally[i].y))
		{
			friendly = true;
		}
	}
	return friendly;
}

void tactics()
{
	int i, j;

	initialiseAllPacks();

	// Calculate ship distances
	for (i = 0; i < number_of_ships; i++)
	{
		ship[i].distance = (int)sqrt((pow((double)(ship[i].x - ship[0].x), 2)) + (pow((double)(ship[i].y - ship[0].y), 2)));
	}

	// Sort by ship distance
	for (i = 0; i < number_of_ships; i++)
	{
		for (j = 1; j < number_of_ships; j++)
		{
			if (ship[j].distance > ship[j + 1].distance)
			{
				tempShip = ship[j];
				ship[j] = ship[j + 1];
				ship[j + 1] = tempShip;
			}
		}
	}

	myPack.ship[myPack.packSize] = ship[0];
	myPack.packSize++;


	// Seperate friend from enemy
	number_of_friends = 0;
	number_of_enemies = 0;
	enemyInRange = false;
	friendInRange = false;
	if (number_of_ships > 1)
	{
		for (i = 1; i<number_of_ships; i++)
		{
			if (IsaFriend(i))
			{
				friendInRange = true;
				if (ship[i].distance < 50)
				{
					myPack.ship[myPack.packSize] = ship[i];
					myPack.packSize++;
				}
			}
			else
			{
				enemy[number_of_enemies] = ship[i];
				enemyInRange = true;
				number_of_enemies++;
				sortEnemiesIntoPacks(ship[i]);
				//printf("%d %d %d %d\n ",number_of_enemies,ship[i].Distance,ship[i].Health,ship[i].Flag);
			}
		}
	}

	move(enemyPack[0].ship[0].x, enemyPack[0].ship[0].y);

	///* Tactics */

	switch (myPack.packSize) 
	{
		// When pack is one.
			case 1:
				switch (selectTactics())
				{
					// Group up with allies.
						case 1:
							groupUp(ally[0].x, ally[0].y, ally[1].x, ally[1].y, ally[2].x, ally[2].y);
							break;
		
					// Move towards enemy.
						case 2:
							move(enemyPack[0].ship[0].x, enemyPack[0].ship[0].y);
							break;
		
					// Move away from enemy.
						case 3:
							retreat(enemyPack[0].ship[0].x, enemyPack[0].ship[0].y);
							break;

					// Move away from two enemies drawing them into battle with themselves.
						case 4:
							move((enemyPack[0].ship[0].x), (enemyPack[1].ship[0].y - 10));
							break;
		
					// Move to centre of the map
						default:
							move(500,500);
							break;
				}
				break;
	
	// When pack is two.
		case 2:
			break;

	// When pack is three.
		case 3:
			break;

		default:
			break;
	}

	// Fire
	fire_at_ship(enemy[0].x, enemy[0].y);

	// Move
	move_in_direction(left_right, up_down);

	// Print information to console
	printf("ME:     X: %3d Y: %3d HEALTH: %2d ENEMIES: %2d FRIENDS: %1d\n", ship[0].x, ship[0].y, ship[0].health, number_of_enemies, number_of_friends);
	printf("ALLY 1: X: %3d Y: %3d HEALTH: %2d\n", ally[0].x, ally[0].y, ally[0].health);
	printf("ALLY 2: X: %3d Y: %3d HEALTH: %2d\n", ally[2].x, ally[2].y, ally[2].health);
	printf("PACKHEALTH: %2d ENEMYPACKHEALTH: %2d \n", calculatePackHealth(myPack), calculatePackHealth(enemyPack[0]));
	printf("Tick \n\n");
}

/*************************************************************/
/********* Your tactics code ends here ***********************/
/*************************************************************/

void fire_at_ship(int X, int Y)
{
	fire = true;
	fireX = X;
	fireY = Y;
}
void move_in_direction(int X, int Y)
{
	if (X < -2) X = -2;
	if (X >  2) X = 2;
	if (Y < -2) Y = -2;
	if (Y >  2) Y = 2;
	moveShip = true;
	moveX = X;
	moveY = Y;
}
void set_new_flag(int newFlag)
{
	setFlag = true;
	new_flag = newFlag;
}
void communicate_with_server()
{
	char buffer[4096];
	int  len = sizeof(SOCKADDR);
	char chr;
	bool finished;
	int  i;
	int  j;
	int  rc;
	char* p;
	int recvX;
	int recvY;
	int recvHealth;
	int recvFlag;

	// comment out 
	sprintf_s(buffer, "Register  %s,%s,%s", STUDENT_NUMBER, STUDENT_FIRSTNAME, STUDENT_FAMILYNAME);
	sendto(sock_send, buffer, strlen(buffer), 0, (SOCKADDR *)&sendto_addr, sizeof(SOCKADDR));
	// comment out

	while (true)
	{
		if (recvfrom(sock_recv, buffer, sizeof(buffer) - 1, 0, (SOCKADDR *)&receive_addr, &len) != SOCKET_ERROR)
		{
			p = ::inet_ntoa(receive_addr.sin_addr);
			if ((strcmp(IP_ADDRESS_FRIEND0, p) == 0))
			{
				sscanf_s(buffer, "%d,%d,%d", &ally[0].x, &ally[0].y, &ally[0].health);
				number_of_friends++;
			}
			if ((strcmp(IP_ADDRESS_FRIEND1, p) == 0))
			{
				sscanf_s(buffer, "%d,%d,%d", &ally[1].x, &ally[1].y, &ally[1].health);
				number_of_friends++;
			}
			if ((strcmp(IP_ADDRESS_SERVER, "127.0.0.1") == 0) || (strcmp(IP_ADDRESS_SERVER, p) == 0))
			{
				i = 0;
				j = 0;
				finished = false;
				number_of_ships = 0;
				while ((!finished) && (i<4096))
				{
					chr = buffer[i];
					switch (chr)
					{
					case '|':
						InputBuffer[j] = '\0';
						j = 0;
						sscanf_s(InputBuffer, "%d,%d,%d,%d", &recvX, &recvY, &recvHealth, &recvFlag);
						receiveShipData(recvX, recvY, recvHealth, recvFlag);
						number_of_ships++;
						break;
					case '\0':
						InputBuffer[j] = '\0';
						sscanf_s(InputBuffer, "%d,%d,%d,%d", &recvX, &recvY, &recvHealth, &recvFlag);
						receiveShipData(recvX, recvY, recvHealth, recvFlag);
						number_of_ships++;
						finished = true;
						break;
					default:
						InputBuffer[j] = chr;
						j++;
						break;
					}
					i++;
				}


				tactics();

				myX = ship[0].x;
				myY = ship[0].y;
				myHealth = ship[0].health;
				myFlag = ship[0].flag;

				// sending my location and health
				sprintf_s(buffer, "%d,%d,%d", sendX, sendY, myHealth);
				sendto(sock_send, buffer, strlen(buffer), 0, (SOCKADDR *)&sendto_addr_friend0, sizeof(SOCKADDR));
				sendto(sock_send, buffer, strlen(buffer), 0, (SOCKADDR *)&sendto_addr_friend1, sizeof(SOCKADDR));
				if (fire)
				{
					sprintf_s(buffer, "Fire %s,%d,%d", STUDENT_NUMBER, fireX, fireY);
					sendto(sock_send, buffer, strlen(buffer), 0, (SOCKADDR *)&sendto_addr, sizeof(SOCKADDR));
					fire = false;
				}
				if (moveShip)
				{
					sprintf_s(buffer, "Move %s,%d,%d", STUDENT_NUMBER, moveX, moveY);
					rc = sendto(sock_send, buffer, strlen(buffer), 0, (SOCKADDR *)&sendto_addr, sizeof(SOCKADDR));
					moveShip = false;
				}
				if (setFlag)
				{
					tactics();
					sprintf_s(buffer, "Flag %s,%d", STUDENT_NUMBER, new_flag);
					sendto(sock_send, buffer, strlen(buffer), 0, (SOCKADDR *)&sendto_addr, sizeof(SOCKADDR));
				}
			}
		}
		else
		{
			printf_s("recvfrom error = %d\n", WSAGetLastError());
		}
	}
	printf_s("Student %s\n", STUDENT_NUMBER);
}
int _tmain(int argc, _TCHAR* argv[])
{
	char chr = '\0';
	printf("\n");
	printf("Battleship Bots\n");
	printf("UWE Computer and Network Systems Assignment 2 (2013-14)\n");
	printf("\n");
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0) return(0);
	//sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  // Here we create our socket, which will be a UDP socket (SOCK_DGRAM).
	//if (!sock)
	//{	
	//	printf("Socket creation failed!\n"); 
	//}
	sock_send = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  // Here we create our socket, which will be a UDP socket (SOCK_DGRAM).
	if (!sock_send)
	{
		printf("Socket creation failed!\n");
	}
	sock_recv = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  // Here we create our socket, which will be a UDP socket (SOCK_DGRAM).
	if (!sock_recv)
	{
		printf("Socket creation failed!\n");
	}
	memset(&sendto_addr, 0, sizeof(SOCKADDR_IN));
	sendto_addr.sin_family = AF_INET;
	sendto_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS_SERVER);
	sendto_addr.sin_port = htons(PORT_SEND);

	memset(&sendto_addr_friend0, 0, sizeof(SOCKADDR_IN));
	sendto_addr_friend0.sin_family = AF_INET;
	sendto_addr_friend0.sin_addr.s_addr = inet_addr(IP_ADDRESS_FRIEND0);
	sendto_addr_friend0.sin_port = htons(PORT_RECEIVE)
		;
	memset(&sendto_addr_friend1, 0, sizeof(SOCKADDR_IN));
	sendto_addr_friend1.sin_family = AF_INET;
	sendto_addr_friend1.sin_addr.s_addr = inet_addr(IP_ADDRESS_FRIEND1);
	sendto_addr_friend1.sin_port = htons(PORT_RECEIVE);

	memset(&receive_addr, 0, sizeof(SOCKADDR_IN));
	receive_addr.sin_family = AF_INET;
	//receive_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS_SERVER);
	receive_addr.sin_addr.s_addr = INADDR_ANY;
	receive_addr.sin_port = htons(PORT_RECEIVE);

	int ret = bind(sock_recv, (SOCKADDR *)&receive_addr, sizeof(SOCKADDR));
	if (ret)
	{
		printf("Bind failed! %d\n", WSAGetLastError());
	}
	communicate_with_server();
	closesocket(sock_send);
	closesocket(sock_recv);
	WSACleanup();
	// comunicate with friend
	while (chr != '\n')
	{
		chr = getchar();
	}
	return 0;
}