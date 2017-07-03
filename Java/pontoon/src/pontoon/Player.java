package group.pontoon;

/**
 * Created by Charlie Arnold for used in G33
 * coursework for introduction to object
 * orientated systems 2016.
 */
public class Player {    
    private String name;
    private int balance;
    private int[] numberOfCardsHands = new int[2];
    private int[][] hands = new int[2][5];
    private int[] betOnHands = new int[2];
    private boolean handHasAce;
    private boolean haveSecondHand;

    //Constructor
    public Player(String name) {
        this.name = name;
        this.balance = 3000;
        clearHands();
    }

    //This takes a cards ID value and uses it to find its game value
    private int findCardValue(int cardRef){
        cardRef--;
        cardRef %= 13;
        cardRef = cardRef>=9? 10: (cardRef+1);
        return cardRef;
    }
    
    //Sets a players hand to the default values
    public void clearHands(){
        for(int i=0; i < 2; i++){
            this.numberOfCardsHands[i] = 0;
            for(int j=0; j<5; j++){
                this.hands[i][j] = 0;
            }
            this.betOnHands[i] = 0;
        }
        handHasAce = false;
        haveSecondHand = false;
    }
    
    //This adds a card to a players hand,
    //-it needs to know which hand (0/1)
    //-what bet they player is putting on it (can be 0)
    //-what the numeric card value is
    public void addCardToHand(int whichHand, int bet, int card){
        //Take money out of balance
        updateBalance(bet*-1);
        //Add it to current bet
        this.betOnHands[whichHand] += bet;
        //Add card to hand
        this.hands[whichHand][getNumberOfCardsHands(whichHand)] = card;
        //Update number of cards in hand
        this.numberOfCardsHands[whichHand]++;
    }
    //This will work out the total in a hand 
    public int calculateHandTotal(int whichHand){
        int total = 0, cardValue;
        //For every card in the hand
        for(int i : this.hands[whichHand]){
            cardValue = findCardValue(i);
            if(i > 0){
                //If its the first ace
                if((cardValue == 1) && (this.handHasAce == false)){
                    total += 11;
                    this.handHasAce = true;
                }
                else{
                    total += cardValue;
                }
            }   
        }
        //If the player has an ace and went bust try using ace as 1
        if((total > 21) && (this.handHasAce = true)){
            total = 0;
            for(int i : this.hands[whichHand]){
                cardValue = i % 13;
                if(i > 0){
                    total += cardValue;                 
                }
            }
        }
        this.handHasAce = false;
        return total;
    }
    
    //This will return whether or not a hand contains an ace
    public boolean handHasAce(int whichHand){
        for(int card : hands[whichHand]){
            if(findCardValue(card) == 1 && card > 0)
                return true;
        }
        return false;
    }
    
    //This splits a players starting hand if they have the option
    public void split(){
        this.haveSecondHand = true;
        this.numberOfCardsHands[0]--;
        addCardToHand(1, this.betOnHands[0], this.hands[0][1]);
        this.hands[0][1] = 0;
    }
    
    //This burns a players starting hand if they have the option
    public void burn(){
        updateBalance(this.betOnHands[0]);
        clearHands();
    }
    
    //This checks wether a user can split their initial hand
    public boolean canSplit(){
        int card1Rank = (this.hands[0][0] - 1) % 13;
        int card2Rank = (this.hands[0][1] - 1) % 13;
        if((this.numberOfCardsHands[0] == 2) && (card1Rank == card2Rank) && !this.haveSecondHand)
            return true;
        else
            return false;
    }
    
    //This checks if a user can burn their initial hand
    public boolean canBurn(){
        if((this.numberOfCardsHands[0] == 2) && !this.haveSecondHand){
            int card1Val = findCardValue(this.hands[0][0]);
            int card2Val = findCardValue(this.hands[0][1]);
            int total = card1Val + card2Val;
            if((card1Val == 1) | (card2Val == 1))
                total+=10;
            if(total == 14)
                return true;
            else
                return false;
        }
        else 
            return false;
    }
    
    //This returns true if the player has a second hand
    public boolean hasSecondHand() {
        return haveSecondHand;
    }
    
    //This returns the players bet on a given hand
    public int getBetOnHands(int whichHand) {
        return betOnHands[whichHand];
    }
   
    //This updates the players balance
    public void updateBalance(int transfer) {
        this.balance += transfer;
    }
    
    //This returns a 2D array of both player hands
    //0,0,0,0,0 if hand empty
    public int[][] getHands() {
        return this.hands;
    }
    
    //This returns a single hand
    public int[] getHand(int whichHand){
        return this.hands[whichHand];
    }
    
    //This gets the number of cards in a players hand
    public int getNumberOfCardsHands(int whichHand) {
        return numberOfCardsHands[whichHand];
    }

    //This gets the players balance
    public int getBalance() {
        return balance;
    }
    
    //This gets the players name...helpful for amnesiacs
    public String getName() {
        return name;
    }

    //This returns the players details in the form:
    //Player{name = ____, balance = ____}
    //Number of cards in hand1
    //Values stored in hand1
    //Bet on hand1
    //Hand1 total
    //If the player has a second hand it will then display
    //Number of cards in hand2
    //Values stored in hand2
    //Bet on hand2
    //Hand2 total
    @Override
    public String toString() {
        String message = "Player : " + name + ", Balance: " + balance + "\n"
                + "Hand 1 :\n";
        
        for(int card : hands[0])
            if(card != 0)
                message += Deck.getCardName(card) + "\n";
        message += "Hand Total = " + this.calculateHandTotal(0) + "\n";
        
        if(haveSecondHand){
            message += "Hand 2 :\n";
            for(int card : hands[1])
                if(card != 0)
                    message += Deck.getCardName(card) + "\n";
            message += "Hand Total = " + this.calculateHandTotal(1);
        }
        
        String string = "Player{" + "name=" + name + ", balance=" + balance + "} " 
                   + this.getNumberOfCardsHands(0) + " Hand1:";
        for(int i: hands[0]){
            string += i + ",";
        }
        string += " " + this.getBetOnHands(0);
        string += " " + this.calculateHandTotal(0);
        if(this.haveSecondHand){
            string += " " + this.getNumberOfCardsHands(1) + " Hand2:";
            for(int i: hands[1]){
                string += i + ",";
            }
            string += " " + this.getBetOnHands(1);
            string += " " + this.calculateHandTotal(1);
            
        }
        return message;
    }

}
