/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package group.pontoon;
import java.util.ArrayList;
/**
 *
 * @author Owner
 */
public class SettleBets {
    
    public static void settleBets(ArrayList<Player> players, Player dealer){
        //For every player in player array
        for(Player player : players){
            //If players first hand has one
            if(playerHasWon(player, dealer, 0)){
                dealer.updateBalance(-2 * player.getBetOnHands(0));
                player.updateBalance(2 * player.getBetOnHands(0));
                System.out.println(player.getName() + "'s first hand won!.");
            }
            else{
                dealer.updateBalance(2*player.getBetOnHands(0));
                System.out.println(player.getName() + "'s first hand lost.");
            }
            //If player has a second hand and it's one
            if(player.hasSecondHand()){
                if(playerHasWon(player, dealer, 1)){
                  dealer.updateBalance(-2 * player.getBetOnHands(1));
                  player.updateBalance(2 * player.getBetOnHands(1)); 
                  System.out.println(player.getName() + "'s second hand won!");
                }
                else{
                    dealer.updateBalance(2*player.getBetOnHands(1));
                    System.out.println(player.getName() + "'s second hand lost.");
                }
            }
            player.clearHands();
        }
        dealer.clearHands();
    }
    
    private static boolean playerHasWon(Player player, Player dealer, int whichHand){
        //If player has bust
        if(player.calculateHandTotal(whichHand) > 21)
            return false;
        //If dealer has bust in both hands
        if((dealer.calculateHandTotal(0) > 21) && (dealer.calculateHandTotal(1) > 21))
            return true;
        //if Dealer has pontoon in either hand
        if(((dealer.calculateHandTotal(0) == 21) && (dealer.getNumberOfCardsHands(0) == 2)) 
                || ((dealer.calculateHandTotal(1) == 21) && (dealer.getNumberOfCardsHands(1) == 2)))
            return false;
        //if plyaer has pontoon
        if((player.calculateHandTotal(whichHand) == 21) && (player.getNumberOfCardsHands(whichHand) == 2))
            return true;
        //If dealer has 5 card trick in either hand
        if(((dealer.calculateHandTotal(0) <= 21) && (dealer.getNumberOfCardsHands(0) == 5)) 
                || ((dealer.calculateHandTotal(1) <= 21) && (dealer.getNumberOfCardsHands(1) == 5)))
            return false;
        //if player has 55 card trick
        if((player.calculateHandTotal(whichHand) <= 21) && (player.getNumberOfCardsHands(whichHand) == 5))
            return true;
        //If players hand is larger than both of dealers hands
        return hasHigherHand(player, dealer, whichHand);
    }
    private static boolean hasHigherHand(Player player, Player dealer, int whichHand){
        return ((player.calculateHandTotal(whichHand) > dealer.calculateHandTotal(0)) || (dealer.calculateHandTotal(0) > 21))
                && ((player.calculateHandTotal(whichHand) > dealer.calculateHandTotal(1)) || (dealer.calculateHandTotal(1) > 21));
    }
}


