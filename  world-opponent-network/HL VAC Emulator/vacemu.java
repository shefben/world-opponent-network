/*
 * Half-Life secure server emulator by Tomas Janousek <tomi@nomi.cz>
 *
 * Based on JAD-decompiled sources of
 *   Secure server emulator v1.3.2 by Shtirlits boris@uchcom.botik.ru
 *   ( http://uchcom.botik.ru/boris/cs/index_e.html )
 */

import java.io.*;
import java.net.*;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Hashtable;

public class vacemu
{

    vacemu()
    {
        S = null;
        Connections = new Hashtable();
    }

    void serv(int i)
    {
        try
        {
            S = new DatagramSocket(i);
            S.setSoTimeout(5000);
        }
        catch(Exception exception)
        {
            exception.printStackTrace();
            if(S != null)
                try
                {
                    S.close();
                }
                catch(Exception exception2) { }
            return;
        }
        System.out.println(DF.format(new Date()) + "Waiting connections ...");
        do
            try
            {
                buf = new byte[256];
                P = new DatagramPacket(buf, buf.length);
                S.receive(P);
                Connection connection = (Connection)Connections.get(P.getAddress().toString() + P.getPort());
                if(connection == null)
                {
                    System.out.println(DF.format(new Date()) + "Connect from " + " (" + P.getAddress().getHostAddress() + ")");
                    connection = new Connection(this, P);
                    Connections.put(P.getAddress().toString() + P.getPort(), connection);
                    connection.start();
                } else
                {
                    connection.packet(P);
                }
            }
            catch(Exception exception1) { }
        while(true);
    }

    void closeConnection(Connection connection)
    {
        Connections.remove(connection.Address.toString() + connection.Port);
    }

    static void writeShort(int i, DataOutputStream dataoutputstream)
        throws Exception
    {
        dataoutputstream.writeByte(i & 0xff);
        dataoutputstream.writeByte(i >> 8 & 0xff);
    }

    static void writeInt(int i, DataOutputStream dataoutputstream)
        throws Exception
    {
        dataoutputstream.writeByte(i & 0xff);
        dataoutputstream.writeByte(i >> 8 & 0xff);
        dataoutputstream.writeByte(i >> 16 & 0xff);
        dataoutputstream.writeByte(i >> 24 & 0xff);
    }

    static byte[] Cmd_Challenge()
        throws Exception
    {
        ByteArrayOutputStream bytearrayoutputstream = new ByteArrayOutputStream();
        DataOutputStream dataoutputstream = new DataOutputStream(bytearrayoutputstream);
        writeInt(-1, dataoutputstream);
        dataoutputstream.writeByte(74);
        dataoutputstream.writeByte(0);
        dataoutputstream.writeBytes("CHALLENGE");
        dataoutputstream.writeByte(0);
        return bytearrayoutputstream.toByteArray();
    }

    static byte[] Cmd_Accept(int i)
        throws Exception
    {
        ByteArrayOutputStream bytearrayoutputstream = new ByteArrayOutputStream();
        DataOutputStream dataoutputstream = new DataOutputStream(bytearrayoutputstream);
        writeInt(-1, dataoutputstream);
        dataoutputstream.writeByte(75);
        dataoutputstream.writeByte(0);
        dataoutputstream.writeBytes("ACCEPT");
        dataoutputstream.writeByte(0);
        writeInt(i, dataoutputstream);
        return bytearrayoutputstream.toByteArray();
    }

    static byte[] Cmd_Abort(String s)
        throws Exception
    {
        ByteArrayOutputStream bytearrayoutputstream = new ByteArrayOutputStream();
        DataOutputStream dataoutputstream = new DataOutputStream(bytearrayoutputstream);
        writeInt(-1, dataoutputstream);
        dataoutputstream.writeByte(75);
        dataoutputstream.writeByte(0);
        dataoutputstream.writeBytes("ABORT");
        dataoutputstream.writeByte(0);
        dataoutputstream.writeBytes(s);
        dataoutputstream.writeByte(0);
        return bytearrayoutputstream.toByteArray();
    }

