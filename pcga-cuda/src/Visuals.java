import java.awt.*;
import java.io.*;
import javax.swing.*;
import java.util.*;

public class Visuals {
    private static final int WINDOW_SIZE = 1024;
    private static int VERTEX_RADIUS = 4;
    private static int EDGE_WIDTH = 1;

    private static ArrayList<Vertex> vertices = new ArrayList<>();
    private static ArrayList<Edge> edges = new ArrayList<>();
    private static ArrayList<Color> colors = new ArrayList<>();

    static class Vertex {
        int id;
        int color;
        int x, y;

        public Vertex(int id, int color) {
            this.id = id;
            this.color = color;
            this.x = (int) (Math.random() * WINDOW_SIZE);
            this.y = (int) (Math.random() * WINDOW_SIZE);
        }
    }

    static class Edge {
        Vertex u, v;
    }

    static class PgcaMain extends JFrame{
        PgcaPanel drawing = new PgcaPanel();

        PgcaMain() {
            Container content = this.getContentPane();
            content.setLayout(new BorderLayout());
            
            content.add(drawing, BorderLayout.CENTER);
            
            this.setTitle("Parallel Graph Coloring Algorithm - Visuals");     
            this.pack();
        }

    }

    static class PgcaPanel extends JPanel {
        public PgcaPanel() {
            setPreferredSize(new Dimension(WINDOW_SIZE, WINDOW_SIZE));
            setBackground(Color.white);
        }

        public void paintComponent(Graphics g) {
            super.paintComponent(g);
            for (Edge edge : edges) {
                drawEdge(g, edge.u.x, edge.u.y, edge.v.x, edge.v.y);
            }

            for (Vertex vertex : vertices) {
                drawVertex(g, vertex.x, vertex.y, VERTEX_RADIUS, colors.get(vertex.color));
            }
        }

        public void drawVertex(Graphics cg, int xCenter, int yCenter, int r, Color color) {
            cg.setColor(Color.BLACK);
            cg.drawOval(xCenter-r, yCenter-r, 2*r, 2*r);
            cg.setColor(color);
            cg.fillOval(xCenter-r, yCenter-r, 2*r, 2*r);
        }

        public void drawEdge(Graphics cg, int x1, int y1, int x2, int y2) {
            cg.setColor(Color.LIGHT_GRAY);
            ((Graphics2D) cg).setStroke(new BasicStroke(EDGE_WIDTH));
            cg.drawLine(x1, y1, x2, y2);
        }
    }

    public static void main(String[] args) {
        colors.add(Color.BLACK);
        colors.add(Color.RED);
        colors.add(Color.BLUE);
        colors.add(Color.GREEN);
        colors.add(Color.YELLOW);
        colors.add(Color.ORANGE);
        colors.add(Color.PINK);
        colors.add(Color.CYAN);
        colors.add(Color.MAGENTA);
        colors.add(Color.LIGHT_GRAY);
        colors.add(Color.DARK_GRAY);

        try {
            File colorFile = new File(args[1]);
            Scanner colorReader = new Scanner(colorFile);

            int vertexId = 0;
            int colorId = 0;
            while (colorReader.hasNextLine()) {
                String data = colorReader.nextLine();
                colorId = Integer.parseInt(data);
                Vertex vertex = new Vertex(vertexId, colorId);
                vertices.add(vertex);
                vertexId++;
            }
            colorReader.close();
        } catch (FileNotFoundException e) {
            System.out.println("Color file read error.");
            e.printStackTrace();
        }

        try {
            File edgeFile = new File(args[0]);
            Scanner edgeReader = new Scanner(edgeFile);
            edgeReader.nextLine();

            while (edgeReader.hasNextLine()) {
                String data = edgeReader.nextLine();
                String[] edgeVertices = data.split(" ");
                Edge edge = new Edge();
                edge.u = vertices.get(Integer.parseInt(edgeVertices[0]));
                edge.v = vertices.get(Integer.parseInt(edgeVertices[1]));
                edges.add(edge);
            }
            edgeReader.close();
        } catch (FileNotFoundException e) {
            System.out.println("Edge file read error.");
            e.printStackTrace();
        }

        JFrame myWindow = new PgcaMain();
        myWindow.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        myWindow.setVisible(true);
    }
}
