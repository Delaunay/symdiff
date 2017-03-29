package symdiff;


import java.util.Hashtable;
import java.util.Map;

public class MainTest {

    public static void main(String[] args){
        Symdiff x = new Symdiff(2.0);
        Symdiff f = x.add(x).mult(x).inv().neg();
        Map<String, Symdiff.Expression> ctx = new Hashtable<>();

        f.full_eval(ctx);

        f.pretty_print();
        System.out.print('\n');
        f.derivative("x").pretty_print();
    }
}
