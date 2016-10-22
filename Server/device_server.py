#coding: utf-8
#!/usr/bin/python

import ConfigParser
import SocketServer


#UDPサーバー割り込み処理
class MyUDPHandler(SocketServer.BaseRequestHandler):

    def handle(self):
        data = self.request[0].strip()
        user_list = data.split(",")
        device_user = user_list[0]
        passed_user = user_list[1:6]
        socket = self.request[1]
        print "device user ID:{}".format(device_user)
        print "passed user ID:{}".format(passed_user)


class DeviceServer:

    def __init__(self):
        self.inifile = ConfigParser.SafeConfigParser()
        self.inifile.read('./config.ini')

    def run(self):
        #アドレスとポート指定
        HOST = self.inifile.get('settings','host')
        PORT = self.inifile.getint('settings','port')
        #サーバー立ち上げ
        server = SocketServer.ThreadingUDPServer((HOST, PORT), MyUDPHandler)
        #割り込みループ
        server.serve_forever()

