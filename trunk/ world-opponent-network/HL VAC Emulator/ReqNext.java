import java.io.DataInputStream;
import java.io.PrintStream;
import java.net.DatagramPacket;

class ReqNext extends Req
{

    ReqNext(DatagramPacket datagrampacket)
        throws Exception
    {
        this(Req.getBuf(datagrampacket));
    }

    ReqNext(byte abyte0[])
        throws Exception
    {
        super(abyte0);
    }

    void read(DataInputStream datainputstream)
        throws Exception
    {
        super.read(datainputstream);
        if("NEXT".equals(super.Cmd))
            Pos = readInt(datainputstream);
        else
            super.legal = false;
    }

    void print()
    {
        super.print();
        System.out.println("Pos = " + Pos);
    }

    int Pos;
}
