import java.io.DataInputStream;
import java.io.PrintStream;
import java.net.DatagramPacket;

class ReqGet extends Req
{

    ReqGet(DatagramPacket datagrampacket)
        throws Exception
    {
        this(Req.getBuf(datagrampacket));
    }

    ReqGet(byte abyte0[])
        throws Exception
    {
        super(abyte0);
    }

    void read(DataInputStream datainputstream)
        throws Exception
    {
        super.read(datainputstream);
        if("GET".equals(super.Cmd))
        {
            Id = readInt(datainputstream);
            FileName = readString(datainputstream);
            D = readInt(datainputstream);
            TestDir = readString(datainputstream);
            Random = new int[3];
            Random[0] = Random[1] = Random[2] = 0;
            int i = 0;
            try
            {
                for(i = 0; i < 3; i++)
                    Random[i] = readInt(datainputstream);

            }
            catch(Exception exception)
            {
                System.out.println("                  Warning: read " + i + " randoms from 3");
            }
        } else
        {
            super.legal = false;
        }
    }

    void print()
    {
        super.print();
        System.out.println("Id = " + Id);
        System.out.println("FileName = " + FileName);
        System.out.println("D = " + D);
        System.out.println("TestDir = " + TestDir);
        System.out.println("Random1 = " + Random[0]);
        System.out.println("Random2 = " + Random[1]);
        System.out.println("Random3 = " + Random[2]);
    }

    int Id;
    String FileName;
    int D;
    String TestDir;
    int Random[];
}
