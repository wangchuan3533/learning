import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Properties;
class PropertyTest {
    public static void main(String[] args) {
        try {
            File f = new File("./hello");
            Properties p = new Properties();
            p.load(new FileInputStream(f));
            System.out.println(p.getProperty("hello").length());
        } catch (IOException e) {
            System.err.println(e);
        }
    }
}
