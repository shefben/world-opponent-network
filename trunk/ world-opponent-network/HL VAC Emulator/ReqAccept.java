import java.io.DataInputStream;
import java.io.PrintStream;

class ReqAccept extends Req
{

    ReqAccept(byte abyte0[])
        throws Exception
    {
        super(abyte0);
    }

    void read(DataInputStream datainputstream)
        throws Exception
    {
        super.read(datainputstream);
        if("ACCEPT".equals(super.Cmd))
            Id = readInt(datainputstream);
        else
            super.legal = false;
    }

    void print()
    {
        super.print();
        System.out.println("Id = " + Integer.toHexString(Id));
    }

    int Id;
}
