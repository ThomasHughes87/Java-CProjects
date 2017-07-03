// BattleshipBot.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>
#include <math.h>
#pragma comment(lib, "wsock32.lib")


#define STUDENT_NUMBER		"420_MLG_PRO"
#define STUDENT_FIRSTNAME	"Jakub"
#define STUDENT_FAMILYNAME	"Mól"

#define IP_ADDRESS_SERVER	"127.0.0.1"
//#define IP_ADDRESS_SERVER "164.11.80.27"
//#define IP_ADDRESS_SERVER "164.11.157.13"

#define PORT_SEND	 1924 // We define a port that we are going to use.
#define PORT_RECEIVE 1925 // We define a port that we are going to use.


#define MAX_BUFFER_SIZE	500
#define MAX_SHIPS		200

#define FIRING_RANGE	100

#define MOVE_LEFT		-1
#define MOVE_RIGHT		 1
#define MOVE_UP			 1
#define MOVE_DOWN		-1
#define MOVE_FAST		 2
#define MOVE_SLOW		 1


SOCKADDR_IN sendto_addr;
SOCKADDR_IN receive_addr;

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

bool moveShip = false;
int moveX;
int moveY;

bool setFlag = true;
int new_flag = 1;


void fire_at_ship(int X, int Y);
void move_in_direction(int left_right, int up_down);
void set_new_flag(int newFlag);







/*************************************************************/
/********* Your tactics code starts here *********************/
/*************************************************************/

int up_down = MOVE_LEFT*MOVE_SLOW;
int left_right = MOVE_UP*MOVE_FAST;

int shipDistance[MAX_SHIPS];

struct Ship
{
	int X;
	int Y;
	int Health;
	int Flag;
	int Distance;
	int FlagKey;
};

Ship ship[MAX_SHIPS];

Ship OldShip[MAX_SHIPS];

Ship enemy[MAX_SHIPS];

Ship ally[MAX_SHIPS];

Ship tempShip;

int number_of_friends;
int number_of_enemies;
int oldNumberOfShips;


/*
Name: track
Input: int x , int y 
Output: void 
Description: Moves the ship coordinets towards the x and y inputed
*/
void track(int x, int y)
{
	int offset = 0;

	if ((x == 500) && (y == 500))
	{
		offset += 100 ;
	}
	// y upper 
	if ( myY > (y + offset)) 
	{
		up_down = MOVE_DOWN*MOVE_FAST;
	}
	// x left
	if (myX < (x - offset))
	{
		left_right = MOVE_RIGHT*MOVE_FAST;
	}
	//y lower
	if ( myY < (y - offset))
	{
		up_down = MOVE_UP*MOVE_FAST;
	}
	// x right 
	if (myX > (x + offset))
	{
		left_right = MOVE_LEFT*MOVE_FAST;
	}
}



bool IsaFriend(int index)
{
	bool rc,pass;

	rc = false;

	int shipArea;

	shipArea = ship[index].Flag - ship[index].X - ship[index].Y; 

	if((shipArea >= 896) && (shipArea <= 904))
	{
		rc = true;
	}

	return rc;
}


