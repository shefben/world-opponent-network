import java.io.DataInputStream;
import java.io.PrintStream;
import java.net.DatagramPacket;

class ReqCheckAccess extends Req
{

    ReqCheckAccess(DatagramPacket datagrampacket)
        throws Exception
    {
        this(Req.getBuf(datagrampacket));
    }

    ReqCheckAccess(byte abyte0[])
        throws Exception
    {
        super(abyte0);
    }

    void read(DataInputStream datainputstream)
        throws Exception
    {
        super.A = readInt(datainputstream);
        super.B = datainputstream.readByte();
        super.C = datainputstream.readByte();
        super.legal = super.A == -1 && super.B == 77 && super.C == 0;
        Id = readInt(datainputstream);
    }

    void print()
    {
        super.print();
        System.out.println("Id = " + Integer.toHexString(Id));
    }

    int Id;
}
