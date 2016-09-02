import java.util.Random;
public class Prob {

    public Prob(int[] probs) {
        for(int i = 0; i < probs.length; i++) {
            sum += probs[i];
        }
        this.probs = probs;
    }
    
    protected Random random = new Random(System.currentTimeMillis());
    
    protected int[] probs;
    protected int sum = 0;
    
    public int randomSelect() {
        int rand = random.nextInt(sum);
        for(int k = 0; k < probs.length; k++) {
            rand = rand - probs[k];
            if(rand < 0) {
                return k;
            }
        }
        return 0;
    }
    
    public int[] getProbClone() {
        return probs.clone();
    }
    
    public int[] randomPick(int n) {
        if (n > probs.length) {
            return null;
        }
        int[] result = new int[n];
        int[] probs = this.probs.clone();
        int sum = this.sum;
        for (int i = 0; i < n; i++) {
            int rand = random.nextInt(sum);
            for (int k = 0; k < probs.length; k++) {
                rand = rand - probs[k];
                if (rand < 0) {
                    result[i] = k;
                    sum -= probs[k];
                    probs[k] = 0;
                    break;
                }
            }
        }
        return result;
    }

    public static void main(String[] args) {
        Prob prob = new Prob(new int[]{1, 2, 3, 4, 5, 6});
        int[] list = prob.randomPick(5);
        System.out.println(list[0] + " " + list[1] + " " + list[2] + " " + list[3] + " " + list[4]);
//        System.out.println(prob.randomSelect());
    }
}
