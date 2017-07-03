package group.pontoon;

import java.util.ArrayList;
import java.util.Scanner;

/**
 * Author:  Colin Berry
 * Created: 17-Feb-2016
 */
public class Main {
    public static final int MIN_BET = 50;
    public static ArrayList<Player> playerArray = new ArrayList<>();
    public static Player dealer = new Player("Dealer");
    public static Deck deck = new Deck();
    public static Scanner scan = new Scanner(System.in);
    public static boolean playing = true;
    
    public static void main(String[] args) {
        // Loop games onto one another
        while(true){
            addPlayers(playerArray.size());
            playing = true;
            // Loops while the same people are playing
            while(playing){
                setupPlayers();
                System.out.println("Dealers first card is the " + Deck.getCardName(dealer.getHand(0)[0]) + "\n");
                Play.playHands();
                System.out.println(dealer);
                for(Player player : playerArray){
                    System.out.println(player);
                }
                // -- SETTLE BETS -- 
                SettleBets.settleBets(playerArray, dealer);
                for(Player player : playerArray)
                    System.out.println(player.getName() + " has " + player.getBalance() + " credits remaining.");
                // Removes players that don't meet the minimum bet requirement
                for(Player player : playerArray){
                    if(player.getBalance() < MIN_BET){
                        System.out.println(player.getName() + " has been eliminated due to lack of funds.");
                        playerArray.remove(player);
                    }
                }
                // If players remain, ask if they wish to continue, else break back into setup loop
                if(playerArray.size() != 0)
                    playing = getBoolean("\nDo you wish to continue playing with the same players? : ");
                else
                    return;
                // If same players don't wish to keep playing, ask each individually before prompting for new players
                if(!playing){
                    ArrayList<Player> removedPlayers = new ArrayList<Player>();
                    for(Player player : playerArray){
                        boolean staying = getBoolean("Does " + player.getName() + " wish to continue playing?: ");
                        if(!staying)
                            removedPlayers.add(player);
                    }
                    playerArray.removeAll(removedPlayers);
                }
            }
        }
    }
    
    // Deals out cards to each player as a new round begins
    private static void setupPlayers(){
        dealer.clearHands();
        dealer.addCardToHand(0, 0, deck.drawCard());
        dealer.addCardToHand(0, 0, deck.drawCard());
        for(Player player : playerArray){
            player.clearHands();
            // Deals 2 cards and places a default bet
            player.addCardToHand(0, MIN_BET, deck.drawCard());
            player.addCardToHand(0, 0, deck.drawCard());
            String cardName1 = Deck.getCardName(player.getHand(0)[0]);
            String cardName2 = Deck.getCardName(player.getHand(0)[1]);
            // Asks the player if they wish to burn or split if possible
            if(player.canSplit()){
                boolean split = getBoolean(player.getName() + ", Do you wish to split with " + cardName1 + " and " + cardName2 + "? (y/n) : ");
                if(split){
                    player.split();
                    player.addCardToHand(0, 0, deck.drawCard());
                    player.addCardToHand(1, 0, deck.drawCard());
                }
            } else if (player.canBurn()){
                boolean burn = getBoolean(player.getName() + ", Do you wish to burn with " + cardName1 + " and " + cardName2 + "? (y/n) : ");
                if(burn){
                    player.burn();
                    player.addCardToHand(0, MIN_BET, deck.drawCard());
                    player.addCardToHand(0, 0, deck.drawCard());
                }
            }
        }
    }
    
    // Creates 2-5 fresh player objects
    private static void addPlayers(int currentPlayers){
        if(currentPlayers == 5)
            return;
        
        // Creates minimum and maximum amount of players able to be added
        int lowerBound = 2;
        if(currentPlayers >= 2)
            lowerBound = 0;
        int upperBound = 5 - currentPlayers;
        
        int numberOfPlayers = getInteger("Enter the number of new players (" + lowerBound + "-" + upperBound + ") : ",lowerBound,upperBound);
        scan.nextLine();
        for(int num = 0; num < numberOfPlayers; num++){
            System.out.print("Enter a player name : ");
            String name = scan.nextLine();
            playerArray.add(new Player(name));
        }
        System.out.println("");
    }
    
    // Returns a valid integer from the user given a prompt and bounds in which it must lie
    public static int getInteger(String prompt, int lowerBound, int upperBound){
        boolean validReply = false;
        int reply = Integer.MIN_VALUE;
        // Loops until a valid response is given
        while(!validReply){
            try{
                System.out.print(prompt);
                reply = Integer.parseInt(scan.next());
                if(reply >= lowerBound && reply <= upperBound)
                    validReply = true;
                else
                    System.out.println("Not a valid number! - Try Again.\n");
            // Exception thrown when input is not an integer
            }catch(Exception e){
                reply = Integer.MIN_VALUE;
                System.out.println("Not an Integer! - Try Again.\n");
            }
        }
        return reply;
    }
    
    // Returns a boolean value from the user given a prompt
    public static boolean getBoolean(String prompt){
        boolean validReply = false;
        boolean replyBool = false;
        String reply;
        // Loops until a valid response is given
        while(!validReply){
            System.out.print(prompt);
            reply = scan.next();
            if("y".equals(reply.toLowerCase()) || "yes".equals(reply.toLowerCase())){
                validReply = true;
                replyBool = true;
            } else if ("n".equals(reply.toLowerCase()) || "no".equals(reply.toLowerCase()))
                validReply = true;
            else
                System.out.println("Not a valid answer! - Try Again.\n");
        }
        return replyBool;
    }
}
