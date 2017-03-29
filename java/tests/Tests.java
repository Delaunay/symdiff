package tests;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

import java.util.Hashtable;
import java.util.Map;

import symdiff.Symdiff;
import symdiff.Builder;
import symdiff.Symdiff.Expression;


public class Tests {

    @Test
    public void add(){
        Map<String, Expression> ctx = new Hashtable<>();
        assertEquals(7., (new Symdiff(4)).add(3).full_eval(ctx), 1e-6);
    }

    @Test
    public void ctx(){
        Map<String, Expression> ctx = new Hashtable<>();
        ctx.put("x", new Symdiff(4).toExpr());

        // --------------------------------------------------------------------
        Symdiff x = new Symdiff("x");
        Symdiff f = x.mult(2);

        // --------------------------------------------------------------------
        System.out.print("---\n Your function is:  ");
        f.pretty_print();
        System.out.println("\n---");

        // --------------------------------------------------------------------

        System.out.print("---\n The derivative is: ");
        f.derivative("x").pretty_print();
        System.out.println("\n---");

        // --------------------------------------------------------------------
        assertEquals(8., f.full_eval(ctx), 1e-6);
    }

}
