#!/usr/bin/perl

# Small VAC auth server by beast.

use IO::Socket;
use POSIX 'strftime';

$lport = 27012;

srand;
while() {
  $server = IO::Socket::INET->new(Proto=>'udp', LocalPort=>$lport) || die "Error";
  $addr = $server->recv($data, 128);
  $server->close();
  ($size, $port, @_) = unpack("nnC4", $addr);
  $addr = join(".", @_);
  printf strftime "%d/%m/%Y - %H:%M:%S: VAC auth request from $addr:$port\n", localtime;

  ($ffff, $type, $auth) = unpack("NSN", $data);
  
  if ($ffff == 0xFFFFFFFF && $type == 0x4D) {
    $rndv = int(rand(0xFFFF));
    $data = pack("NSNSNS", 0xFFFFFFFF, 0x4E, $auth, 0x01, 0x00000000, $rndv);
    $server = IO::Socket::INET->new(Proto=>'udp', LocalPort=>$lport, PeerAddr=>$addr, PeerPort=>$port) || die "Error";
    $server->send($data);
    $server->close();
  }
}