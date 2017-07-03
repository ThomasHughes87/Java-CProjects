package group.pontoon;

/**
 * Author:  Colin Berry
 * Created: 15-Feb-2016
 * Modified:25-Feb-2016
 * Desc:    Used to handle playing each round after initial cards have been dealt
 * Changes: Linked in with group Player class
 */
public class Play {
    static boolean playing;
    static int action;
    
    // Plays until both player and dealer have stuck
    public static void playHands(){
        for(Player player : Main.playerArray){
            System.out.println("Next Player: " + player.getName());
            // Plays the first hand
            playing = true;
            while(playing){
                action = getPlayerAction(player);
                playPlayer(player,0);
            }
            System.out.println("Final Hand Total - " + player.calculateHandTotal(0) + "\n");
            // Plays a second hand if available
            if(player.hasSecondHand()){
                System.out.println("Now Playing Second Hand, " + player.getName());
                playing = true;
                while(playing){
                    action = getPlayerAction(player);
                    playPlayer(player,1);
                    
                }
                System.out.println("Final Hand Total - " + player.calculateHandTotal(1) + "\n");
            }
        }
        playDealer(Main.dealer);
        // TODO: Call to SettleBets
    }
    
    // Plays a given players hand
    private static void playPlayer(Player player, int hand){
        switch(action){
            // Twist
            case 1:
                player.addCardToHand(hand, 0, Main.deck.drawCard());
                break;
            // Buy Card
            case 2:
                if(canBet(player,hand))
                    player.addCardToHand(hand, getPlayerBet(player,hand), Main.deck.drawCard());
                else
                    System.out.println("You do not have the funds to bet!\n");
                break;
            // Stick
            case 3:
                playing = false;
        }
        // Stop if hand is 21 OR Bust OR 5 Cards
        if(player.getNumberOfCardsHands(hand) == 5 || player.calculateHandTotal(hand) >= 21){
            if(player.calculateHandTotal(hand) > 21)
                System.out.println("-- BUST! --");
            else if(player.getNumberOfCardsHands(hand) == 5)
                System.out.println("-- 5 CARD HAND! --");
            playing = false;
        }
    }
    
    // Plays the dealers hand
    private static void playDealer(Player dealer){
        // Play until Bust OR 5 Cards OR Hard 17+
        int total;
        while(dealer.getNumberOfCardsHands(0) < 5){
            total = dealer.calculateHandTotal(0);
            if(total < 17)
                dealer.addCardToHand(0, 0, Main.deck.drawCard());
            else
                return;
        }
    }
    
    // Checks if a player has the balance to bet
    private static boolean canBet(Player player, int hand){
        return player.getBalance() > player.getBetOnHands(hand);
    }
    
    // Returns a valid action from the player
    private static int getPlayerAction(Player player){
        System.out.println(player.toString());
        return Main.getInteger("\nEnter an action... 1-Twist, 2-Buy, 3-Stick : ", 1, 3);
    }
    
    // Returns a valid bet from the player
    private static int getPlayerBet(Player player, int hand){
        // Valid bets must be between the current bet and 2 times the current bet
        int lowerBet = player.getBetOnHands(hand);
        int higherBet = player.getBetOnHands(hand) * 2;
        if(player.getBalance() < higherBet)
            higherBet = player.getBalance();
        // Loops until a valid repsonse is given
        return Main.getInteger("\nEnter a bet between " + lowerBet + " and " + higherBet + ": ", lowerBet, higherBet);
    }
}