void tactics()
{
	int i,j;
	bool enemyInRange, friendInRange ;
	// Calculate ship distances

	for (i=0;i<number_of_ships;i++)
	{
		ship[i].FlagKey = ship[i].X *ship[i].Y;
		ship[i].Distance = (int)sqrt((pow((double)(ship[i].X - ship[0].X),2))+(pow((double)(ship[i].Y - ship[0].Y),2)));
	}

	// Sort by ship distance
	for (i=0;i<number_of_ships;i++)
	{
		for(j=1;j<number_of_ships;j++)
		{
			if (ship[j].Distance > ship[j+1].Distance)
			{
				tempShip = ship[j];
				ship[j] = ship[j+1];
				ship[j+1] = tempShip;
			}
		}
	}

	// Seperate friend from enemy

	number_of_friends = 0;
	number_of_enemies = 0;
	enemyInRange = false;
	friendInRange = false;

	if (number_of_ships > 1)
	{
		for (i=1; i<number_of_ships; i++)
		{
			if (IsaFriend(i))
			{
				friendInRange = true;
				ally[number_of_friends] = ship[i];	
				number_of_friends++;
			}
			else
			{
				enemyInRange = true;
				enemy[number_of_enemies] = ship[i];	
				printf("%d %d %d %d\n ",number_of_enemies,enemy[number_of_enemies].Distance,enemy[number_of_enemies].Health,enemy[number_of_enemies].Flag);
				number_of_enemies++;

			}
		}
	}
	oldNumberOfShips = number_of_ships;
	for (i=1; i<number_of_ships; i++)
	{
		OldShip[i] = ship[i];
	}

	// priorotise low health enemy
	if(enemy[1].Health < enemy[0].Health)
	{
		tempShip = enemy[0];
		enemy[0] = enemy[1];
		enemy[1] = tempShip;
	}


	// Direction choice
	if (enemyInRange && friendInRange)
	{
		// friend and enemy in range
		if(ally[0].Distance > 50)
		{
			track(ally[0].X,ally[0].Y);
		}
		else
		{
			track(enemy[0].X,enemy[0].Y);
		}
	}
	else
	{
		if(enemyInRange && !friendInRange)
		{
			// enemy in range but no friend
			track(enemy[0].X,enemy[0].Y);
		}
		else
		{
			if (!enemyInRange && friendInRange)
			{
				// friend in range but no enemy
				if(ally[0].Distance > 50)
				{
					track(ally[0].X,ally[0].Y);
				}
				else
				{
					track(500,500);
				}
			}
			else
			{
				// firend and enemy not in range
				track(500,500);
			}
		}
	}

	set_new_flag(900 + ship[0].X + ship[0].Y);
	if(ally[0].Health < 3 && friendInRange)
	{
		fire_at_ship(ally[0].X, ally[0].Y );
	}
	else
	{
		fire_at_ship(enemy[0].X, enemy[0].Y);
	}	
	move_in_direction(left_right, up_down);
	printf("ME:  %d %d %d %d %d %d\n ",ship[0].Health,ship[0].Flag,number_of_enemies,number_of_friends,ship[0].FlagKey,ship[0].X*ship[0].Y);

}

/*************************************************************/
/********* Your tactics code ends here ***********************/
/*************************************************************/



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

	sprintf_s(buffer, "Register  %s,%s,%s", STUDENT_NUMBER, STUDENT_FIRSTNAME, STUDENT_FAMILYNAME);
	sendto(sock_send, buffer, strlen(buffer), 0, (SOCKADDR *)&sendto_addr, sizeof(SOCKADDR));

	while (true)
	{
		if (recvfrom(sock_recv, buffer, sizeof(buffer)-1, 0, (SOCKADDR *)&receive_addr, &len) != SOCKET_ERROR)
		{
			p = ::inet_ntoa(receive_addr.sin_addr);

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
						sscanf_s(InputBuffer,"%d,%d,%d,%d", &ship[number_of_ships].X, &ship[number_of_ships].Y, &ship[number_of_ships].Health, &ship[number_of_ships].Flag);
						number_of_ships++;
						break;

					case '\0':
						InputBuffer[j] = '\0';
						sscanf_s(InputBuffer,"%d,%d,%d,%d", &ship[number_of_ships].X, &ship[number_of_ships].Y, &ship[number_of_ships].Health, &ship[number_of_ships].Flag);
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

				myX = ship[0].X;
				myY = ship[0].Y;
				myHealth = ship[0].Health;
				myFlag = ship[0].Flag;


				tactics();

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


void fire_at_ship(int X, int Y)
{
	fire = true;
	fireX = X;
	fireY = Y;
}



void move_in_direction(int X, int Y)
{

	if (X < -2) X = -2;
	if (X >  2) X =  2;
	if (Y < -2) Y = -2;
	if (Y >  2) Y =  2;

	moveShip = true;
	moveX = X;
	moveY = Y;
}


void set_new_flag(int newFlag)
{
	setFlag = true;
	new_flag = newFlag;
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

	memset(&receive_addr, 0, sizeof(SOCKADDR_IN));
	receive_addr.sin_family = AF_INET;
	//	receive_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS_SERVER);
	receive_addr.sin_addr.s_addr = INADDR_ANY;
	receive_addr.sin_port = htons(PORT_RECEIVE);

	int ret = bind(sock_recv, (SOCKADDR *)&receive_addr, sizeof(SOCKADDR));
	//	int ret = bind(sock_send, (SOCKADDR *)&receive_addr, sizeof(SOCKADDR));
	if (ret)
	{
		printf("Bind failed! %d\n", WSAGetLastError());  
	}

	communicate_with_server();

	closesocket(sock_send);
	closesocket(sock_recv);
	WSACleanup();

	while (chr != '\n')
	{
		chr = getchar();
	}

	return 0;
}

