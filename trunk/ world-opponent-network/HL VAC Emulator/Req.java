import java.io.*;
import java.net.DatagramPacket;

class Req
{

    static byte[] getBuf(DatagramPacket datagrampacket)
    {
        byte abyte0[] = new byte[datagrampacket.getLength()];
        System.arraycopy(datagrampacket.getData(), 0, abyte0, 0, datagrampacket.getLength());
        return abyte0;
    }

    Req(DatagramPacket datagrampacket)
        throws Exception
    {
        this(getBuf(datagrampacket));
    }

    Req(byte abyte0[])
        throws Exception
    {
        read(new DataInputStream(new ByteArrayInputStream(abyte0)));
    }

    int readInt(DataInputStream datainputstream)
        throws Exception
    {
        return datainputstream.readByte() & 0xff | (datainputstream.readByte() & 0xff) << 8 | (datainputstream.readByte() & 0xff) << 16 | (datainputstream.readByte() & 0xff) << 24;
    }

    short readShort(DataInputStream datainputstream)
        throws Exception
    {
        return (short)(datainputstream.readByte() & 0xff | (datainputstream.readByte() & 0xff) << 8);
    }

    String readString(DataInputStream datainputstream)
        throws Exception
    {
        byte abyte0[] = new byte[256];
        int i = 0;
        for(byte byte0 = 1; byte0 != 0;)
        {
            byte0 = datainputstream.readByte();
            abyte0[i] = byte0;
            i++;
        }

        return new String(abyte0, 0, i - 1);
    }

    void read(DataInputStream datainputstream)
        throws Exception
    {
        A = readInt(datainputstream);
        B = datainputstream.readByte();
        C = datainputstream.readByte();
        if(B != 77)
            Cmd = readString(datainputstream);
        else
            Cmd = "CheckAccess";
        legal = true;
    }

    void print()
    {
        System.out.println("A = " + A);
        System.out.println("B = " + B);
        System.out.println("C = " + C);
        System.out.println("Cmd = " + Cmd);
    }

    int A;
    int B;
    int C;
    String Cmd;
    boolean legal;
}
