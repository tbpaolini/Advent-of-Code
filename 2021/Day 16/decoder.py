from collections import deque, namedtuple
from typing import Dict, Tuple
from uuid import UUID, uuid4

Packet = namedtuple("Packet", "version type_id parent payload")

class Decoder():

    def __init__(self, data:str) -> None:
        
        raw_bytes = bytes.fromhex(data)
        self.bits = deque()
        for value in raw_bytes:
            self.bits.extend(
                (value & (1 << n)) >> n
                for n in range(7, -1, -1)
            )
        
        self.packets:Dict[UUID,Packet] = {}
        self.version_sum = 0
    
    def next_bits(self, amount:int) -> int:
        output = 0
        for bit in range(amount):
            try:
                output <<= 1
                output |= self.bits.popleft()
            except IndexError:
                raise StopDecoding("Reached the end of the bit stream.")
        
        return output
    
    def parse_packet(self, parent:Packet=None) -> Tuple[Packet, int]:
        
        uuid = uuid4()
        bit_count = 0

        version = self.next_bits(3)
        type_id = self.next_bits(3)
        bit_count += 6
        self.version_sum += version

        if type_id == 4:
            
            payload = 0
            separator = 1
            while separator == 1:
                separator = self.next_bits(1)
                payload <<= 4
                payload |= self.next_bits(4)
                bit_count += 5
        
        else:
            length_type = self.next_bits(1)
            bit_count += 1
            payload = []
            
            if length_type == 0:
                max_bits = self.next_bits(15)
                bit_count += 15
                decoded_size = 0
                while decoded_size < max_bits:
                    subpacket, packet_size = self.parse_packet(parent=uuid)
                    payload += [subpacket]
                    decoded_size += packet_size
                    bit_count += packet_size
                assert decoded_size == max_bits
            
            elif length_type == 1:
                max_packets = self.next_bits(11)
                bit_count += 11
                for _ in range(max_packets):
                    subpacket, packet_size = self.parse_packet(parent=uuid)
                    payload += [subpacket]
                    bit_count += packet_size
        
        packet = Packet(version, type_id, parent, payload)
        assert uuid not in self.packets
        self.packets[uuid] = packet
        return (packet, bit_count)
    
    def decode(self) -> None:
        
        while len(self.bits) > 10:
            try:
                self.parse_packet()
            except StopDecoding:
                break


class StopDecoding(Exception):
    """Signals that the decoding ended."""


if __name__ == "__main__":

    with open(r"C:\Users\Tiago\OneDrive\Documentos\Python\Projetos\Advent of code\2021\Day 16\input.txt", "rt") as file:
        raw_text = file.read().strip()
    
    transmission = Decoder(raw_text)
    transmission.decode()
    
    print(f"Part 1: {transmission.version_sum}")