    static byte[] Cmd_Finish()
        throws Exception
    {
        ByteArrayOutputStream bytearrayoutputstream = new ByteArrayOutputStream();
        DataOutputStream dataoutputstream = new DataOutputStream(bytearrayoutputstream);
        writeInt(-1, dataoutputstream);
        dataoutputstream.writeByte(75);
        dataoutputstream.writeByte(0);
        dataoutputstream.writeBytes("FINISH");
        dataoutputstream.writeByte(0);
        return bytearrayoutputstream.toByteArray();
    }

    static byte[] Cmd_File(int i, byte abyte0[])
        throws Exception
    {
        ByteArrayOutputStream bytearrayoutputstream = new ByteArrayOutputStream();
        DataOutputStream dataoutputstream = new DataOutputStream(bytearrayoutputstream);
        writeInt(-1, dataoutputstream);
        dataoutputstream.writeByte(75);
        dataoutputstream.writeByte(0);
        dataoutputstream.writeBytes("FILE");
        dataoutputstream.writeByte(0);
        writeInt(i, dataoutputstream);
        dataoutputstream.write(abyte0, 0, abyte0.length);
        return bytearrayoutputstream.toByteArray();
    }

    static byte[] Cmd_Next(int i)
        throws Exception
    {
        ByteArrayOutputStream bytearrayoutputstream = new ByteArrayOutputStream();
        DataOutputStream dataoutputstream = new DataOutputStream(bytearrayoutputstream);
        writeInt(-1, dataoutputstream);
        dataoutputstream.writeByte(74);
        dataoutputstream.writeByte(0);
        dataoutputstream.writeBytes("NEXT");
        dataoutputstream.writeByte(0);
        writeInt(i, dataoutputstream);
        return bytearrayoutputstream.toByteArray();
    }

    static byte[] Cmd_Block(byte abyte0[])
        throws Exception
    {
        ByteArrayOutputStream bytearrayoutputstream = new ByteArrayOutputStream();
        DataOutputStream dataoutputstream = new DataOutputStream(bytearrayoutputstream);
        writeInt(-1, dataoutputstream);
        dataoutputstream.writeByte(75);
        dataoutputstream.writeByte(0);
        dataoutputstream.writeBytes("BLOCK");
        dataoutputstream.writeByte(0);
        writeShort(abyte0.length, dataoutputstream);
        dataoutputstream.write(abyte0, 0, abyte0.length);
        return bytearrayoutputstream.toByteArray();
    }

    static byte[] Cmd_Get(String s, int i, boolean flag)
        throws Exception
    {
        int j = (int)(Math.random() * 2147483647D);
        int k = (int)(Math.random() * 2147483647D);
        int l = (int)(Math.random() * 2147483647D);
        ByteArrayOutputStream bytearrayoutputstream = new ByteArrayOutputStream();
        DataOutputStream dataoutputstream = new DataOutputStream(bytearrayoutputstream);
        writeInt(-1, dataoutputstream);
        dataoutputstream.writeByte(74);
        dataoutputstream.writeByte(0);
        dataoutputstream.writeBytes("GET");
        dataoutputstream.writeByte(0);
        writeInt(i, dataoutputstream);
        dataoutputstream.writeBytes(s);
        dataoutputstream.writeByte(0);
        writeInt(0x43210004, dataoutputstream);
        if(flag)
            dataoutputstream.writeBytes(TestModuleDir);
        dataoutputstream.writeByte(0);
        writeInt(j, dataoutputstream);
        writeInt(k, dataoutputstream);
        writeInt(l, dataoutputstream);
        return bytearrayoutputstream.toByteArray();
    }

    static byte[] Cmd_AccessGranted(int i, byte byte0)
        throws Exception
    {
        ByteArrayOutputStream bytearrayoutputstream = new ByteArrayOutputStream();
        DataOutputStream dataoutputstream = new DataOutputStream(bytearrayoutputstream);
        writeInt(-1, dataoutputstream);
        dataoutputstream.writeByte(78);
        dataoutputstream.writeByte(0);
        writeInt(i, dataoutputstream);
        dataoutputstream.writeByte(byte0);
        dataoutputstream.writeByte(0);
        dataoutputstream.writeByte(0);
        dataoutputstream.writeByte(0);
        dataoutputstream.writeByte(0);
        dataoutputstream.writeByte(0);
        return bytearrayoutputstream.toByteArray();
    }

