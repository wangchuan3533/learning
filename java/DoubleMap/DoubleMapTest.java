import java.util.Random;
import java.util.concurrent.locks.ReentrantLock;
import java.util.Map;
import java.util.HashMap;
import java.util.Arrays;
class DoubleMap<K, V> {
	private ReentrantLock lock = new ReentrantLock();
	private Map<K, V> frontend, backend;

    public DoubleMap() {
        frontend = new HashMap<K, V>();
        backend = new HashMap<K, V>();
    }

	public void put(K k, V v) {
		lock.lock();
		frontend.put(k, v);
		lock.unlock();
	}

    public V get(K k) {
        return frontend.get(k);
    }

	public Map<K, V> getBackend() {
		lock.lock();
		Map<K, V> ret = backend;
		backend = frontend;
		frontend = new HashMap<K, V>();
		lock.unlock();
		return ret;
	}
}

class Tuple {
    private int[] values;

    public Tuple(int[] values) {
        this.values = values;
    }

    public int[] getValues() {
        return values;
    }

    public boolean incr(int index, int value) {
        if (index < 0 || index >= values.length) {
            return false;
        }
        values[index] += value;
        return true;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj instanceof Tuple) {
            Tuple t = (Tuple)obj;
            return Arrays.equals(this.values, t.values);
        }
        return false;
    }

    @Override
    public int hashCode() {
        return Arrays.hashCode(this.values);
    }


    @Override
    public String toString() {
        String str = "[";
        for (int v : values) {
            str = str + " " + v;
        }
        str = str + "]";
        return str;
    }
}

public class DoubleMapTest implements Runnable {
    static DoubleMap<Tuple, Tuple> map = new DoubleMap<Tuple, Tuple>();
    public void run() {
        Random rand = new Random();
        while (true) {
            int[] x = new int[3], y = new int[2];
            x[0] = rand.nextInt() % 5;
            x[1] = rand.nextInt() % 5;
            x[2] = rand.nextInt() % 5;
            y[0] = 1;
            y[1] = 1;

            Tuple k = new Tuple(x);
            Tuple v = map.get(k);
            if (v == null) {
                v = new Tuple(y);
                map.put(k, v);
            } else {
                v.incr(0, y[0]);
                v.incr(1, y[1]);
            }

            try {
                Thread.sleep(1);
            } catch (InterruptedException ex) {
                Thread.currentThread().interrupt();
            }
        }
    }

	public static void main(String[] Args) {
        DoubleMapTest test = new DoubleMapTest();
        Thread t = new Thread(test);
        t.start();
        while (true) {
            Map<Tuple, Tuple> toMerge = map.getBackend();
            for (Map.Entry<Tuple, Tuple> entry : toMerge.entrySet()) {
                Tuple k = entry.getKey(), v = entry.getValue();
                System.out.println(k + " => " + v);
            }
            try {
                Thread.sleep(1000);
            } catch (InterruptedException ex) {
                Thread.currentThread().interrupt();
            }
        }
	}
}
