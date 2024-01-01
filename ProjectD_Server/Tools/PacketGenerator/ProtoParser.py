class ProtoParser():
    def __init__(self, start_id, recv_prefix, send_prefix):
        self.recv_pkt = [] # ���� ��Ŷ ���
        self.send_pkt = [] # �۽� ��Ŷ ���
        self.total_pkt = [] # ��ü ��Ŷ ���
        self.start_id = start_id # ��Ŷ ID ���� ��ȣ
        self.id = start_id # ��Ŷ ���� ID
        self.recv_prefix = recv_prefix # ���� ��Ŷ ���λ�
        self.send_prefix = send_prefix # �۽� ��Ŷ ���λ�
        
    def parse_proto(self, path):
        f = open(path, 'r')
        lines = f.readlines()
        
        for line in lines:
            if line.startswith('message') == False:
                continue
            
            pkt_name = line.split()[1].upper()
            if pkt_name.startswith(self.recv_prefix):
                self.recv_pkt.append(Packet(pkt_name, self.id))
            elif pkt_name.startswith(self.send_prefix):
                self.send_pkt.append(Packet(pkt_name, self.id))
            else:
                continue
            
            self.total_pkt.append(Packet(pkt_name, self.id))
            self.id += 1
            
        f.close()

class Packet:
    def __init__(self, name, id):
        self.name = name
        self.id = id
   