    static void PrintHexArr(byte abyte0[], int i)
    {
        for(int j = 0; j < i; j++)
            System.out.print(Integer.toHexString(abyte0[j] & 0xff) + " ");

        System.out.println();
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

    static void Print(DatagramPacket datagrampacket)
    {
        PrintArr(datagrampacket.getData(), datagrampacket.getLength());
    }

    static void PrintArr(byte abyte0[])
    {
        PrintArr(abyte0, abyte0.length);
    }

    static int Challenge(DatagramSocket datagramsocket, InetAddress inetaddress, int i)
        throws Exception
    {
        System.out.print("Sending the challenge request...");
        byte abyte0[] = Cmd_Challenge();
        datagramsocket.send(new DatagramPacket(abyte0, abyte0.length, inetaddress, i));
        try
        {
            Thread.sleep(50L);
        }
        catch(Exception exception) { }
        abyte0 = new byte[256];
        DatagramPacket datagrampacket = new DatagramPacket(abyte0, abyte0.length);
        datagramsocket.receive(datagrampacket);
        ReqAccept reqaccept = new ReqAccept(abyte0);
        if(!((Req) (reqaccept)).legal)
        {
            System.out.println("Connection rejected");
            Print(datagrampacket);
            throw new Exception();
        } else
        {
            System.out.println(" ok");
            return reqaccept.Id;
        }
    }

    static Module LoadModule(DatagramSocket datagramsocket, InetAddress inetaddress, int i, String s, boolean flag)
    {
        try
        {
            Module module = new Module();
            module.Name = s;
            int j = Challenge(datagramsocket, inetaddress, i);
            System.out.print("Sending the get request, file '" + s);
            if(flag)
                System.out.print(" [beta] ");
            byte abyte0[] = Cmd_Get(s, j, flag);
            datagramsocket.send(new DatagramPacket(abyte0, abyte0.length, inetaddress, i));
            try
            {
                Thread.sleep(50L);
            }
            catch(Exception exception2) { }
            System.out.print("' ...");
            abyte0 = new byte[256];
            DatagramPacket datagrampacket = new DatagramPacket(abyte0, abyte0.length);
            datagramsocket.receive(datagrampacket);
            System.out.println(" ok ");
            ReqFile reqfile = new ReqFile(abyte0);
            if(!((Req) (reqfile)).legal)
            {
                System.out.println("Can't download module " + s);
                return null;
            }
            module.Size = reqfile.Size;
            module.Header = reqfile.Header;
            module.Data = new byte[module.Size];
            Module module1 = null;
            try
            {
                module1 = new Module(s);
            }
            catch(Exception exception3)
            {
                module1 = null;
            }
            if(module1 != null && module1.compareHeader(module))
            {
                byte abyte1[] = Cmd_Abort("");
                datagramsocket.send(new DatagramPacket(abyte1, abyte1.length, inetaddress, i));
                System.out.println(s + " is already the newest version.");
                return null;
            }
            System.out.print("downloading data");
            int k = 0;
            do
            {
                byte abyte2[] = Cmd_Next(k);
                datagramsocket.send(new DatagramPacket(abyte2, abyte2.length, inetaddress, i));
                try
                {
                    Thread.sleep(50L);
                }
                catch(Exception exception4) { }
                abyte2 = new byte[16534];
                DatagramPacket datagrampacket1 = new DatagramPacket(abyte2, abyte2.length);

		try {
		    datagramsocket.receive(datagrampacket1);
		} catch (InterruptedIOException e) {
		    continue;
		}

                ReqBlock reqblock = new ReqBlock(abyte2);
                if(!((Req) (reqblock)).legal)
                {
                    if(!"FINISH".equals(((Req) (reqblock)).Cmd))
                        throw new Exception("Protocol violation");
                    break;
                }
                System.arraycopy(reqblock.Data, 0, module.Data, k, reqblock.Size);
                k += reqblock.Size;
                System.out.print(".");
            } while(true);
            if(k != module.Size)
            {
                throw new Exception("File size mismatch");
            } else
            {
                System.out.println("done");
                Module.decode(module.Data, j);
                return module;
            }
        }
        catch(Exception exception)
        {
            exception.printStackTrace();
        }
        if(datagramsocket != null)
            try
            {
                datagramsocket.close();
            }
            catch(Exception exception1) { }
        System.out.println("Can't download module " + s);
        return null;
    }

    static void LoadModule(String s, int i, String s1, boolean flag)
    {
        DatagramSocket datagramsocket = null;
        Object obj = null;
        System.out.println("Loading from " + s + ":" + i);
        try
        {
            datagramsocket = new DatagramSocket();
            datagramsocket.setSoTimeout(10000);
            InetAddress inetaddress = InetAddress.getByName(s);
            Module module = LoadModule(datagramsocket, inetaddress, i, s1, flag);
            if(module != null)
                module.write();
        }
        catch(Exception exception)
        {
            exception.printStackTrace();
            if(datagramsocket != null)
                try
                {
                    datagramsocket.close();
                }
                catch(Exception exception2) { }
            return;
        }
        try
        {
            datagramsocket.close();
        }
        catch(Exception exception1) { }
    }

    static byte[] sendReq(byte abyte0[], int i, String s, int j)
        throws Exception
    {
        DatagramSocket datagramsocket = new DatagramSocket(i);
        try
        {
            datagramsocket.setSoTimeout(30000);
            datagramsocket.send(new DatagramPacket(abyte0, abyte0.length, InetAddress.getByName(s), j));
            byte abyte1[] = new byte[512];
            DatagramPacket datagrampacket = new DatagramPacket(abyte1, abyte1.length);
            datagramsocket.receive(datagrampacket);
            byte abyte2[] = new byte[datagrampacket.getLength()];
            System.arraycopy(datagrampacket.getData(), 0, abyte2, 0, datagrampacket.getLength());
            datagramsocket.close();
            return abyte2;
        }
        catch(Exception exception)
        {
            try
            {
                datagramsocket.close();
            }
            catch(Exception exception1) { }
            throw exception;
        }
    }

    public static void main(String args[])
    {
        System.out.println("Half-Life secure server emulator by Tomas Janousek <tomi@nomi.cz>");
        System.out.println("Based on Secure server emulator v1.3.2 by Shtirlits boris@uchcom.botik.ru");
        System.out.println();
        String s = "half-life.speakeasy-nyc.hlauth.net";
        int i = 27012;
        char c = '\u6984';
        String s1 = "";
        if(args.length > 0)
            s1 = args[0];
        if("help".equalsIgnoreCase(s1))
        {
            System.out.println("      ");
            System.out.println("usage:");
            System.out.println("      ");
            System.out.println("      to start server:      java -jar vac.jar [port]");
            System.out.println("      ");
            System.out.println("      to load module:       java -jar vac.jar load filename [host port]");
            System.out.println("      ");
            System.out.println("      by default download from half-life.speakeasy-nyc.hlauth.net:27012");
            System.out.println("      ");
            System.out.println("      files:");
            System.out.println("         CStrike:            DoD:");
            System.out.println("      cstrike/ModuleC.dll      dod/ModuleC.dll");
            System.out.println("      cstrike/ModuleS.dll      dod/ModuleS.dll");
            System.out.println("      cstrike/ModuleS_i386.so  dod/ModuleS_i386.so");
            return;
        }
        if("load".equalsIgnoreCase(s1))
        {
            if(args.length < 2)
            {
                System.out.println("File name to download not specified.");
                return;
            }
            if(args.length == 4)
                try
                {
                    i = Integer.parseInt(args[3]);
                    s = args[2];
                }
                catch(Exception exception)
                {
                    i = 27012;
                    s = "half-life.speakeasy-nyc.hlauth.net";
                }
            LoadModule(s, i, args[1], false);
        } else
        {
            int j;
            try
            {
                j = Integer.parseInt(s1);
            }
            catch(Exception exception1)
            {
                j = 27012;
            }
            (new vacemu()).serv(j);
        }
    }

    static String TestModuleDir = "\\private\\vactest";
    static SimpleDateFormat DF = new SimpleDateFormat("dd.MM.yy HH:mm:ss ");
    byte buf[];
    DatagramSocket S;
    DatagramPacket P;
    Hashtable Connections;

}
