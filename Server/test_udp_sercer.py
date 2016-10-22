#coding: utf-8
#!/usr/bin/python

import SocketServer

#UDPサーバー割り込み処理
class MyUDPHandler(SocketServer.BaseRequestHandler):

    def handle(self):
        data = self.request[0].strip()
        socket = self.request[1]
        print "{} wrote:".format(self.client_address[0])
        print data
        #socket.sendto(data.upper(), self.client_address)


if __name__ == "__main__":

    #アドレスとポート指定
    HOST, PORT = "192.168.2.103", 50000
    #サーバー立ち上げ
    server = SocketServer.UDPServer((HOST, PORT), MyUDPHandler)
    #割り込みループ
    server.serve_forever()
