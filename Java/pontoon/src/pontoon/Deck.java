package group.pontoon;

import java.util.ArrayList;
import java.util.Collections;

/**
 * Author:  Colin Berry
 * Created: 16-Feb-2016
 */
public class Deck {
    
    private static final String[] suits = {"Spades","Hearts","Diamonds","Clubs"};
    private static final String[] ranks = {"Ace","Two","Three","Four","Five","Six","Seven","Eight","Nine","Ten","Jack","Queen","King"};
    
    private final ArrayList<Integer> deckOfCards = new ArrayList<>();
    private final ArrayList<Integer> drawnCards = new ArrayList<>();
    
    // Used externally to check when to reset the deck
    public int cardsLeft;
    
    // Create a full deck consisting of 3 decks of cards
    public Deck(){
        for(int card=1; card <= 52; card++){
            // Add 3 cards to simulate 3 decks
            deckOfCards.add(card);
            deckOfCards.add(card);
            deckOfCards.add(card);
        }
        Collections.shuffle(deckOfCards);
        cardsLeft = deckOfCards.size();
    }
    
    // Draws a card from the deck
    public int drawCard(){
        int card = deckOfCards.get(deckOfCards.size() - 1);
        deckOfCards.remove(deckOfCards.size() - 1);
        drawnCards.add(card);
        cardsLeft--;
        return card;
    }
    
    // Reshuffles every card back into the main deck
    public void resetDeck(){
        deckOfCards.addAll(drawnCards);
        drawnCards.clear();
        Collections.shuffle(deckOfCards);
        cardsLeft = deckOfCards.size();
    }
    
    // Returns the name of the given card
    public static String getCardName(int card){
        card--;
        int rank = card % 13;
        int suit = card / 13;
        return ranks[rank] + " of " + suits[suit];
    }
}
