package symdiff;

/**
 * Created by user on 3/28/2017.
 */
public class MainTest {
    public static void main(String[] args){
        Symdiff x = new Symdiff(2.0);
        Symdiff f = x.add(x).mult(x).inv().neg();

        f.full_eval();

        f.pretty_print();
    }
}
