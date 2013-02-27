import java.io.DataInputStream;
import java.io.PrintStream;

class ReqFile extends Req
{

    ReqFile(byte abyte0[])
        throws Exception
    {
        super(abyte0);
    }

    void read(DataInputStream datainputstream)
        throws Exception
    {
        super.read(datainputstream);
        if("FILE".equals(super.Cmd))
        {
            Size = readInt(datainputstream);
            Header = new byte[34];
            datainputstream.read(Header);
        } else
        {
            super.legal = false;
        }
    }

    static void PrintArr(byte abyte0[], int i)
    {
        for(int j = 0; j < i; j++)
        {
            byte byte0 = abyte0[j];
            if(byte0 < 32 || byte0 > 122)
                System.out.print(Integer.toHexString(byte0 & 0xff) + " ");
            else
                System.out.print("'" + (char)byte0 + "' ");
        }

        System.out.println();
    }

    void print()
    {
        super.print();
        System.out.println("Size = " + Size + " " + Integer.toHexString(Size));
        System.out.println("Header size=" + Header.length);
        PrintArr(Header, Header.length);
    }

    int Size;
    byte Header[];
}
