#coding: utf-8
#!/usr/bin/python

import ConfigParser
import SocketServer

inifile = ConfigParser.SafeConfigParser()
inifile.read('./config.ini')

#UDPサーバー割り込み処理
class MyUDPHandler(SocketServer.BaseRequestHandler):

    def handle(self):
        data = self.request[0].strip()
        socket = self.request[1]
        print "{} wrote:".format(self.client_address[0])
        print data
        socket.sendto("ok", self.client_address)


if __name__ == "__main__":

    #アドレスとポート指定
    HOST = inifile.get('settings','host')
    PORT = inifile.getint('settings','port')
    #サーバー立ち上げ
    server = SocketServer.UDPServer((HOST, PORT), MyUDPHandler)
    #割り込みループ
    server.serve_forever()
