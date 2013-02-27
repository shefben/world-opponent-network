import java.io.DataInputStream;
import java.io.PrintStream;

class ReqBlock extends Req
{

    ReqBlock(byte abyte0[])
        throws Exception
    {
        super(abyte0);
    }

    void read(DataInputStream datainputstream)
        throws Exception
    {
        super.read(datainputstream);
        if("BLOCK".equals(super.Cmd))
        {
            Size = readShort(datainputstream);
            Data = new byte[Size];
            datainputstream.read(Data);
        } else
        {
            super.legal = false;
        }
    }

    void print()
    {
        super.print();
        System.out.println("Size = " + Size);
    }

    int Size;
    byte Data[];
}
