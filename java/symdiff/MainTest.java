package symdiff;

public class MainTest {
    public static void main(String[] args){
        Symdiff x = new Symdiff(2.0);
        Symdiff f = x.add(x).mult(x).inv().neg();

        f.full_eval();

        f.pretty_print();
        System.out.print('\n');
        f.derivative().pretty_print();
    }
}
