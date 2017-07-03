
package RangeAverafe;
// import Scanner
import java.util.Scanner;

public class AverageRange {

   
    public static void main(String[] args) {
        // Store variables
    int number = 0, total = 0, counter = 0, Average;    
    // Scanner that imports files A and B
    Scanner input = new Scanner(AverageRange.class.getResourceAsStream("NumbersA.txt"));
    
    while (input.hasNextInt()) { // read in all the numbers untill there are no more
        number = input.nextInt();
        
        if ((number < 10 ) || (number > 30)) { // gives you the range
            total += number;
            counter ++; // ignore the counter if the number is in the 10-30 range
        }
    }
    Average = total / counter; // works out the average of the numbers within the range
    
    System.out.println("Average of numbers in range is: " + Average);

    }
    
    
}